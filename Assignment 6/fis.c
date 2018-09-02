#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>




#define BUFFER_SIZE 1024

typedef struct _filetablenode{
	char * filename;
	char * ip;
		struct _filetablenode * next;
}filetablenode;

filetablenode* head = NULL;


void populate_filetable(){
	FILE *f;
    char c;
    f=fopen("fis.txt","rt");
    int n;
    fscanf(f,"%d",&n);
    int i;
    for(i=0;i<n;++i)
    {
    	char filename[BUFFER_SIZE],ip[16];
    	fscanf(f,"%s",filename);
    	fscanf(f,"%s",ip);
    	if (head == NULL){
			head = (filetablenode *)malloc(sizeof(filetablenode));
			head->filename = (char*)malloc(sizeof(char)*(strlen(filename)+1));
			head->ip = (char*)malloc(sizeof(char)*(strlen(ip)+1));
			head->next = NULL;
			strcpy(head->filename,filename);
			strcpy(head->ip,ip);
		}	
		else
		{
		filetablenode * temp = NULL;
		temp = (filetablenode *)malloc(sizeof(filetablenode));
		temp->filename = (char*)malloc(sizeof(char)*(strlen(filename)+1));
		temp->ip = (char*)malloc(sizeof(char)*(strlen(ip)+1));
		strcpy(temp->filename,filename);
		strcpy(temp->ip,ip);

		temp->next = head;
		head = temp;		
		}
    }
    fclose(f);
    
}


void view_filetable(){
	if (head == NULL)
		printf("Table empty\n");
	else{
		filetablenode *temp = head;
		while(temp != NULL){
			printf("%s\t%s\n",temp->filename,temp->ip);
			temp = temp->next;
		}
	}
}
char* find_ip_from_filename(char *filename){
	char *ip = NULL;
	printf("%s hi\n",filename );
	if(head == NULL){
		return ip;
	}
	else{
		filetablenode* temp = head;
		while(temp != NULL){
			if (strcmp(filename,temp->filename)==0){
				return temp->ip;
			}
			temp=temp->next;
		}
	}
	return ip;
}


void get_ip(char* buffer, int n){
	// n is the size 
	printf("%s\n",buffer );
	char * ip = NULL;
	char * filename = NULL;
	int i;
	filename = (char *)malloc(sizeof(char)*(n-2));
	for(i=0;i<n;i++){
		filename[i] = buffer[i];
	}
	filename[n] = '\0';

	ip = find_ip_from_filename(filename);
	if (ip == NULL){
		printf("File not present\n");
		buffer[0] = '-';
		buffer[1] = '\0';
	}
	else{
		strcpy(buffer,ip);
	}
}


int main(){
	// init the file map
	populate_filetable();
	view_filetable();

	int s,l,n;
	int i;
	socklen_t src_len;
	struct sockaddr_in server;
	struct sockaddr_in src;

	// 1024 bit buffer of data
	char buffer[1024];

	s = socket(AF_INET, SOCK_DGRAM,0);
	if (s < 0){
		perror("Cannot open socket");
		exit(1);
	}

	l = sizeof(server);
	bzero(&server,l);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(12000);

	if (bind(s,(struct sockaddr *)&server, l) < 0){
		perror("Binding error");
		exit(1);
	}

	src_len = sizeof(struct sockaddr_in);

	char type[4] ; 

	while(1){
		n = recvfrom(s,buffer,1024,0,(struct sockaddr *)&src,&src_len);
        printf("Datagram's IP address is: %s\n", inet_ntoa(src.sin_addr));
        printf("Datagram's port is: %d\n", (int) ntohs(src.sin_port));
        if (n < 0) {
			perror("recvfrom error");
			exit(1);
		}
		printf("here\n");
		get_ip(buffer, n);
		
		
		n = sendto(s, buffer, strlen(buffer),0, (struct sockaddr *)&src,src_len);
		if (n < 0){
			perror("sendto error");
			exit(1);
		}
	}
}



