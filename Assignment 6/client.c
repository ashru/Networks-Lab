#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dirent.h>

#define BUFFER_SIZE 1024


char** files = NULL;
char** allfiles = NULL;
int files_count = 0;
int allfiles_count = 0;

// code for client for FIS
int sfd,wsfd;
char buffer[1024];
socklen_t addr_size;



char send_buffer[1024];  
int sockfd;
char rec_buffer[1024];
struct sockaddr_in server_addr;


int connect_to_fis() {
    sockfd = 0;

    memset(rec_buffer, '0' ,sizeof(rec_buffer));
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))< 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12005);
    server_addr.sin_addr.s_addr = inet_addr("10.0.0.1");

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
}




int filesocket;
void send_fname(char* fname)
{
    char buffer[BUFFER_SIZE];
    strcpy(buffer,fname);
    send(filesocket,buffer,strlen(buffer),0);
}


void request_file(char* ip, char* file, char* saveas)
{
    socklen_t addr_size;
    struct sockaddr_in serverAddr;

    filesocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12002);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));  
    addr_size = sizeof(serverAddr);
    printf("%d\n",addr_size );
    if(connect(filesocket, (struct sockaddr *) &serverAddr, addr_size)<0){
        perror("Connect: ");
        printf("%s\n",ip );
        exit(1);
    }
    
    send_fname(file);
    FILE *f = fopen(saveas,"w");
    char buffer[BUFFER_SIZE];
    int t;
    int i;
    while((t=recv(filesocket, buffer, BUFFER_SIZE, MSG_WAITALL))>40) {
        for(i=0;i<t;i++)
            putc(buffer[i],f);
    }
    fclose(f);
    close(filesocket);
    printf("File saved.\n");
}




int sock, n;
unsigned int length;
struct sockaddr_in server, from;
struct hostent *hp;
char fis_ip[16];



void connectfis() {
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    hp = gethostbyname("localhost");
    if (hp==0) {
        perror("Unknown host");
        exit(1);
    }

    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    server.sin_addr.s_addr = inet_addr("10.0.0.1");
    server.sin_port = htons(12000);
    length = sizeof(struct sockaddr_in);
    
}






int getDetails (char *file) {
    // file = "REQ"+file;
    char * new_file = NULL;
    new_file = (char *)malloc(sizeof(char)*(strlen(file)));
    new_file[0] = '\0';
    //strcat(new_file,"REQ");
    strcpy(new_file,file);

    connectfis();
    strcpy(buffer,new_file);
    printf("\n%s",buffer);
    n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *)&server,length);
    if (n < 0) {
        perror("Sendto");
        exit(1);
    }

    printf("%s\n",buffer);
    n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&from, &length);
    if (n < 0) {
        perror("recvfrom");
        exit(1);
    }

    printf("\nmadarchod%s\n",buffer);
    
    write(1,"Got an ack: ",12);
    write(1,buffer,n);
    close(sock);
    
    if (buffer[0]=='-') 
        return -1;
    
    return 0;
}


int main(int argc, char *argv[])
{
    /*if (argc == 2){
        strcpy(fis_ip,argv[1]); 
    }
    else{
        printf("Enter the IP of FIS Server : ");
        scanf ("%[^\n]%*c", fis_ip);
    }
    */
    //list_dir();
    
    //init();
    // printf("here\n");
    
    
    int input, i;
    // string fname, save_as;
    
    
    int x;
    while(1) {
        printf("What would you like to do?\n");
        printf("1. Download a file.\n");
        printf("2. Shut down.\n");
        scanf("%d",&input);
        printf("%d\n",input);
        char c;

       
        if(input==1){        
            while((c = getchar()) != '\n' && c != EOF);
            printf("File name: \n");
            fflush(stdin);
            char fname[1024];
            fname[0]='\0';
    char save_as[1024];
    save_as[0]='\0';
            fgets(fname, BUFFER_SIZE , stdin);
            fflush(stdin);
            
            //while((c = getchar()) != '\n' && c != EOF);
            fname[strlen(fname)-1] = '\0';
            printf("File requested : %s \n",fname);   
            

            if (getDetails(fname) ==-1) {
                printf("File not found.\n");
                break;
            }
            
            fflush(stdin);
            
            
            printf("\nSave as: \n");
            fflush(stdin);
            
            
            
            fgets(save_as, sizeof(save_as) , stdin);
            fflush(stdin);

            
            //while((c = getchar()) != '\n' && c != EOF);
            fflush(stdin);
        save_as[strlen(save_as)-1] = '\0';
            
            printf("File to be saved as : %s \n",save_as);   
            while((c = getchar()) != '\n' && c != EOF);
            
           // scanf (" %[^\n]%*c\n", save_as);
            
            char* ip = NULL;
            ip = (char*)malloc(n*sizeof(char));
            for(i=0;i<n;i++){
                ip[i] = buffer[i];
            }
            ip[n]='\0';
            // string ip(buffer,n);
            printf("Banchod saala");
            request_file(ip,fname,save_as);
            
            printf("Madarchod saala");
            
        }
        else
            break;
    }
    
    return 0;
    
}
