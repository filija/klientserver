#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <ctype.h>

#define MAXPORT 65535 //maximalni cislo portu
#define MINPORT 1024  //minimalni cislo portu

/*
	struktura parametry "legenda"
	port - cislo portu
	l - uzivatelske jmeno
	u - UID
	g - GID 
	n - cele jmeno
	h - domovsky adresar
	s - logovaci shell
*/
typedef struct tparams{
	int port, l, u, g, n, h, s; //
	char *servername;
	char *login;
}TParams;

enum numEcode{
	EARGS=0, //chybne argumenty
};

const char *errcodes[]={
	"chybne argumenty", //EARGS
};

int printError(numErr)
{
	fprintf(stderr, "%s\n", numErr);
	return -1;
}

int getParams(int argc, char **argv)
{
	TParams param={0, 0, 0, 0, 0, 0, 0, NULL, NULL};
	int c;
	int numOptarg;
	if(argc < 3)
	{
		return printError(EARGS);
	}	

	while((c=getopt(argc, argv, "p:h:l:u:GHNS"))!=-1)
	{
		switch(c)
		{
			case 'p':
				numOptarg=atoi(optarg);
				if(numOptarg >= MINPORT && numOptarg <= MAXPORT)
				{
					param.port=numOptarg;
				}
				else{
					return printError(EARGS);
				}
				break;

			case 'h':
				  param.servername=optarg;
				  break;

			case 'l':
					param.login=optarg;
					printf("login je %s\n", param.login);
					param.l++;
					break;
			break;
			case 'u':
			printf("Na radku byla vola 'u' s parametrem %s\n", optarg);
			break;
			case 'G':
			printf("Na radku byla vola 'G' s parametrem %s\n", optarg);
			break;
			case 'H':
			printf("Na radku byla vola 'H' s parametrem %s\n", optarg);
			break;
			case 'N':
			printf("Na radku byla vola 'N' s parametrem %s\n", optarg);
			break;
			case 'S':
			printf("Na radku byla vola 'S' s parametrem %s\n", optarg);
			break;
			case '?':
			printf("neznama volba");
			break;
			default:
				return 1;
		}
	}

return 0;

}

int main (int argc, char *argv[] )
{
  getParams(argc, argv);

  //int s, n;
  //struct sockaddr_in sin; struct hostent *hptr;
  //char msg[80] = "Hello World!";
  //if ( argc < 3 ) {
   // printf ("%s host port\n", argv[0] );   /* input error: need host & port */
  //  return -1;
  //}
  //if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) { /* create socket*/
   // perror("error on socket");  /* socket error */
    //return -1;
  //}
  //sin.sin_family = PF_INET;              /*set protocol family to Internet */
  //sin.sin_port = htons(atoi(argv[2]));  /* set port no. */
  //if ( (hptr =  gethostbyname(argv[1]) ) == NULL){
   // fprintf(stderr, "gethostname error: %s", argv[1]);
   // return -1;
  // }
  //memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
  //if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
  //  perror("error on connect"); return -1;   /* connect error */
 // }
  //if ( write(s, msg, strlen(msg) +1) < 0 ) {  /* send message to server */
 //   perror("error on write");    return -1; /*  write error */
 // }
  //if ( ( n = read(s, msg, sizeof(msg) ) ) <0) {  /* read message from server */
  //  perror("error on read"); return -1; /*  read error */
  //}
 // printf ("received %d bytes: %s\n", n, msg);  /* print message to screen */
  //* close connection, clean up socket */
  //if (close(s) < 0) { 
  //  perror("error on close");   /* close error */
  //  return -1;
 // }*/
 // return 0;
}
