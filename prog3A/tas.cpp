#include <atomic>     // atomic_flag
#include <iostream>   // cerr
#include <stdlib.h>   // atoi
#include <pthread.h>  // pthread_create, pthread_join

using namespace std;

atomic_flag lock = ATOMIC_FLAG_INIT;

static int cnt = 0;

void *inc( void* arg ) {
  int tid = *(int *)arg;
  while ( true ) {
    while ( atomic_flag_test_and_set_explicit( &lock, memory_order_acquire ) )
      ;
    if ( cnt < 100 ) {
      cout << cnt++ << ": output from thread " << tid << endl;
      atomic_flag_clear_explicit( &lock, memory_order_release );      
    }
    else {
      atomic_flag_clear_explicit( &lock, memory_order_release );
      break;
    }
  }

  return NULL;
}

int main( int argc, char* argv[]  ) {
  if ( argc != 2 ) {
    cerr << "usage: tas nThreads" << endl;
    return -1;
  }
  int nThreads = atoi( argv[1] );
  pthread_t tid[nThreads];
  int logical_tid[nThreads];

  for ( int i = 0; i < nThreads; i++ )
    logical_tid[i] = i;

  for ( int i = 0; i < nThreads; i++ )
    pthread_create( &tid[i], NULL, inc, (void *)&logical_tid[i] );
  
  for ( int i = 0; i < nThreads; i++ )
    pthread_join( tid[i], NULL );
}
