#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>

class Program {
	std::string name;
	std::string path;
public:
	Program();
	Program(std::string n, std::string p);
	void set(std::string n, std::string p);
	void set_name(std::string n);
	void set_path(std::string p);
	std::string get_name();
	std::string get_path();
};

#endif