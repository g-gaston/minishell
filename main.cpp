#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <pwd.h>
#include "program.h"
#include "command.h"
#include "alias.h"
// Anadido redirec
#include <unistd.h>
#include <fcntl.h>
// Anadido redirec

#ifndef PROFILE_FILE_PATH
#define PROFILE_FILE_PATH ".shell_profile"
#endif

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<Program> &listprograms(const char *name, int level, std::vector<Program> &programs) {
    DIR *dir;
    struct dirent *entry;
    char *complete_path;
    Program program;

    if (!(dir = opendir(name)))
        return programs;
    if (!(entry = readdir(dir)))
        return programs;

    char path[1024];

    do {
        if (entry->d_type == DT_DIR) {
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            listprograms(path, level + 1, programs);
        }
        else {
        	snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
        	if (! access(path, X_OK)) {
        		program.set(entry->d_name, path);
        		programs.push_back(program);
        	}
        }
    } while ((entry = readdir(dir)));
    closedir(dir);
    return programs;
}

int find_and_exec(std::vector<Program> &programs, std::string frst_wrd_command,
				  std::string rst_command, bool alarm) {

	std::vector<Program>::iterator i;
	for (i = programs.begin(); i != programs.end(); ++i) {
	    if ((*i).get_name() == frst_wrd_command) {
	    	break;
	    }
	}
	if (i != programs.end()) {
		// Call here the program in "(*i).get_path()"
		//The execvp() look for the command in the PATH, so maybe this part could be reduced
		if(command_launch((*i).get_path() + " " + rst_command, alarm) != 1){
			std::cerr << "Problem executing the command " << frst_wrd_command << std::endl;
			return -1;
		}
		return 0;
	} else {
		std::cout << frst_wrd_command << ": program not found " << std::endl;
		return -1;
	}
}

void process_comand (std::string &frst_wrd_command, std::string &rst_command, std::string complete_command) {
	std::istringstream aux_stream(complete_command);
	std::getline(aux_stream, frst_wrd_command, ' ');	// Get user command first word
	std::getline(aux_stream, rst_command);
	return;
}

