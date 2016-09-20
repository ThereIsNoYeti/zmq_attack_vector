#include <stdio.h>
#include <czmq.h>

int start_bonsai_request_channel(void *args) {
  printf ("Starting Thread...\n");
  void * ctx = zctx_new ();
  assert (ctx);

  zloop_t *reactor = zloop_new ();
//-- Comment or uncomment this line to toggle the 'valgrind --leak-check=full' errors ----------------------------------
  zloop_start (reactor);
//----------------------------------------------------------------------------------------------------------------------

   while(true){
      zclock_sleep (100);
      if (zctx_interrupted){
       fprintf(stderr, "Interrupted, shutting down...\n");
        break;
      }
    }

  zloop_destroy (&reactor);
  zctx_destroy((zctx_t **)&ctx);
  fprintf(stderr, "Interrupted, shut down.\n");

  return 0;
}

int
main(int argc, const char* argv[]) {

  int rc;

  //Version check
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  fprintf(stderr, "ZMQ version is %d.%d.%d\n", major, minor, patch);
  fprintf(stderr, "CZMQ version is %d.%d.%d\n", ZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH);

  //Setup context
  zctx_t *ctx = zctx_new ();
  assert(ctx);

  //Setup some threads
  const char* params[] = {};

  rc = zthread_new ((void*)start_bonsai_request_channel, params);
  assert(rc == 0);

  //Loop forever while the threads work
  while(true){
    zclock_sleep (100);
    if (zctx_interrupted){
     fprintf(stderr, "Interrupted, sending kill signal...\n");
      zclock_sleep (100*10);
      break;
    }
  }

  fprintf(stderr, "Shutting down...\n");
  zctx_destroy (&ctx);

  return 0;
}