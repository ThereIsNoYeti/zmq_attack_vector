#include <stdio.h>
#include <czmq.h>

#define URI "tcp://127.0.0.1:5056"
#define SMALL_MESSAGE "Tick"
#define HUGE_MESSAGE_SIZE 2147483647/2

void send_huge_message(zloop_t *loop, zmq_pollitem_t *item, void **params)
{
  //void **args = (void **)params;
  zmsg_t *mess = zmsg_new();
  zmsg_addstr(mess, (const char *)params[1]);
  assert(zmsg_send(&mess, params[0]) == 0);
}

void send_small_message(zloop_t *loop, zmq_pollitem_t *item, void *socket)
{
  zmsg_t *mess = zmsg_new();
  zmsg_addstr(mess, SMALL_MESSAGE);
  assert(zmsg_send(&mess, socket) == 0);
}

void recieve_message(zloop_t *loop, zmq_pollitem_t *poller, void *socket)
{

  //zmsg_t *mess = zmsg_recv (socket);
  zframe_t *frame = zframe_recv(socket);
  int len = zframe_size(frame);
  if (len < 10)
    {fprintf(stderr, "|");}
    //{fprintf(stderr, "Small message: %s\n", zframe_data(frame));}
  else
    {fprintf(stderr, "Large message: %d\n", len);}

  zframe_destroy(&frame);
}

int start_huge_message_client(void *args)
{
  int i;
  char *big_message;
  big_message = (char *) malloc (HUGE_MESSAGE_SIZE*sizeof(char));

  for(i = 0; i<HUGE_MESSAGE_SIZE; i++)
    big_message[i] ='a';
  big_message[HUGE_MESSAGE_SIZE] = '\0';

  //Setup a context
  zctx_t *ctx = zctx_new ();
  assert(ctx);

  void *socket =zsocket_new (ctx, ZMQ_DEALER); assert(socket);
  zsocket_set_maxmsgsize(socket, HUGE_MESSAGE_SIZE);
  assert(zsocket_connect(socket, URI) > -1);

  //Reactor setup and binding
  zloop_t *reactor = zloop_new();
  void* params[] = {socket, big_message};
  zloop_timer(reactor, 1000, 0, (void *)send_huge_message, params);

  //while(true)
    zloop_start(reactor);

  //Clean up
  fprintf(stderr, "Large client down...\n");
  zloop_destroy (&reactor);
  zctx_destroy (&ctx);
  free(big_message);
}

int start_small_message_client(void *args)
{
  //Setup a context
  zctx_t *ctx = zctx_new ();
  assert(ctx);

  void *socket =zsocket_new (ctx, ZMQ_DEALER); assert(socket);
  assert(zsocket_connect(socket, URI) > -1);

  //Reactor setup and binding
  zloop_t *reactor = zloop_new();
  assert(zloop_timer(reactor, 10, 0, (void *)send_small_message, socket) == 0);
  zloop_start (reactor);

  //Clean up
  fprintf(stderr, "Small client down...\n");
  zloop_destroy(&reactor);
  zctx_destroy(&ctx);
}

int main(int argc, const char* argv[])
{

  int rc;
  char buff[256];

  //Version check
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  fprintf(stderr, "Starting blocker test.\n");
  fprintf(stderr, "ZMQ version is %d.%d.%d\n", major, minor, patch);
  fprintf(stderr, "CZMQ version is %d.%d.%d\n", CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH);

  //Setup socket
  zctx_t *ctx = zctx_new ();
  assert(ctx);

  void* socket =zsocket_new (ctx, ZMQ_ROUTER); assert(socket);
  zsocket_set_maxmsgsize(socket, HUGE_MESSAGE_SIZE);
  assert(zsocket_bind(socket, URI) > -1);

  //Reactor setup and binding
  zloop_t *reactor = zloop_new();
  zmq_pollitem_t poll = {socket, 0, ZMQ_POLLIN, 0 };
  assert (zloop_poller (reactor, &poll,  (void *) recieve_message, socket) == 0);

  //Setup some threads to connect and send things
  assert(zthread_new((void*)start_small_message_client, NULL) == 0);
  assert(zthread_new((void*)start_huge_message_client, NULL) == 0);

  //Start the socket reactor
  printf ("Server started...\n");
  zloop_start (reactor);

  fprintf(stderr, "socket shutting down...\n");
  zloop_destroy (&reactor);
  zctx_destroy (&ctx);

  return 0;
}