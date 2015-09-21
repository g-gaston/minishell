#ifndef ALIAS_H
#define ALIAS_H

#define ALIAS_FILE_PATH ".alias"
#define ALIAS_SIZE 32

typedef std::tuple<std::string,std::string> alias_tuple;

std::vector<alias_tuple> &insert_alias(const std::string &s, char delim, std::vector<alias_tuple> &elems, int from_file);
std::vector<alias_tuple> read_alias();
void print_alias(std::vector<alias_tuple> alias);
int is_alias(std::string command, std::vector<alias_tuple> alias_list);

#endif
