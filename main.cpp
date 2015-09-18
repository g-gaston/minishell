#include <string>
#include <iostream>
#include <unistd.h>

int main (int argc, char **argv) {

	// Process profile file
	// Read programs in PATH
	// Change current directory to HOME

	//Process alias file

	std::string frst_wrd_command;	// First command's word
	std::string rst_command;		// Rest of the command
	char cwd[1024]; 				// Current working directory max length is 1024

	while (true) {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			std::cout << std::string(cwd) << "$ "; 	// Print prompt
			std::cin >> frst_wrd_command; 			// Read user command first word
			std::getline (std::cin, rst_command);	// Read rest of user command

			if (frst_wrd_command == "quit" ||
				frst_wrd_command == "Quit" ||
				frst_wrd_command == "QUIT") {

				return 0;
			} else if (0) { // Alias definition
				/* code */
			} else if (0) { // Alias usage from definitions
				/* code */
			} else {		// Should be a program from PATH

			}
		}

	}

}