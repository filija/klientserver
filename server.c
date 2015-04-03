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
#include <stdbool.h>

#define MAXPORT 65535 //maximalni cislo portu
#define MINPORT 1024  //minimalni cislo portu
#define MAXLOGIN 60 //maximalni pocet loginu
#define MAXUID 60 
#define MAXMSG 200 //maximalni delka zpravy
enum numEcode
{
  EARGS=0,
  ESOCK,
  EBIND,
  CONNECTOK, //pripojeni se zdarilo
  ECONNECT,
  ELISTEN,
  EACCEPT,
  EREAD,
  EWRITE,
  EPRINT,
  ECLOSE,
};

const char *errcodes[]={
  "Chybne argumenty", //EARGS
  "Nepodarilo se vytvorit socket", //ESOCK
  "Nepodarilo se navazat spojeni", //EBIND
  "", //CONNECTOK
  "Nelze se pripojit!\n", //ECONNECT
  "Naslouchani se nezdarilo", //ELISTEN
  "Selhalo prijmani pripojeni", //EACCEPT
  "Cteni selhalo", //EREAD
  "Zapis selhal", //EWRITE
  "Pripojeni se nezdarilo uzavrit", //ECLOSE
};

typedef struct tinfo{
	char *login[MAXLOGIN];
	int uid[MAXUID];
	bool U, G, N, H, S; //klient zada o 
	unsigned int countOfId;
}TInfo;

int printError(int numErr) //tisk chyb
{
  fprintf(stderr, "%s\n", errcodes[numErr]);
  return -1;
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

TInfo unparsed(char *string)
{
	int i=0;
	TInfo info;
	while(string[i]!='\0')
	{
		if(string[i]=='!')
		{
			printf("znak za ! je %c\n", string[i+1]);
			info.countOfId=string[i+1]-'0';
			break;
		}
		i++;
	}
	
	
}

int main(int argc, char **argv)
{
  int sinlen, t, sock, port; 
  struct sockaddr_in sin;
  int j;
  struct hostent * hp;
  char msg[MAXMSG];
  TInfo info; //rozdeleny retezec do datovych typu

  if((port=getParams(argc, argv))!=EARGS)
  {
    printf("cislo portu je: %d\n", port);
    if(connectServer(port, &sock, &sin)==CONNECTOK)
    {
      printf("pripojeni se zdarilo\n");
        sinlen=sizeof(sin);

        while(1)
        { //prijmani noveho pripojeni od klienta
          if ( (t = accept(sock, (struct sockaddr *) &sin, &sinlen) ) < 0 ) 
          {
              return printError(EACCEPT);
          } 

          hp=(struct hostent *)gethostbyaddr((char *)&sin.sin_addr,4,AF_INET);
          j=(int)(hp->h_length);
          printf( "From %s (%s) :%d.\n",inet_ntoa(sin.sin_addr), hp->h_name, ntohs(sin.sin_port) );
          bzero(msg,sizeof(msg));
          if ( read(t, msg, sizeof(msg) ) <0) 
          { 
              return printError(EREAD);
          }
	
 				info=unparsed(msg);

          if ( write(t, msg, strlen(msg) ) < 0 ) 
          { 
                return printError(EWRITE);
          } 

          if(close(t)<0)
          {
            return printError(ECLOSE);
          }

        }
        return 0; 
    }

    if(close(sock)<0)
    {
      return printError(ECLOSE);
    }
    else{
      return printError(ECONNECT);
    }
       
  }

  else{
    return 1;
  }
  
}