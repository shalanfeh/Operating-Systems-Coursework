#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit

#include <iostream>      // for cout

using namespace std;

int main( int argc, char** argv ) {
  int fds[2][2]; //pipe theory
  int pid;

  if ( argc != 2 ) {
    cerr << "Usage: processes command" << endl;
    exit( -1 );
  }

  // fork a child 0_0
  if ( ( pid = fork( ) ) < 0 ) {
    perror( "fork error" );
  }
  else if ( pid == 0 ) {
    // I'm a child
      //create a pipe with fd[0]
    pipe(fds[0]);

    //read from grandchild (pipe 0)
    dup2(fds[0][0], 0);

    //=============

    //fork
    pid = fork();
    if ( pid < 0 ) {
      perror( "fork error" );
    } else if (pid == 0) {
      //I'm a grandchild

      //close read end to child (pipe 0)
      close(fds[0][0]);

        //create a pipe to grandchild with fd[1]
      pipe(fds[1]);
      dup2(fds[1][0], 0); //read from great-grandchild (pipe 1)

      //========================

      //fork
      pid = fork();
      if ( pid < 0 ) {
        perror( "fork error" );
      } else if (pid == 0) {
        //I'm a great-grandchild

        //close read to grandchild
        close(fds[1][0]);

        //dup2 to grandchild's write
        dup2(fds[1][1], 1);

        //execute ps
        execlp("ps", "ps", "-A", NULL);
        exit(-1); //error
      }

      //========================

      //close write to grandchild (pipe 1)
      close(fds[1][1]);

      //write to child (pipe 0)
      dup2(fds[0][1], 1);

      //wait for great-grandchild to complete
      wait(NULL);

      //execute grep
      execlp("grep", "grep", argv[1], NULL);
      exit(-1); //error
    }

    //=============

    //close write end of pipe(0)
    close(fds[0][1]);

    //wait for grandchild to complete
    wait(NULL);

    //execute wc
    execlp("wc", "wc", "-l", NULL);
    exit(-1); //error
  }
  else {
    // I'm a parent
    wait( NULL );
    cout << "commands completed" << endl;
  }
}
