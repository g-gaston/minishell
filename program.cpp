#include "program.h"

Program::Program() {
	name = "";
	path = "";
}

Program::Program(std::string n, std::string p) {
	set(n, p);
}

void Program::set(std::string n, std::string p) {
	set_name(n);
	set_path(p);
}

void Program::set_name(std::string n) {
	name = n;
}

void Program::set_path(std::string p) {
	path = p;
}

std::string Program::get_name() {
	return name;
}

std::string Program::get_path() {
	return path;
}