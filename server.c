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
#include <pwd.h>
#include <limits.h>

#define MAXPORT 65535 //maximalni cislo portu
#define MINPORT 1024  //minimalni cislo portu
#define MAXLOGIN 60 //maximalni pocet loginu
#define MAXUID 60 
#define MAXMSG 200 //maximalni delka zpravy
#define LENGTHLOGIN 10 //maximalni delka jednoho loginu
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
  EFILE,
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
  "Soubor se nepodarilo otevrit" //EFILE
};

typedef struct tinfo{
  char *login[MAXLOGIN];
  int uid[MAXUID];
  bool L, U, G, N, H, S; //klient zada o 
  unsigned int countOfId;
  char loguid;
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
  char login[LENGTHLOGIN];
  int j=0;
  int k;
  int tmp=0;

  while(string[i]!='$')
  { 
    if(string[i]==':')
    {
      login[tmp]='\0';
      info.login[j]=malloc(sizeof(char *));
      strcpy(info.login[j], login);
      j++;
      i++;
      tmp=0;

      for(k=0; k<LENGTHLOGIN; k++) //vynulovani pomocneho loginu
      {
          login[k]='\0';
      }        
    }

    else{
        login[tmp]=string[i];
        i++;
        tmp++;
    }
  }

  if(string[i+2]=='1')
  { 
    info.L=true;
    i=i+2;
  }

  if(string[i+2]=='1')
  {
    info.U=true;
    i=i+2;
  }

  if(string[i+2]=='1')
  { 
    info.G=true;
    i=i+2;
  }

  if(string[i+2]=='1')
  { 
    info.N=true;
    i=i+2;
  }

  if(string[i+2]=='1')
  { 
    info.H=true;
    i=i+2;
  }

  if(string[i+2]=='1')
  { 
    info.S=true;
    i=i+2;
  }

  if(string[i+2]=='l')
  { 
    info.loguid='l';
  }

  else{
      info.loguid='u';
  }

  info.countOfId=j;
  return info;
}

char *searchInEtc(TInfo info)
{  
    int i=0;
    int j;
    struct passwd *pwd;
    char *buffer;
    char *returner=malloc(MAXMSG*sizeof(char*));

      setpwent();

        while((pwd=getpwent())!=NULL)
        {
          if(info.loguid='l')
          {
            if(strcmp(info.login[j], pwd->pw_name)==0)
            {
                break;
            }
          }

          else if(info.loguid='u')
          {
              sprintf(buffer, "%i", pwd->pw_uid);

            if(strcmp(info.login[j], buffer)==0)
            {
                break;
            }
          }            
        }                
          if(info.L)
          { 
              strcat(returner, pwd->pw_name);
              strcat(returner, " ");

          }

          if(info.U)
          {    
              sprintf(buffer, "%i", pwd->pw_uid);
              strcat(returner, buffer);
              strcat(returner, " ");

          }

          if(info.G)
          {
            sprintf(buffer, "%i", pwd->pw_gid);
            strcat(returner, buffer);
            strcat(returner, " ");

          }

          if(info.N)
          {
              strcat(returner, pwd->pw_gecos);
              strcat(returner, " ");

          }

          if(info.H)
          {
              strcat(returner, pwd->pw_dir);
              strcat(returner, " ");

          }

          if(info.S)
          {
              strcat(returner, pwd->pw_shell);
              strcat(returner, " ");

          }

       endpwent();   
    
    return returner;  
}

int main(int argc, char **argv)
{
  int sinlen, t, sock, port; 
  struct sockaddr_in sin;
  int j;
  struct hostent * hp;
  char msg[MAXMSG];
  TInfo info; //rozdeleny retezec do datovych typu
  pid_t pid;
  char *result;
  int i;
  long p;
  if((port=getParams(argc, argv))!=EARGS)
  {
    printf("cislo portu je: %d\n", port);
    if(connectServer(port, &sock, &sin)==CONNECTOK)
    {
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
        
          if(pid=fork()>0)
          {
            if(close(t)<0)
            {
              return printError(ECLOSE);
            }
          }

          else{
            p=getpid();

          if ( read(t, msg, sizeof(msg) ) <0) 
          { 
              return printError(EREAD);
          }
            info=unparsed(msg);   
            result=searchInEtc(info); 
            printf("%s\n", result);
            strcpy(msg, result);

            for(i=0; i<info.countOfId; i++)
            {
              free(info.login[i]);
            }
              
          if ( write(t, msg, strlen(msg)+1 ) < 0 ) 
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