#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

int main (int argc, char *argv[] )
{
  int s, n;
  struct sockaddr_in sin; struct hostent *hptr;
  char msg[80] = "Hello World!";
  if ( argc < 3 ) {
    printf ("%s host port\n", argv[0] );   /* input error: need host & port */
    return -1;
  }
  if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) { /* create socket*/
    perror("error on socket");  /* socket error */
    return -1;
  }
  sin.sin_family = PF_INET;              /*set protocol family to Internet */
  sin.sin_port = htons(atoi(argv[2]));  /* set port no. */
  if ( (hptr =  gethostbyname(argv[1]) ) == NULL){
    fprintf(stderr, "gethostname error: %s", argv[1]);
    return -1;
   }
  memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
  if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
    perror("error on connect"); return -1;   /* connect error */
  }
  if ( write(s, msg, strlen(msg) +1) < 0 ) {  /* send message to server */
    perror("error on write");    return -1; /*  write error */
  }
  if ( ( n = read(s, msg, sizeof(msg) ) ) <0) {  /* read message from server */
    perror("error on read"); return -1; /*  read error */
  }
  printf ("received %d bytes: %s\n", n, msg);  /* print message to screen */
  /* close connection, clean up socket */
  if (close(s) < 0) { 
    perror("error on close");   /* close error */
    return -1;
  }
  return 0;
}
