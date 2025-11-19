#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <unistd.h>   // sleep( )
#include <sys/time.h> // gettimeofday
#include <iostream>   // cout

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;

//================
class Table2 {
public:
  Table2( ) {
    pthread_mutex_init(&lock, NULL); //same from Table1

    //initialize glutton mental states to 'thinking'
    for (int i = 0; i < PHILOSOPHERS; i++) {
      state[i] = THINKING;
      pthread_cond_init(&self[i], NULL);
    }
  }


  //critical section
  void pickup( int i ) {
    pthread_mutex_lock(&lock); //obtain the lock

    state[i] = HUNGRY; //want to feast

    test(i); //attempt to start eating
    if (state[i] != EATING) { //I can't eat yet, test failed. left or right eating.
      pthread_cond_wait(&self[i], &lock); //wait until someone calls test on i and it passes
    }

    cout << "philosopher[" << i << "] picked up chopsticks" << endl; //FEAST

    pthread_mutex_unlock(&lock); //release the lock
  }

  //critical section
  void putdown( int i ) {
    pthread_mutex_lock(&lock); //obtain the lock

    state[i] = THINKING; //ooga booga time

    //pass the chopsticks to left and right
    test((i+1)%5);
    test((i+4)%5);

    cout << "philosopher[" << i << "] put down chopsticks" << endl;

    pthread_mutex_unlock(&lock); //release the lock
  }

private:
  enum { THINKING, HUNGRY, EATING } state[PHILOSOPHERS];
  pthread_mutex_t lock;
  pthread_cond_t self[PHILOSOPHERS];

  //left = (i+1)%5
  //right = (i+4)%5
  //tests if philosopher i can eat, and if it can, frees it from wait() and sets it's status to eating.
  void test( int i ) {
    //if I'm hungry,
    //and if left is not eating,
    //and if right is not eating, i can feast!
    if ((state[i] == HUNGRY) && (state[(i+1)%5] != EATING) && (state[(i+4)%5] != EATING)) {
      //start feasting
      state[i] = EATING;

      //notify i that they can break free from the wait in pickup()
      pthread_cond_signal(&self[i]);
    }
  }

};
//================

//================
//Allows each philosopher to lock the entire table before picking up chopsticks
//and unlock the table after putting down the chopsticks.
//The logic is correct but has no concurrency, (i.e., runs slowest).
class Table1 {
public:
  Table1( ) {
    // initialize the mutex lock
    pthread_mutex_init( &lock, NULL );
  }
  void pickup( int i ) {
    // lock by yourself
    pthread_mutex_lock( &lock );
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown( int i ) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
    // unlock by yourself
    pthread_mutex_unlock( &lock );
  }

private:
  // define a mutex lock
  pthread_mutex_t lock;
};
//================

//================
class Table0 {
public:
  void pickup( int i ) {
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown( int i ) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
  }
};
//================

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void *philosopher( void *arg ) {
  int id = *(int *)arg;
  
  for ( int i = 0; i < MEALS; i++ ) {
    switch( table_id ) {
    case 0:
      table0.pickup( id );
      sleep( 1 );
      table0.putdown( id );
      break;
    case 1:
      table1.pickup( id );
      sleep( 1 );
      table1.putdown( id );
      break;
    case 2:
      table2.pickup( id );
      sleep( 1 );
      table2.putdown( id );
    break;
    }
  }
  return NULL;
}

int main( int argc, char** argv ) {
  pthread_t threads[PHILOSOPHERS];
  pthread_attr_t attr;
  int id[PHILOSOPHERS];
  table_id = atoi( argv[1] );

  pthread_attr_init(&attr);
  
  struct timeval start_time, end_time;
  gettimeofday( &start_time, NULL );
  for ( int i = 0; i < PHILOSOPHERS; i++ ) {
    id[i] = i;
    pthread_create( &threads[i], &attr, philosopher, (void *)&id[i] );
  }

  for ( int i = 0; i < PHILOSOPHERS; i++ )
    pthread_join( threads[i], NULL );
  gettimeofday( &end_time, NULL );

  sleep( 1 );
  cout << "time = " << ( end_time.tv_sec - start_time.tv_sec ) * 1000000 + ( end_time.tv_usec - start_time.tv_usec ) << endl;

  return 0;
}
