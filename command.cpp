#include <unistd.h>
#include <string>
#include <signal.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <list>
#include <sstream>
#include <time.h>


pid_t pid;

int command_launch(std::string frst_wrd_command, std::string rst_command)
{
  pid_t wpid;
  int status;
  time_t iniTime, execTime;

  pid = fork();
  if (pid == 0) {
    // Child process

    std::istringstream ss(rst_command);
    std::string arg;
    std::list<std::string> ls;
    std::vector<char*> v;

    ls.push_back(frst_wrd_command);                           // Program name in first position
    v.push_back(const_cast<char*>(ls.back().c_str()));

    while (ss >> arg)                                         // Construct vector word by word, does not support quoting
    {                                                         // Quotes should be processes as one only argument, needs improvement
       ls.push_back(arg);
       v.push_back(const_cast<char*>(ls.back().c_str()));
    }
    v.push_back(0);  // need terminating null pointer

    if (execv(v[0], &v[0]) == -1) {                           // Execv program, first argument is program name, second, the complete command
      std::cerr << "Problem executing command" << std::endl;  // including the program name
    }

    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    std::cerr << "Problem executing command: error in the forking process" << std::endl;
  } else{
    //Parent process waiting for a signal. Either timer or execution.
    std::string response="";
    int killed=0;
    int wait_seconds = 5;
    time(&iniTime);
    do {
      usleep(100);
      wpid = waitpid(pid, &status, WNOHANG);
      time(&execTime);
      if (execTime-iniTime >= wait_seconds){
        std::cout << wait_seconds << " seconds have passed, do you want to kill the process? (y/n): ";
        std::getline (std::cin, response);
        if(response == "y"){
          kill(pid, 15);
          killed=1;
        } else {
          wait_seconds += 5;
        }
      }

    } while (!WIFEXITED(status) && !killed);
  }
  return 1;
}