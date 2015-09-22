#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include "program.h"
#include "command.h"


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

	std::string frst_wrd_command;	// First command's word
	std::string rst_command;		// Rest of the command
	char cwd[1024]; 				// Current working directory max length is 1024

	while (true) {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			std::cout << std::string(cwd) << "$ "; 	// Print prompt
			std::cin >> frst_wrd_command; 			// Read user command first word
			std::getline (std::cin, rst_command);	// Read rest of user command
			if (rst_command.size() > 0) {
				rst_command = rst_command.substr(1, rst_command.size()-1); // Delete space
			}

			if (frst_wrd_command == "quit" ||
				frst_wrd_command == "Quit" ||
				frst_wrd_command == "QUIT") {

				return 0;
			} else if (0) { // Alias definition
				/* code */
			} else if (0) { // Alias usage from definitions
				/* code */
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
					//The execvp() look for the command in the PATH, so maybe this part could be reduce
					if(command_launch((*i).get_path() + frst_wrd_command,rst_command) != 1){
						std::cerr << "Problem executing the command " << frst_wrd_command << std::endl;
					}




				} else {
					std::cout << frst_wrd_command << ": program not found " << std::endl;
				}

			}
		}

	}

}