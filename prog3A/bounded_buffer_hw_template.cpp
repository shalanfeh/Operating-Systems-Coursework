#include "bounded_buffer_hw.h"
#include <iostream>

/* Create an empty, bounded, shared FIFO buffer with n slots */
BoundedBufferHW::BoundedBufferHW( int bufsize ) {
  if ( bufsize <= 0 )
    bufsize = BUFSIZE;
  buf = new int[bufsize];
  n = bufsize;              /* Buffer holds max of n items */
  front = rear = 0;         /* Empty buffer iff front == rear */
  sem_m.init(&mutex, 0, 1); /* Binary semaphore for locking */
  sem_f.init(&full, 0, n);  /* Initially, buf has n available slots */
  sem_e.init(&empty, 0, 0); /* Initially, buf has zero data items */
}

/* Clean up buffer sp */
BoundedBufferHW::~BoundedBufferHW( ) {
  delete buf;
}

/* Insert item onto the rear of shared buffer sp */
//will be spammed -- producer
void BoundedBufferHW::insert( int item ) {
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
  sem_e.wait(&empty);
  sem_m.wait(&mutex);

  rear = (rear + 1) % n;
  buf[rear] = item;

  sem_m.post(&mutex);
  sem_f.post(&full);
 }

/* Remove and return the first item from buffer sp */
//will be spammed -- consumer
int BoundedBufferHW::remove( )
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
  sem_f.wait(&full);
  sem_m.wait(&mutex);

  front = (front + 1) % n;
  int item = buf[front];

  sem_m.post(&mutex);
  sem_e.post(&empty);
 return item;
 }

