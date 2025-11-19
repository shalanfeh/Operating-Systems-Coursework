#ifndef BOUNDED_BUFFER_HW_H
#define BOUNDED_BUFFER_HW_H

#include "semaphore.h"            /* Semaphore.init, Semaphore.wait, Semamphore.post */
#define BUFSIZE 10                /* Default buffer size */

class BoundedBufferHW {
 public:
  BoundedBufferHW( int bufsize ); /* Create an empty, bounded, shared FIFO buffer with n slots */
  ~BoundedBufferHW( );            /* Deconstruct a given bounded buffer */
  void insert( int item );        /* Insert an item into the rear of shared buffer */
  int remove( );                  /* Remove and return the first item from buffer sp */
 private:
  Semaphore sem_m, sem_f, sem_e;  /* semaphores, each bound to mutex, full, and empty */
  int *buf;                       /* Buffer array */
  int n;                          /* Maximum number of slots */
  int front;                      /* buf[(front+1)%n] is first item */
  int rear;                       /* buf[rear%n] is last item */
  int mutex;                      /* Protects accesses to buf */
  int full;                       /* Counts available slots: full == 0 means no more space. */
  int empty;                      /* Counts available items: empty == 0 means no more data. */
};

#endif // BOUNDED_BUFFER_HW_H

