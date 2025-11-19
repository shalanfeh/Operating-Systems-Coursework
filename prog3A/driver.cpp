#include <pthread.h>            /* pthread_create, pthread_join */
#include <iostream>             /* cout, cerr, endl */
#include <stdlib.h>             /* atoi */
#include <time.h>               /* clock */
#include "bounded_buffer_hw.h"
#include "bounded_buffer_os.h"

#define MAX 10

int mode = 0;
int nItems = MAX;
int print = 0;
BoundedBufferHW hBuf( 100 );
BoundedBufferOS oBuf( 100 );

/* Thread routines */
void *producer( void *arg ) {
  
  for ( int i = 0; i < nItems; i++ ) {
    switch( mode ) {
    case 1: hBuf.insert( i ); break;
    case 2: oBuf.insert( i ); break;
    default: cerr << "wrong mode" << endl; exit( -1 );
    }
  }
  return NULL;
}

void *consumer( void *arg ) {

  for ( int i = 0; i < nItems; i++ ) {
    int item = -1;
    switch( mode ) {
    case 1: item = hBuf.remove( ); break;
    case 2: item = oBuf.remove( ); break;
    default: cerr << "wrong mode" << endl; exit( -1 );
    }
    if ( print != 0 )
      cout << item << endl;
  }
  return NULL;
}

/* main() thread */
int main( int argc, char **argv ) {
  if ( argc != 4 ) {
    cerr << "usage: driver M I P" << endl;
    cerr << "       where M = 1 (HW), 2 (OS)" << endl;
    cerr << "             I = #items" << endl;
    cerr << "             P = 0: no printf, 1: printf" << endl;
    return -1;
  }
  mode = atoi( argv[1] ); // 1: HW, 2: OS, 3: Language
  if ( atoi( argv[2] ) > MAX ) nItems = atoi( argv[2] );
  print = atoi( argv[3] );

  // create a cpu set with core 1
  cpu_set_t cpuset_1;
  CPU_ZERO(&cpuset_1);
  CPU_SET(1, &cpuset_1);

// create a cpu set with core 2
  cpu_set_t cpuset_2;
  CPU_ZERO(&cpuset_2);
  CPU_SET(2, &cpuset_2);

  // start the timer
  clock_t start = clock( );
  /* Create threads and wait for them to finish */
  pthread_t tid1, tid2;
  pthread_create( &tid1, NULL, producer, NULL );
  pthread_create( &tid2, NULL, consumer, NULL );

  // Set the affinity to thread 1
  if ( pthread_setaffinity_np(tid1, sizeof(cpu_set_t), &cpuset_1) != 0 ) {
    cerr << "pthread_set_affinity_np, s1" << endl;
    return -1;
  }
  
  // set the affinity to thread 2
  if ( pthread_setaffinity_np(tid2, sizeof(cpu_set_t), &cpuset_2) != 0 ) {
    cerr << "pthread_set_affinity_np, s2" << endl;
    return -1;
  }
  
  pthread_join( tid1, NULL );
  pthread_join( tid2, NULL );
  // finish the timer
  clock_t end = clock( );

  printf( "time = %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC );
}

