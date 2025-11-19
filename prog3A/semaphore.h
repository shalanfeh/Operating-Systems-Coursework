#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <atomic>     // atomic_flag

using namespace std;

class Semaphore {
 public:
  int init( int *sem, int pshared, int value );
  int wait( int *sem );
  int post( int *sem );
 private:
  atomic_flag lock = ATOMIC_FLAG_INIT;
};

#endif // SEMAPHORE_H
