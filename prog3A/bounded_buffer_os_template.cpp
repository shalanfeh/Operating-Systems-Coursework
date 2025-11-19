#include "bounded_buffer_os.h"

/* Create an empty, bounded, shared FIFO buffer with n slots */
BoundedBufferOS::BoundedBufferOS( int bufsize ) {
  if ( bufsize <= 0 )
    bufsize = BUFSIZE;
  buf = new int[bufsize];
  n = bufsize;            /* Buffer holds max of n items */
  front = rear = 0;       /* Empty buffer iff front == rear */
  sem_init(&mutex, 0, 1); /* Binary semaphore for locking */
  sem_init(&full, 0, n);  /* Initially, buf has n available slots */
  sem_init(&empty, 0, 0); /* Initially, buf has zero data items */
}

/* Clean up buffer sp */
BoundedBufferOS::~BoundedBufferOS( ) {
  delete buf;
}

/* Insert item onto the rear of shared buffer sp */
//will be spammed -- producer
void BoundedBufferOS::insert( int item ) {
  //from empty to full
  //as full -> 0, empty -> n

  /*semaphore function: sem_f:wait //we have space
   *sem_m:wait //be the only one editing the buffer
   *
   *add data into the buffer
   *
   *sem_m:post //release the buffer
   *sem_e:post //tell consumers to consume
   */
  sem_wait(&empty);
  sem_wait(&mutex);

  rear = (rear + 1) % n;
  buf[rear] = item;

  sem_post(&mutex);
  sem_post(&full);
}

/* Remove and return the first item from buffer sp */
//will be spammed -- consumer
int BoundedBufferOS::remove( )
{
  //from full to empty
  //as full -> 0, empty -> n

  /*semaphore function: sem_e:wait //there is data
   *sem_m:wait //be the only one editing the buffer
   *
   *add data into the buffer
   *
   *sem_m:post //release the buffer
   *sem_f:post //tell producers to produce
   */
  sem_wait(&full);
  sem_wait(&mutex);

  front = (front + 1) % n;
  int item = buf[front];

  sem_post(&mutex);
  sem_post(&empty);
  return item;
}

