#include <stdio.h>
#include <czmq.h>

#define URI "tcp://127.0.0.1:5056"

int
main(int argc, const char* argv[]) {

  int count, rc;
  char buff[256], *message;

  //Version check
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  fprintf(stderr, "Starting DDOS test.\n");
  fprintf(stderr, "ZMQ version is %d.%d.%d\n", major, minor, patch);
  fprintf(stderr, "CZMQ version is %d.%d.%d\n", CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH);

  //Setup context
  zctx_t *server_ctx = zctx_new ();
  assert(server_ctx);

  void* server =zsocket_new (server_ctx, ZMQ_ROUTER); assert(server);
  //void* server =zsocket_new (server_ctx, ZMQ_PUB); assert(server);                                                    //Swap with the above line for the PUB/SUB scenario
  rc = zsocket_bind(server, URI); assert(rc > -1);

  while(true){
  //message = zstr_recv(server);
  //printf("Got something => %s", message);
  //free(message);
    if (zctx_interrupted){
      fprintf(stderr, "Interrupted, please take a moment to observe the size of the executable in memory.\nPress enter when done...\n");
      scanf("%c", &buff);
      break;
    }
  }

  fprintf(stderr, "Shutting down...\n");
  zctx_destroy (&server_ctx);

  return 0;
}
