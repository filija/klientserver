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
#include <stdbool.h>
#define MAXPORT 65535 //maximalni cislo portu
#define MINPORT 1024  //minimalni cislo portu
#define MAXLOGIN 60 //maximalni pocet  loginu
#define MAXUID 60
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
	int port, l, u;
	char *servername;
	char *login[MAXLOGIN];
	int uid[MAXUID];
	int parErr;
	bool U, G, N, H, S; //klient zada o 
}TParams;

enum numEcode{
	EARGS=0, //chybne argumenty
};

const char *errcodes[]={
	"chybne argumenty", //EARGS
};

int printError(numErr)
{
	fprintf(stderr, "%s\n", errcodes[numErr]);
	return -1;
}

TParams getParams(int argc, char **argv)
{
	TParams param={0, 0, 0, NULL, NULL};
	param.U, param.G, param.N, param.H, param.S=false;
	int c;
	int i=0;	//iterator
	int j=0;	//iterator
	int lengthLogin=0;
	int numOptarg;
	param.parErr=0;
	if(argc < 3)
	{
		param.parErr=1;
		return param;
	}	

	while((c=getopt(argc, argv, "p:h:l:u:UGHNS"))!=-1)
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
					param.parErr=1;
					return param;
				}
				break;

			case 'h':
				  param.servername=optarg;
				  break;

			case 'l':
					param.login[0]=optarg;
					
					optind--;
					while(optind < argc && *argv[optind] != '-')
					{
						param.login[i]=argv[optind];
						optind++;
						i++;		
						param.l++;				
					}			
					
					break;

			case 'u':
					optind--;
					 while (optind < argc && *argv[optind] != '-') 
					 {
							param.uid[j]=atoi(argv[optind]);
							
							if(param.uid[j]==0)
							{
								break;
							}
							else
							{
								j++;
								optind++;	
								param.u++;
							}
					 }
					break;

			case 'U':
						param.U=true;
						break;
			case 'G':
						param.G=true;
						break;
			case 'H':
						param.H=true;
						break;
			case 'N':
						param.N=true;
						break;
			case 'S':
						param.S=true;
						break;
			case '?':
						param.parErr=1;
						return param;
						break;
			default:
				param.parErr=1;
				return param;
		}
	}

return param;

}

int main (int argc, char *argv[] )
{
	TParams result;
  	result=getParams(argc, argv);

  	printf("cislo portu je: %d\n", result.port);
  	printf("hostname je: %s\n", result.servername);
  	printf("login je: %s\n", result.login[0]);
  	printf("pocet loginu je %d\n", result.l);
  	
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
