#include <stdio.h>                 // printf, scanf                                                       
#include <string.h>                // bzero, strcmp                                                       
#include <stdlib.h>                // exit                                                                
#include <unistd.h>                // fork, execlp
#include <sys/types.h>             // wait                                                                
#include <sys/wait.h>              // wait
#include <iostream>                // cout, cint
#include <string>                  // c++ string, getline
#include <cstring>                 // strtok
#include <stack>                   // c++ stack

#define MAX_TOKENS 100             // the max # of tokens

using namespace std;

int main( ) {
  pid_t pid = 0;                    // a child process id to be returned from fork( )
  stack<pid_t> pid_stack;           // a pid stack of children in background
  string fullLine;                  // stores an fullLine from the keyboard.
  int status;                       // receives a termination status from a child
  char* token[MAX_TOKENS];          // tokenize fullLine
  bool background = false;          // true if a command is given with &

  while(true) {
    //"exit" command will break out of the loop
    printf( "$ " );                 // print out a shell prompt
    getline( cin,  fullLine );      // read one line
    if ( fullLine.empty( ) )        // go back to the top of while
      continue;

    //if last character == &, bg. Else, fg
    background = false; //reset incase of a prior &
    if (fullLine.at(fullLine.length()-1) == '&') {
      background = true;
    } //else {its a foreground process}

    // if the last character is '&' or ';', replace it with a space ' '.
    //why? because & and ; isn't a part of the command. its strictly for the shell to know
    //where to run the command (fg or bg)
    if (fullLine.at(fullLine.length()-1) == '&' or fullLine.at(fullLine.length()-1) == ';') {
      fullLine.pop_back( );
      fullLine.push_back(' ');
    }

    // tokenize fullLine with a space.
    const char* delimiter = " "; //where to break
    token[0] = strtok( (char *)fullLine.c_str( ), delimiter); 
    for ( int i = 0; token[i] != NULL && i < 80; i++ )
      token[i + 1] = strtok(NULL, delimiter);

    // token[0] is a command.

    //===implementation details for commands 'exit' and 'fg'
    // check if it's "exit".
    if (strcmp(token[0], "exit") == 0) {
      exit(0);
    }

    // check if it's "fg" in which case wait for the pid on the stack top. then, pop it out.
    if (strcmp(token[0], "fg") == 0 and !(pid_stack.empty( )) ) {
      waitpid(pid_stack.top(), &status, 0);
      pid_stack.pop( );
      continue;
    }

    //===implementation details for general commands
    // fork a child shell           pid_t fork(void)
    pid_t child_id = fork( );

    // child should replace it(execvp) with token[0](file) [as possing the entire token](???).
    if (child_id == -1) { //error case
      perror("fork");
      return 1;
    } else if (child_id == 0) { //child case
      //execvp(args[0], args)
      //execvp(file, the entire command including file)
      execvp(token[0], token); //overwrite child shell with command

      //according to google, this is only reachable is execvp dies.
      //which makes sense because execvp is supposed to overwrite the process
      perror("execvp");
      return 1;
    } else {
      //parent case
      if (background) { //background processes go into the pid_stack
        pid_stack.push(child_id);
      } else {
        // parent should wait for the child if !background. (!background is fg)
        waitpid(child_id, &status, 0);
      }
    }
  }
  return 0;
}
