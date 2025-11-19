#ifndef BOUNDED_BUFFER_OS_H
#define BOUNDED_BUFFER_OS_H

#include <semaphore.h>            /* sem_init, sem_wait, sem_post */
#define BUFSIZE 10                /* Default buffer size */

class BoundedBufferOS {
 public:
  BoundedBufferOS( int bufsize ); /* Create an empty, bounded, shared FIFO buffer with n slots */
  ~BoundedBufferOS( );            /* Deconstruct a given bounded buffer */
  void insert( int item );        /* Insert an item into the rear of shared buffer */
  int remove( );                  /* Remove and return the first item from buffer sp */
 private:
  int *buf;                       /* Buffer array */
  int n;                          /* Maximum number of slots */
  int front;                      /* buf[(front+1)%n] is first item */
  int rear;                       /* buf[rear%n] is last item */
  sem_t mutex;                    /* Protects accesses to buf */
  sem_t full;                     /* Counts available slots: full == 0 measn no more space. */
  sem_t empty;                    /* Counts available items: empty == 0 means no more data. */  
};

#endif // BOUNDED_BUFFER_OS_H

