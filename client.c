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
	char *uid[MAXUID];
	int parErr;
	bool U, G, N, H, S; //klient zada o 
}TParams;

enum numEcode{
	EARGS=0, //chybne argumenty
	ECONCATENATE,
};

const char *errcodes[]={
	"chybne argumenty", //EARGS
	"nebyl zadan ani login, ani uid", //ECONCATENATE
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
							param.uid[j]=argv[optind];
							
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

char *concatenate(TParams result)
{
	char *tmp;
	int i;
	char countOfLogin[2]; // pocet zadanych loginu
	char countOfUid[2]; //pocet zadanych uid

	sprintf(countOfLogin, "%d", result.l);
	sprintf(countOfUid, "%d", result.u);


	if(result.u != 0)
	{
		if(result.u==1)
		{
			tmp=result.uid[0];
			tmp=strcat(tmp, ":");
		}

		else if(result.u > 1)
		{
			tmp=strcat(tmp, result.uid[0]);
			for(i=1; i<result.u; i++)
			{
				tmp=strcat(tmp, ":");
				tmp=strcat(tmp, result.uid[i]);
			}
		}

		else{
			return NULL;
		}
	}

	else if(result.u==0 && result.l!=0)
	{	
		if(result.l==1)
		{
			tmp=result.login[0];
		}

		else if(result.l > 1)
		{
			tmp=result.login[0];
			for(i=1; i<result.l; i++)
			{
				tmp=strcat(tmp, ":");
				tmp=strcat(tmp, result.login[i]);
			}
		}
		
	}

	else{
		return NULL;
	}

	if(result.U)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "1");
	}

	else{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "0");
	}

	if(result.G)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "1");
	}

	else{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "0");
	}

	if(result.N)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "1");
	}

	else{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "0");
	}

	if(result.H)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "1");
	}

	else{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "0");
	}

	if(result.S)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "1");
	}

	else{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, "0");
	}


	if(result.l!=0)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, countOfLogin);		
	}

	if(result.u!=0)
	{
		tmp=strcat(tmp, ":");
		tmp=strcat(tmp, countOfUid);			
	}

    return tmp;
}

int main (int argc, char *argv[] )
{
	TParams result;
  	result=getParams(argc, argv);

  	int s, n;
  	struct sockaddr_in sin; struct hostent *hptr;
  	char *msg;

  	msg=concatenate(result);
  	printf("msg je %s\n", msg);	
  	if(result.parErr!=1)
  	{
  		if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
  		{ 
    		perror("error on socket");  /* socket error */
    		return -1;
  		}

  		sin.sin_family = PF_INET;              /*set protocol family to Internet */
  		sin.sin_port = htons(result.port);  /* set port no. */
  		
  		if ( (hptr =  gethostbyname(result.servername) ) == NULL)
  		{
   	 		fprintf(stderr, "gethostname error: %s", result.servername);
   	 		return -1;
  	 	}
  		
  		memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
  		if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
  		{
  	  		perror("error on connect"); return -1;   /* connect error */
 	 	}

  		if ( write(s, msg, strlen(msg) +1) < 0 ) 
  		{ 
 	   		perror("error on write");    return -1; /*  write error */
 	 	}
  	
  		if ( ( n = read(s, msg, sizeof(msg) ) ) <0) 
  		{ 
  	  		perror("error on read"); return -1; /*  read error */
  		}

 	 	printf ("received %d bytes: %s\n", n, msg);  /* print message to screen */
  		/* close connection, clean up socket */
  		if (close(s) < 0) 
  		{ 
  	  		perror("error on close");   /* close error */
  	  		return -1;
		}
	} 

	else{
		return printError(EARGS);
	}

 	 return 0;
 }

