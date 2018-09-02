#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>

int main()
{
    struct sockaddr_in server,client;
    int serversock,n,fp,end;
    char filename[20],buffer[100];
    serversock=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=2000;
    server.sin_addr.s_addr=inet_addr("10.0.0.1");
    bind(serversock,(struct sockaddr *)&server,sizeof(server));
    n=sizeof(client);
    recvfrom(serversock,filename,sizeof(filename), 0,(struct sockaddr *)&client,&n);
    fp=open(filename,O_RDONLY);
    while(1)
    {
        end=read(fp,buffer,sizeof(buffer));
        printf("%s",buffer);
        // if(end==0)
        //     break;
        sendto(serversock,buffer,sizeof(buffer),0,(struct sockaddr *)&client,n);
        bzero(buffer,100);
    }
    strcpy(buffer,"end");
    sendto(serversock,buffer,sizeof(buffer),0,(struct sockaddr *)&client,n);
    return 0;
}
