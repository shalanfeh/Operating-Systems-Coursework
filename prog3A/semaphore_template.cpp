#include <iostream>

#include "semaphore.h"

int Semaphore::init( int *sem, int pshared, int value ) {
  *sem = value;
  return 0;
}

//I feel like I didn't need to do double while loops here and
//could have just used a while into if statement, but I'll follow the pseudo-code provided
//on the board in class here.
//Decrements the semaphore -- uses busy waiting to get into the lock
int Semaphore::wait( int *sem ) {
  //busy wait
  while (true) {
    while (atomic_flag_test_and_set(&lock)) {
      ;
    }
    //we have the lock here
    if (*sem > 0) {
      //decrement it to signify entering
      *sem -= 1;
      //unlock the atomic lock
      atomic_flag_clear(&lock);

      //break out of the wait function
      return 0;
    } else {
      //if we can't enter the semaphore (sem == 0) or by technicality (sem < 0)
      //unlock the atomic lock
      atomic_flag_clear(&lock);

      //back to waiting hell
      continue;
    }
  }
}

//Increments the semaphore -- uses busy waiting to get into the lock
int Semaphore::post( int *sem ) {
  while (true) {
    //busy wait
    while (atomic_flag_test_and_set(&lock)) {
      ;
    }
    //we have the lock now

    //increment it to signify leaving
    *sem += 1;

    //unlock the atomic lock
    atomic_flag_clear(&lock);

    //break out of the wait function
    return 0;
  }
}
