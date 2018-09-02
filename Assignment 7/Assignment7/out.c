#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    struct sockaddr_in server,client;
    int s,n,ret;size_t fp;
    char filename[20],downloaded[10],filedata[100],c[25];
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    s=socket(AF_INET,SOCK_DGRAM,0);
    server.sin_family=AF_INET;
    server.sin_port=2000;
    server.sin_addr.s_addr=inet_addr("10.0.0.2");
    n=sizeof(server);
    printf("Enter the name of the file: ");
    scanf("%s",filename);
    printf("\nEnter a name to save: ");
    scanf("%s",downloaded);
    printf("\nDownloading...\n");
    sendto(s,filename,sizeof(filename),0,(struct sockaddr *)&server,n);
    fp = open(downloaded, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if(fp==-1)
    {
        printf("\nError...");
        exit(0);
    }
    recvfrom(s,filedata,sizeof(filedata),0,NULL,NULL);
    printf("\nProcessing Contents...\n");
    while(1)
    {        
        if(strcmp(filedata,"end")==0){
            // printf("%s\n",filedata);
            break;
        }
        printf("%s",filedata);
        ret=write(fp,filedata,strlen(filedata));
        bzero(filedata,100);
        recvfrom(s,filedata,sizeof(filedata),0,NULL,NULL);
    }
    printf("\nFile downloaded successfully...\n");
    return 0;
}
