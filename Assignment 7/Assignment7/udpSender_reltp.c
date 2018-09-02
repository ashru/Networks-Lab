#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>     
#include <string.h>    
#include <unistd.h>     
#include <errno.h>      
#include <signal.h>     
#define DATALIMIT       4096   
#define TIMEOUT_SECS    3      
#define DATAMSG         1       
#include <sys/stat.h>
struct stat st;
FILE *f;

    int sock,noTearDownACK,base,seqNumber,dataLength,numOfSegments;                               
    struct sockaddr_in recievingServer;     
    struct sockaddr_in fromAddr;            
    unsigned short recievingServerPort;    
    unsigned int fromSize;                 
    struct sigaction myAction;              
    char *servIP;                          
    int respStringLen;                     
        struct packet ackPacket;

    int chunkSize;                          
    int windowSize;                        
int tries=0;   
char *dataBuffer;

void myAlarm(int ignored)
{
    return;
}           

struct packetheader {
    int seq_no;
    int ack_no;
    int control;
    int term;
    
};

struct packet {
    struct packetheader header;
    char data[512];
    int length;
};

void ack_rec()
{
        if(ackPacket.header.term != 1 && ackPacket.header.control == 1){
            printf("Recieved %d th Acknowledgement!!!!\n", ackPacket.header.ack_no);
            if(ackPacket.header.ack_no > base){
                base = ackPacket.header.ack_no;
            }
        } else {
            printf("End acknowledgement Recieved \n");
            noTearDownACK = 0;
        }
}
void init_socket()
{
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");


    memset(&recievingServer, 0, sizeof(recievingServer));    /* Zero out structure */
    recievingServer.sin_family = AF_INET;
    recievingServer.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    recievingServer.sin_port = htons(recievingServerPort);       /* Server port */

}
void signal_helper()
{
        myAction.sa_handler = myAlarm;
    if (sigemptyset(&myAction.sa_mask) < 0) 
        perror("sigemptyset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        perror("sigaction() failed for SIGALRM");
}
void frame_and_send()
{
     struct packet dataPacket;
        dataPacket.header.control = 0;
        dataPacket.header.term = 0;
            if(seqNumber == numOfSegments){
                dataPacket.header.seq_no = seqNumber;
                dataPacket.header.term = 1;
        printf("Sending Terminal Packet\n");
            } else {
                char seg_data[chunkSize];
                int dl=fread(seg_data,1,chunkSize,f);
                dataPacket.length=dl;
        dataPacket.header.seq_no = seqNumber;
        memset(dataPacket.data, 0, sizeof(dataPacket.data));
        strcpy(dataPacket.data, seg_data);
                printf("Sending Packet: %d\n", seqNumber);
            }

            if (sendto(sock, &dataPacket, sizeof(dataPacket), 0,
                 (struct sockaddr *) &recievingServer, sizeof(recievingServer)) != sizeof(dataPacket))
                perror("sendto() sent a different number of bytes than expected");
            seqNumber++;
}
void resend_packets()
{
             while(seqNumber <= numOfSegments && (seqNumber - base) <= windowSize){
                        struct packet dataPacket;
            
            if(seqNumber == numOfSegments){
                        dataPacket.header.seq_no = seqNumber; 
                        dataPacket.header.term = 1;
                        printf("Sending last Packet\n");
                    } else {
                        char seg_data[chunkSize];
                        int dl=fread(seg_data,1,chunkSize,f);
                        dataPacket.length=dl;
                        dataPacket.header.seq_no = seqNumber;
                        memset(dataPacket.data, 0, sizeof(dataPacket.data));
                        strcpy(dataPacket.data, seg_data);
                        printf("Sending Packet: %d\n", seqNumber);
                    }

                    if (sendto(sock, &dataPacket, sizeof(dataPacket), 0,
                        (struct sockaddr *) &recievingServer, sizeof(recievingServer)) != sizeof(dataPacket))
                        perror("sendto() sent a different number of bytes than expected");
                    seqNumber++;

                    }
}
int main(int argc, char *argv[])
{


    if (argc != 6)    
    {
        fprintf(stderr,"Usage: %s <Server IP> <Server Port> <Chunk Size> <Window Size> <Filename>\n", argv[0]);
        exit(1);
    }

    servIP = argv[1];                         
    recievingServerPort = atoi(argv[2]);      
    chunkSize = atoi(argv[3]);                 
    windowSize = atoi(argv[4]);                

        f=fopen(argv[5],"rb");
        fseek(f, 0, SEEK_END);
        int datasize = ftell(f);
        rewind(f);



    if(chunkSize > DATALIMIT){
        fprintf(stderr, "Error: Chunk Size is too large. Must be < 2048 bytes\n");
        exit(1);
    }
    init_socket();



    
    numOfSegments = datasize / chunkSize;
    if (datasize % chunkSize > 0){
        numOfSegments++;
    }

    base = -1;           
    seqNumber = 0;      
    dataLength = 0;     


    signal_helper();

    noTearDownACK = 1;
    while(noTearDownACK){

        while(seqNumber <= numOfSegments && (seqNumber - base) <= windowSize){
           frame_and_send();
        }


        alarm(3);        

        printf("Window full: waiting for ACKs\n");

        while ((respStringLen = recvfrom(sock, &ackPacket, sizeof(ackPacket), 0,
               (struct sockaddr *) &fromAddr, &fromSize)) < 0)
        {
            if (errno == EINTR)    
            {
                seqNumber = base + 1;

                printf(" Resending packets\n");
         

                    resend_packets();
                    alarm(3);
                
            }
            else
            {
                perror("recvfrom() failed");
            }
        }
        ack_rec();
        alarm(0);

    }


    close(sock);
    exit(0);
}


