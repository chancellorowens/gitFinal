#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>


/*
using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::stringstream;
*/
using namespace std;

void repl();
void close_pipe(int pipefd [2]);

int main()
{
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  repl();
  return EXIT_SUCCESS;
}

void repl(){
  int failsafe = 0;
  while(failsafe < 10){
    failsafe++;
    cout.setf(std::ios::unitbuf);
    char dott = '.';
    char * dot = &dott;
    char path [PATH_MAX];
    realpath(dot, path);

    cout << "1730sh:" << path << "$ ";

    stringstream ss;
    string fullInput;
    getline(cin,fullInput);
    ss << fullInput;
    string current[128];
    char * args[16][64];
    string stdin = "STDIN_FILENO";
    string stdout = "STDOUT_FILENO";
    string stderr = "STDERR_FILENO";
    int argnum = 0;
    int maxargnum = 0;
    int pipes = 0;
    int processes = 0;
    int skip = 0;
    for(int i = 0; i < 128; i++){
      if (skip != 0){
	i++;
      }
      if(ss >> current[i]){
	if((current[i].compare("|")   == 0) || (current[i].compare("<")   == 0) ||
	   (current[i].compare(">")   == 0) || (current[i].compare(">>")  == 0) ||
	   (current[i].compare("e>")  == 0) || (current[i].compare("e>>") == 0) ){
	  if (current[i].compare("|")   == 0){
	    pipes++;
	    processes++;
	    argnum = 0;
	  }
	  if (current[i].compare("<") == 0){
	    skip = 1;
	  }
	  if (current[i].compare(">") == 0){
	    skip = 2;
	  }
	  if (current[i].compare(">>") == 0){
	    skip = 3;
          }
	  if (current[i].compare("e>") == 0){
	    skip = 4;
	  }
	  if (current[i].compare("e>>") == 0){
	    skip = 5;
            stderr = current[i];
	    stderr += " (append)";
	  }
	}
	else{
	  if(skip != 0){
	    if (skip == 1){
	      stdin = current[i];
	      skip = 0;
	    }
	    if (skip == 2){
	      stdout = current[i];
	      stdout += " (truncate)";	      
	      skip = 0;
	    }
	    if (skip == 3){
	      stdout = current[i];
	      stdout += " (append)";
	      skip = 0;
	    }
	    if (skip == 4){
	      stderr = current[i];
	      stderr += " (truncate)";
	      skip = 0;	      
	    }
	    if (skip == 5){
	      stderr = current[i];
	      stderr += " (append)";
	      skip = 0;
	    }
	  }
	  else{
	    args[processes][argnum] = (char*)current[i].c_str();
	    args[processes][argnum+1] = NULL;
	    argnum++;
	    maxargnum++;
	  }
	}
      }
    }
    string bi = current[0];
    if(bi == "" || bi == "exit" || bi == "help" || bi == "cd")
      {
	if(bi == "exit"){
	  exit(EXIT_SUCCESS);
	}
	if(bi == "help"){
	  cout << "\nHello, and welcome to the 1730 shell!\nWe hope we can provide you with an immersive shell experience like no other!\nType exit to quit\nType help to view this screen again\nType cd to change directory\nFeel free to use any other unix commands you know!\n" << endl;
	}
	if(bi == "cd"){
	  //char * cd =  current[1];
	}

      }
    else
      {
	/*
	  cout << endl
	  << "Job STDIN  = " << stdin << endl
	  << "Job STDOUT = " << stdout << endl
	  << "Job STDERR = " << stderr << endl << endl
      
	  << pipes << " pipe(s)" << endl
	  << processes+1 << " process(es)" << endl << endl;
	*/	

	char * proc[16];
	int pipefd [2];
	pid_t pid;//,wpid;
	int pipe10 = 1;
	for(int i = 0; i <= processes; i++){
	  for(int j = 0; j <= maxargnum;j++){
	    proc[j] = NULL;
	  }
	  for(int j = 0; j <= maxargnum;j++){
	    proc[j] = args[i][j];
	  }
	  if((pid = fork()) == -1){
	    perror("FORK ERROR");
	  }
	  else if (pid == 0){//in child	
	    if (dup2(pipefd[(pipe10 % 2)], STDOUT_FILENO) == -1) {
	      perror("dup2");
	      exit(EXIT_FAILURE);
	    } // if
	    else{
	      pipe10++;

	      if (execvp(proc[0],proc) == -1)
		{
		  perror("exec");
		}
	      close_pipe(pipefd);
	    }
	  }//child
	  else if(pid > 0){//in parent		       
	    waitpid(pid, nullptr, 0);
	  }
	}//for
      }//else
  }//while
}//repl

void close_pipe(int pipefd [2]) {
  if (close(pipefd[0]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  } // if
  if (close(pipefd[1]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  } // if
} // close_pipe
