#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXPORT 65535
#define MINPORT 1024

enum numEcode
{
	EARGS=0,

};

const char *errcodes[]={
	"Chybne argumenty",	//EARGS
};

int getParams(int argc, char **argv)
{
	int port;
	if(argc!=3)
	{
		fprintf(stderr, "%s\n", errcodes[EARGS]);
			return EARGS;
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
					fprintf(stderr, "%s\n", errcodes[EARGS]);
					return EARGS;				
				}
				
			}

			else{
				fprintf(stderr, "%s\n", errcodes[EARGS]);
				return EARGS;
			}
	}
		return EARGS;
}

int main(int argc, char **argv)
{
	int port; //cislo portu
	if((port=getParams(argc, argv))!=EARGS)
	{
		printf("cislo portu je: %d\n", port);
    	return 0;	
	}

	else{
		return 1;
	}
 	
}