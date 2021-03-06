
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#define BUFFER_SIZE 1024



fd_set readset, tempset;
int maxfd, flags;
int srvsock, peersock, j, result, result1, sent, len;
struct timeval tv;
char buffer[BUFFER_SIZE];
struct sockaddr_in addr;
struct sockaddr_storage serverStorage;
socklen_t addr_size = sizeof(serverStorage);

int main(){
  srvsock = socket(PF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12002);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
  int iSetOption = 1;
  setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption));
  if(bind(srvsock, (struct sockaddr *) &addr, sizeof(addr)))
  {
    perror("Error: Could not bind\n");
    exit(1);
  }

  if(listen(srvsock,5)==0)
      printf("== Listening\n");
    else
      printf("Error\n");
  FD_ZERO(&readset);
  FD_SET(srvsock, &readset);
  maxfd = srvsock;

  do {
     memcpy(&tempset, &readset, sizeof(tempset));
     tv.tv_sec = 1000;
     tv.tv_usec = 0;
     result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

     if (result == 0) {
        printf("select() timed out!\n");
     }
     else if (result < 0 && errno != EINTR) {
        printf("Error in select(): %s\n", strerror(errno));
     }
     else if (result > 0) {
     
        if (FD_ISSET(srvsock, &tempset)) {
           len = sizeof(addr);
           peersock = accept(srvsock, (struct sockaddr *) &serverStorage, &addr_size);
           if (peersock < 0) {
              printf("Error in accept(): %s\n", strerror(errno));
           }
           else {
              FD_SET(peersock, &readset);
              maxfd = (maxfd < peersock)?peersock:maxfd;
           }
           FD_CLR(srvsock, &tempset);
        }

        for (j=0; j<maxfd+1; j++) {
           if (FD_ISSET(j, &tempset)) {
            result = recv(j, buffer, BUFFER_SIZE, 0);
            char fname[BUFFER_SIZE];
            strcpy(fname,buffer);

            if(result > 0) {
               buffer[result] = 0;
               printf("Opening: %s\n", buffer);
               sent = 0;

                  FILE *f=fopen(fname,"rb");
    struct stat st;
    int size;
    stat(fname, &st);
    sprintf(buffer,"%d",st.st_size);
  //  send(j,buffer,strlen(buffer),0);
    
                  while(!feof(f))
                  {
                    result1=fread (buffer,1,BUFFER_SIZE,f);
                    int sb = send(j,buffer,result1,0);
      printf("%d\n",result1 );
                  }
      fclose(f);
     printf("CLose File\n");
                  if (result1 > 0)
                     sent += result1;
                  else if (result1 < 0 && errno != EINTR)
                     break;
    printf("File sent: \n");
                 close(j);
                 FD_CLR(j, &readset);
    }
              else {
                 printf("Error in recv(): %s\n", strerror(errno));
              }
           }      // end if (FD_ISSET(j, &tempset))
           
        }      // end for (j=0;...)
     }      // end else if (result > 0)
  } while (1);
}