int main (int argc, char **argv) {

	// Get user home dir
	std::string home = std::string(getpwuid(getuid())->pw_dir) + "/";

	// Process profile file
	std::ifstream profile_file(home + PROFILE_FILE_PATH);
	std::vector<std::string> paths;
	std::vector<Program> programs;
	bool alarm = true;

	if (!profile_file) {
		std::cerr << "I can't read the profile " << PROFILE_FILE_PATH << "." << std::endl;
		return -1;
	}

	std::string line;
	for (int line_no = 1; std::getline(profile_file, line); ++line_no) {
		if (line.substr(0,5) == "PATH=") {
			split(line.substr(5,line.size()-1), ':', paths);
			for (std::vector<std::string>::const_iterator i = paths.begin(); i != paths.end(); ++i) {
			    // Read programs in PATH
			    listprograms((*i).c_str(), 0, programs);
			}
		} else if (line.substr(0,5) == "HOME=") {	// Get HOME work dir if specified in profile file
			home = line.substr(5,line.size()-1);
			// Change current directory to HOME
			if (chdir(home.c_str()) != 0) {
				std::cerr << "Problem opening home directory: " << home << std::endl;
				return -1;
			}
		} else if (line == "ALARM=OFF") {
			alarm = false;
		}
	}

	//Process alias file
  	std::vector<alias_tuple> alias;
	std::string alias_path = home + ALIAS_FILE;
  	alias = read_alias(alias_path);

	std::string frst_wrd_command;	// First command's word
	std::string rst_command;		// Rest of the command
	char cwd[1024]; 				// Current working directory max length is 1024

  // Anadido redireccion
  fpos_t pos;
  fgetpos(stdout, &pos);
  int fw = -1;
  // Anadido redireccion
	while (true) {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			frst_wrd_command = "";
			rst_command = "";
			std::cout << std::string(cwd) << "$ "; 				// Print prompt
			std::getline (std::cin, rst_command);				// Read user command
			std::istringstream aux_stream (rst_command);
			while (aux_stream.peek() == ' ') { 					// Skip spaces at beginning
				aux_stream.get();
			}
			std::getline(aux_stream, frst_wrd_command, ' ');	// Get user command first word
			rst_command = "";
			std::getline(aux_stream, rst_command);				// Get rest of user command

			if (frst_wrd_command.size() == 0) {					// Skip if empty line
				continue;
			}

			if (frst_wrd_command == "quit" ||
				frst_wrd_command == "Quit" ||
				frst_wrd_command == "QUIT") {

				return 0;
			} else if (frst_wrd_command == "alias") { // Alias definition
				if (rst_command == "")
					print_alias(alias);
				else
        	insert_alias(rst_command, '=', alias, alias_path, 0);
			} else if (is_alias(frst_wrd_command, alias) >= 0) { // Alias usage from definitions
        		int alias_elem = is_alias(frst_wrd_command, alias);
				std::string alias_command = std::get<1>(alias[alias_elem]);
				int space = alias_command.find(" ");
        		frst_wrd_command = alias_command.substr(0, space);
				rst_command = alias_command.substr(space+1); // + rst_command;
			}
      // Anadido redirection 
      if (rst_command.find(">") > 0) {
        std::vector<std::string> rest_cmd_redir;
        split(rst_command, '>', rest_cmd_redir);
        std::string file_path = rest_cmd_redir.at(0);
        rst_command = rest_cmd_redir.at(1);
        fw=open("ia.txt", O_APPEND|O_WRONLY);
      } else {
        if (fw >= 0) {
          dup2(fw, 1);
          close(fw);
          fsetpos(stdout, &pos);
        }   
      }
      // Anadido redirection
      if (frst_wrd_command == "cd") {
				if (rst_command.size() == 0) {
					if (chdir(home.c_str()) != 0) {
						std::cerr << "Problem opening directory: " << home << std::endl;
					}
				} else {
					if (chdir(rst_command.c_str()) != 0) {
						std::cerr << "Problem opening directory: " << rst_command << std::endl;
					}
				}
			} else if (frst_wrd_command == "if") {
				std::vector<std::string> if_commands;
				aux_stream.clear();
				aux_stream.seekg (0, aux_stream.beg);
				aux_stream.str(rst_command);
				std::getline(aux_stream, rst_command, ';'); // First command
				if_commands.push_back(rst_command);
				aux_stream >> rst_command;
				if (rst_command != "then") {
					/* error, malformed if then command */
					std::cerr << "error, malformed if-then command, \"then\" not detected, instead " << rst_command << std::endl;
					continue;
				}
				aux_stream.get(); // Skip space
				std::getline(aux_stream, rst_command, ';'); // Second command
				if_commands.push_back(rst_command);
				aux_stream >> rst_command;
				if (rst_command != "else") {
					/* error, malformed if then command */
					std::cerr << "error, malformed if-then command: \"else\" not detected, instead " << rst_command << std::endl;
					continue;
				}
				aux_stream.get(); // Skip space
				aux_stream >> frst_wrd_command;
				if (frst_wrd_command == "fi") {
					std::cerr << "error, malformed if-then command: \"fi\" detected, instead of third command" << std::endl;
					continue;
				}
				while (aux_stream >> rst_command) {
					if (rst_command != "fi") {
						frst_wrd_command = frst_wrd_command + " " + rst_command;
					} else {
						break;
					}
				}
				if (rst_command != "fi") {
					/* error, malformed if then command */
					std::cerr << "error, malformed if-then command: \"fi\" not detected, instead " << rst_command << std::endl;
					continue;
				}
				if_commands.push_back(frst_wrd_command);	// Third command
				// Call here subprocess with those arguments
				frst_wrd_command = "";
				rst_command = "";
				process_comand(frst_wrd_command, rst_command, if_commands[0]);
				if (find_and_exec(programs, frst_wrd_command, rst_command, alarm) == 0) {
					process_comand(frst_wrd_command, rst_command, if_commands[1]);
				} else {
					process_comand(frst_wrd_command, rst_command, if_commands[2]);
				}
				find_and_exec(programs, frst_wrd_command, rst_command, alarm);
			} else {	   // Should be a program from PATH
				find_and_exec(programs, frst_wrd_command, rst_command, alarm);
			}
		}

	}

}
