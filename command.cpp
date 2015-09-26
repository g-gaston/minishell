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

int command_launch(std::string command, bool alarm) {
  pid_t wpid;
  int status, result;
  time_t iniTime, execTime;

  pid = fork();
  if (pid == 0) {
    // Child process

    std::istringstream ss(command);
    std::string arg;
    std::string arg_quot = "";
    std::list<std::string> ls;
    std::vector<char*> v;

    while (ss >> arg) {                                       // Construct vector word by word, support quoting
      if (arg[0] == '"' && arg_quot == "") {
        arg_quot = arg.substr(1, arg.size()-1);
        continue;
      }

      if (arg.back() == '"' && arg_quot != "") {
        arg = arg_quot + " " + arg.substr(0, arg.size()-1);
      } else if (arg.back() != '"' && arg_quot != "") {
        arg_quot += " " + arg;
        continue;
      }

       ls.push_back(arg);
       v.push_back(const_cast<char*>(ls.back().c_str()));
       arg_quot = "";
    }
    v.push_back(0);  // need terminating null pointer
    // Anadido redirect
/*    fpos_t pos;
    fgetpos(stdout, &pos);
    int fw=open("ia.txt", O_APPEND|O_WRONLY);
    dup2(fw, 1);
    //fclose(fw);
    // Anadido redirect
    //dup2(1,1);
    close(fw);
    fsetpos(stdout, &pos);
    // Close anadido redirect
*/    // Close anadido redirect
    if (execv(v[0], &v[0]) == -1) {                           // Execv program, first argument is program name, second, the complete command
      std::cerr << "Problem executing command" << std::endl;  // including the program name
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    std::cerr << "Problem executing command: error in the forking process" << std::endl;
  } else {
    //Parent process waiting for a signal. Either timer or execution.
    std::string response="";
    int killed=0;
    int wait_seconds = 5;
    time(&iniTime);
    do {
      usleep(100);
      wpid = waitpid(pid, &status, WNOHANG);
      time(&execTime);
      if (alarm && execTime-iniTime >= wait_seconds){
        kill(pid, SIGSTOP);
        std::cout << wait_seconds << " seconds have passed, do you want to kill the process? (y/n): ";
        std::getline (std::cin, response);
        kill(pid, SIGCONT);
        if(response == "y"){
          kill(pid, 15);
          killed=1;
        } else {
          time(&execTime);
          wait_seconds = execTime - iniTime + 5; //Increment from the passed time
        }
      }

    } while (!WIFEXITED(status) && !killed);
  }

  return WEXITSTATUS(status);
}
