#include <stdio.h>
#include <czmq.h>

#define URI "tcp://127.0.0.1:5056"

int
main(int argc, const char* argv[]) {

  int count, rc;
  char buff[256];

  //Version check
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  fprintf(stderr, "Starting cleint czmq ddos/leak test.\n");
  fprintf(stderr, "ZMQ version is %d.%d.%d\n", major, minor, patch);
  fprintf(stderr, "CZMQ version is %d.%d.%d\n", CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH);

  //void *ctx = zctx_new (); assert (ctx);

 count = 0;
  while(true){

    void *ctx = zctx_new (); assert (ctx);
    void *client =zsocket_new (ctx, ZMQ_DEALER); assert(client);

    sprintf(buff, "client_%d\0", count);
    //zsockopt_set_identity(client, buff);
    //zsockopt_set_subscribe (client, buff);                                                                            //Swap with the above line for the PUB/SUB scenario

    assert(zsocket_connect(client, URI) == 0);
    //zstr_send(client, "%s", buff);
    zclock_sleep (7);                                                                                                   //I have to do this or I get random lockups at mutex.hpp:90 slow it down more if you have this problem
    assert(zsocket_disconnect(client, URI) == 0);
    zsocket_destroy(ctx, client);
    zctx_destroy((zctx_t **)&ctx);

    fprintf(stderr, "Connected & dismantled %d\n", count);
    count = count + 1;
    //zclock_sleep (7);                                                                                                   //RE:lockups, there is an existing issue for this against libzmq
    if (zctx_interrupted){
     fprintf(stderr, "Interrupted, please take a moment to observe the size of the executable in memory.\nPress enter when done...\n");
     scanf("%c", &buff);
     break;
    }
  }

  //zctx_destroy((zctx_t **)&ctx);
  fprintf(stderr, "Shutting down...\n");
  return 0;
}
