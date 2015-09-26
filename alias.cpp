#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <fstream>
#include "alias.h"

// TODO Meter alias.reserve? Tamaño de cada tuple? Si se mete, sustituir alias.size() por variable que lleve la cuenta
// Tratar comando alias (unir frst_wrd_command y rst_command y volver a separarlos?)

int check_alias (std::string new_alias,std::string new_command, std::vector<alias_tuple> alias_list);

std::vector<alias_tuple> &insert_alias(const std::string &s, char delim, std::vector<alias_tuple> &elems, std::string alias_path, int from_file) {
  // Get words from line
  std::stringstream ss(s);
  std::string items[2];
  if ((int)s.find("=") > 0) {
    for (int i = 0; i < 2; i++) {
      std::getline(ss, items[i], delim);
    }
		// Remove " or ' when needed
		if (items[1].size() > 0) {
	  	if (items[1].at(0) == '"' | items[1].at(0) == '\'')
				items[1] = items[1].substr(1, items[1].size());
			if (items[1].at(items[1].size()-1) == '"' | items[1].at(items[1].size()-1) == '\'')
				items[1] = items[1].substr(0, items[1].size()-1);
		}
	  // Check alias and update vector (do nothing if check_alias returns error)
	  int check = check_alias(items[0], items[1], elems);
	  // Element repeated
	  if (check > 0) {
	    elems.erase(elems.begin()+check-1);
	    std::ofstream ofs;
	    ofs.open(alias_path, std::ofstream::out | std::ofstream::trunc);
	    for (int i = 0; i < elems.size(); i++) {
	      ofs << std::get<0>(elems[i]) << "=\"" << std::get<1>(elems[i]) << "\"\n";
	    }
	    ofs.close();
	  }
	  // Add element
	  if (check >= 0) {
	    elems.push_back(alias_tuple(items[0], items[1]));
			// If we're not loading the aliases from the file, we update the file
	    if (!from_file) {
	      std::ofstream out(alias_path, std::ios::app);
	      out << items[0] << "=\"" << items[1] << "\"\n";
	      out.close();
	    }
    }
  } else {
    std::cout << "Malformed alias command" << std::endl;

  }
  return elems;
}


int check_alias (std::string new_alias, std::string new_command, std::vector<alias_tuple> alias_list) {
  // Avoid loops with aliases
  for (int i = 0; i < alias_list.size(); i++) {
    if (new_alias == std::get<1>(alias_list[i]) | new_command == std::get<0>(alias_list[i])) {
      std::cerr << "Loophole avoided" << std::endl;
      return -1;
    }
  }
  // Repeated element
  for (int i = 0; i < alias_list.size(); i++) {
    if (new_alias == std::get<0>(alias_list[i])) {
#ifdef DEBUG      
      std::cout << "removed row: " << i+1 << std::endl;
#endif
      return i+1;
    }
  }
  // Alias list full
  if (alias_list.size() >= ALIAS_SIZE) {
    std::cerr << "Alias list full. Cannot add anymore." << std::endl;
    return -1;
  }
  return 0; 

}

void print_alias(std::vector<alias_tuple> alias) {
  std::cout << "Alias vector size: " << alias.size() << std::endl;
  for (alias_tuple t: alias) {
    std::cout << std::get<0>(t) << ' ';
    std::cout << std::get<1>(t) << std::endl;
  }
}

std::vector<alias_tuple> read_alias(std::string alias_path) {
  std::vector<alias_tuple> alias;
  std::ifstream alias_file(alias_path);
  
  if (!alias_file) {
    std::cerr << "Alias file not found" << std::endl;
    //return -1;
  }
  
  std::string line;
  for (int i = 0; i < ALIAS_SIZE; i++) {
    if (alias_file.eof())
      break;
    std::getline(alias_file, line);
    if (!line.empty()) {
      insert_alias(line, '=', alias, alias_path, 1);
    }
  } 

  return alias;
}

int is_alias(std::string command, std::vector<alias_tuple> alias_list) {
  for (int i = 0; i < alias_list.size(); i++) {
    if (command == std::get<0>(alias_list[i])) 
      return i;
  }
  return -1;
}

/*
int main() {
  std::vector<alias_tuple> alias;
  std::string frst_wrd_command;
  std::string rst_command;    // Rest of the command
  
  alias = read_alias();
  
  while(true) {
    std::cout << "$>";
    std::cin >> frst_wrd_command;
    std::getline (std::cin, rst_command); // Read rest of user command

#ifdef DEBUG    
    print_alias(alias);
#endif

    if (frst_wrd_command == "alias") {
      insert_alias(rst_command.substr(1, rst_command.size()-1), '=', alias, 0);

#ifdef DEBUG
      print_alias(alias);
#endif

    } else if (is_alias(frst_wrd_command, alias) >= 0) {
      int alias_elem = is_alias(frst_wrd_command, alias);
      frst_wrd_command = std::get<1>(alias[alias_elem]);
      std::cout << frst_wrd_command << std::endl;
    } else {
      std::cout << "not alias command" << std::endl;
    }
  }
  return 0;
}
*/



