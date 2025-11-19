#include <signal.h>   // kill
#include <unistd.h>   // usleep
#include <iostream>   // cout, cerr, endl
#include <valarray>
#include <cmath> // pow

#include "scheduler.h"

Scheduler::Scheduler( int quantum ) {
  msec = ( quantum > 0 ) ? quantum : DEFAULT_QUANTUM;
}

bool Scheduler::addProcess( int pid ) {
  if ( kill( pid, SIGSTOP ) != 0 ) {
    cerr << "process[" << pid << "] can't be paused." << endl;
    return false;
  }
  queue[0].push( pid ); // enqueue this process
  return true;
}

void Scheduler::schedulerSleep( ) {
  usleep( msec );
  cerr << "scheduler: completed " << (++nQuantums) << " quantums" << endl;
}

void Scheduler::run_rr( ) {
  cerr << "scheduler (round robin): started" << endl;
  int current = 0;

  while ( true ) {
    if ( queue[0].size( ) == 0 )         // no more processes to terminate scheduler
      break;
    current = queue[0].front( );         // pick up the first process from the queue

    if ( kill( current, 0 ) == 0 ) {      // current process is alive
      cerr << "\nscheduler: resumed " << current << endl;
      kill( current, SIGCONT );           // run it for a next quantum
      schedulerSleep( );
      kill( current, SIGSTOP );
    }

    queue[0].pop( );
    if ( kill( current, 0 ) == 0 ) {     // current process is still alive
      queue[0].push( current );
    }
    else { // current process is dead
      cerr << "scheduler: confirmed " << current << "'s termination" << endl;
      current = 0; 
    }
  }
  cerr << "scheduler: has no more process to run" << endl;
}

void Scheduler::run_mfq( ) {
  cerr << "scheduler (multilevel feedback queue): started" << endl;
  int current = 0;  // current pid
  int previous = 0; // previous pid
  
  int slices[3];                          // slice[i] means that level i's current time slice.
  for ( int i = 0; i < 3; i++ ) {
    slices[i] = 0;                        // all levels start slice 0.
  }

  while ( true ) {
    int level = 0;

    //Sets current and level variables
    for (; level < 3; level++ ) {
      // if the current level's slice is 0.
      if (slices[level] == 0) {
        // check if the current level queue is empty.
        if (queue[level].size( ) == 0 ) {
          // if so, go to a next lower level queue.
          continue; //next iteration of for loop
        }
        // otherwise, pick up a pid from this queue
        current = queue[level].front( );
        break; //escape for loop
      } else {// current is in a middle of slices 0, 1, or 2
        current = previous;
        break; //out of for loop
      } //end of if statement
    } //end of for loop

    // if we reached level 3, (i.e., the lowest level) and found no processes to schedule
    if ((current == 0) && (level == 3)) {
      // finish scheduler.cpp
      break; //escape while loop
    }

    // check if a process to run is still active.
    if (kill( current, 0 ) == 0 ) { //returns 0 if active
      //if we're running a different process than last quantum, print it out
      if (current != previous) {
        cerr << "\nscheduler: resumed " << current << endl;
      }
      //cerr << "level: " << level << endl;
      //resume it, call schedulerSleep( ) to give a time quantum.
      kill( current, SIGCONT );           // run it for a next quantum
      schedulerSleep( );
      // then, suspends it.
      kill( current, SIGSTOP );
    }

    // check if this process is still active.
    if (kill( current, 0 ) == 0 ) {
      // increment slices by 1
      slices[level] += 1;
      //if slice for this level is greater than the threshold, do something.
      //threshold = 2^level (1 for level 0, 2 for level 1, 4 for level 2)
      if (slices[level] >= pow(2, level)) {
        //cerr << "slices: " << slices[level] << "| threshold: " << int(2^level) << endl;
        slices[level] = 0; //reset level's slices
        if (level < 2) { //queue's 0 and 1
          //go to the next level queue
          queue[level].pop( );
          queue[level+1].push(current);
        } else { //queue 2
          //go to the back of the queue
          queue[level].pop( );
          queue[level].push(current);
        }
      }
    } else {
      //prepare level for next task
      slices[level] = 0;
      queue[level].pop( );
      cerr << "scheduler: confirmed " << current << "'s termination" << endl;
    }

    //update previous for next iteration
    previous = current;
  } //end of while loop
  cerr << "scheduler: has no more process to run" << endl;
}
