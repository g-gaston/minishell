#include <unistd.h>
#include <string>
#include <signal.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

pid_t pid;
typedef void sigfunc(int);

//Signal handler
void sig_handler(int sig)
{
  std::string response;
    if (sig == SIGALRM){
        std::cout << "5 seconds have passed, do you want to kill the process? (y/n): ";
        std::cin >> response;
        if(response == "y")
          kill(pid, 9);
      }
}


int command_launch(std::string frst_wrd_command, std::string rst_command)
{
  pid_t wpid;
  int status;

  //Registering the handler in the kernel
  if (signal(SIGALRM, sig_handler) == SIG_ERR)
#ifdef DEBUG 
      std::cout << "\ncan't catch SIGALRM\n" << std::endl;
#endif

  pid = fork();
  if (pid == 0) {
    // Child process
    int len = rst_command.length();

    //create the correct type of pointer and point it to an array of chars:
    char* const myArr = new char[len + 1];

    //copy string into the array:
    for(int i=0; i < len; i++)
    {
      myArr[i] = rst_command[i];
    }
    myArr[len]='\0';
  
  //create the array of pointers required by the function:
    char* const argv[] = {myArr};

    if (execv(frst_wrd_command.c_str(), argv) == -1) {
      std::cerr << "Problem executing command" << std::endl;
    }

    std::cout << "Comando ejecutado" << frst_wrd_command << std::endl;
    //clean up:
    delete [] myArr;
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    std::cerr << "Problem executing command: error in the forking process" << std::endl;
  } else{
    //Parent process waiting for a signal. Either timer or execution.
    alarm(5);
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}