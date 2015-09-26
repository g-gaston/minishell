#ifndef COMMANDS_H
#define COMMANDS_H

#include <unistd.h>
#include <string>
#include <signal.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

typedef void sigfunc(int);

void sig_handler(int sig);
int command_launch(std::string command, bool alarm);

#endif