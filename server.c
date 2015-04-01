#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define MAXPORT 65535 //maximalni cislo portu
#define MINPORT 1024  //minimalni cislo portu

enum numEcode
{
	EARGS=0,
	ESOCK,
	EBIND,
	CONNECTOK, //pripojeni se zdarilo
	ECONNECT,
	ELISTEN,
	EACCEPT,
};

const char *errcodes[]={
	"Chybne argumenty",	//EARGS
	"Nepodarilo se vytvorit socket", //ESOCK
	"Nepodarilo se navazat spojeni", //EBIND
	"", //CONNECTOK
	"Nelze se pripojit!\n", //ECONNECT
	"Naslouchani se nezdarilo", //ELISTEN
	"Selhalo prijmani pripojeni", //EACCEPT
};

int printError(int numErr) //tisk chyb
{
	fprintf(stderr, "%s\n", errcodes[numErr]);
	return numErr;
}

int getParams(int argc, char **argv)
{
	int port;
	if(argc!=3)
	{
		return printError(EARGS);				
	}

	else{
			if(strcmp(argv[1], "-p")==0)
			{
				port=atoi(argv[2]);
				if(port>=MINPORT && port<=MAXPORT)
				{
					return port;	
				}

				else{
					return printError(EARGS);				
				}
				
			}

			else{
				return printError(EARGS);				
			}
	}
		return EARGS;
}

int connectServer(int port, int *sock, struct sockaddr_in *server)  //vytvoreni socketu
{
	int s;
	struct sockaddr_in sin;
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printError(ESOCK);
	}
	  
	  sin.sin_family = PF_INET;
	  sin.sin_port = htons(port);
	  sin.sin_addr.s_addr = INADDR_ANY;

	  if(bind(s, (struct sockaddr *)&sin, sizeof(sin)) <0)
	  {
	  	return printError(EBIND);
	  }

	  if(listen(s, 5))
	  {
	  	return printError(ELISTEN);
	  }
	  
	  *sock=s;
	  *server=sin;
	  return CONNECTOK;
}

int main(int argc, char **argv)
{
	int sinlen, t, sock, port; 
	struct sockaddr_in sin;

	if((port=getParams(argc, argv))!=EARGS)
	{
		printf("cislo portu je: %d\n", port);
		if(connectServer(port, &sock, &sin)==CONNECTOK)
		{
			printf("pripojeni se zdarilo\n");
    		sinlen=sizeof(sin);

    		while(1)
    		{	//prijmani noveho pripojeni od klienta
    			if ( (t = accept(sock, (struct sockaddr *) &sin, &sinlen) ) < 0 ) 
    			{
     				 	return printError(EACCEPT);
    			}	

    		}
    		return 0;	
		}

		else{
			return printError(ECONNECT);
		}
		
    	
	}

	else{
		return 1;
	}
 	
}