#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void* context = 0;
char const* connection = "tcp://*:5555";

void* ThreadStarter (void* data) {
  void *responder;

  if (2 < argc) {
    conc = atoi(argv[2]);
  }

  if (3 < argc) {
    connection = argv[3];
  }

  // Socket to talk to clients
  responder = zmq_socket (context, ZMQ_REP);
  zmq_bind (responder, connection);
  
  while (1) {
    zmq_msg_t request;
    zmq_msg_t reply;

    // Wait for next request from client
    zmq_msg_init (&request);
    zmq_recv (responder, &request, 0);
    // printf ("Received Hello\n");
    zmq_msg_close (&request);
    
    // do some work
    
    // Send reply back to client
    zmq_msg_init_size (&reply, 5);
    memcpy (zmq_msg_data (&reply), "World", 5);
    zmq_send (responder, &reply, 0);
    zmq_msg_close (&reply);
  }
  // We never get here but if we did, this would be how we end
  zmq_close (responder);
}

int main (int argc, char* argv[])
{
  if (1 < argc) {
    connection = argv[1];
  }

  context = zmq_init (16);



  zmq_term (context);
  return 0;
}
