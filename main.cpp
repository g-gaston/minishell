#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <tuple>
#include "program.h"
#include "alias.h"

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

int main (int argc, char **argv) {

	// Process profile file
	std::ifstream profile_file(PROFILE_FILE_PATH);
	std::vector<std::string> paths;
	std::string home = "/home/";
	std::vector<Program> programs;

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
		} else if (line.substr(0,5) == "HOME=") {
			home = line.substr(5,line.size()-1);
			// Change current directory to HOME
			if (chdir(home.c_str()) != 0) {
				std::cerr << "Problem opening home directory: " << home << std::endl;
				return -1;
			}
		}
	}

	//Process alias file
  	std::vector<alias_tuple> alias;
	std::string alias_path = home + ALIAS_FILE;
  	alias = read_alias(alias_path);

	std::string frst_wrd_command;	// First command's word
	std::string rst_command;		// Rest of the command
	char cwd[1024]; 				// Current working directory max length is 1024

	while (true) {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
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
			} else if (frst_wrd_command == "cd") {
				if (rst_command.size() == 0) {
					if (chdir(home.c_str()) != 0) {
						std::cerr << "Problem opening directory: " << home << std::endl;
					}
				} else {
					if (chdir(rst_command.c_str()) != 0) {
						std::cerr << "Problem opening directory: " << rst_command << std::endl;
					}
				}
			} else {	   // Should be a program from PATH
				std::vector<Program>::iterator i;
				for (i = programs.begin(); i != programs.end(); ++i) {
				    if ((*i).get_name() == frst_wrd_command) {
				    	std::cout << "Found program " << (*i).get_name() << " in " << (*i).get_path() << std::endl;
				    	break;
				    }
				}
				if (i != programs.end()) {
					// Call here the program in "(*i).get_path()"
				} else {
					std::cout << frst_wrd_command << ": program not found " << std::endl;
				}

			}
		}

	}

}
