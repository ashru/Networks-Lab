#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>    
#include <string.h>   
#include <unistd.h>  


    int sock,base,seqNumber;                       
    struct sockaddr_in echoServAddr; 
    struct sockaddr_in echoClntAddr; 
    unsigned int cliAddrLen;        
    char echoBuffer[50000];        
    unsigned short echoServPort;    
    int recvMsgSize;                
        int chunkSize;                   

FILE *outfile;
struct packet dataPacket;
    struct packet ackPacket;
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
void init_socket()
{
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
       perror("socket() failed");
       exit(0);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));   
    echoServAddr.sin_family = AF_INET;               
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    echoServAddr.sin_port = htons(echoServPort);  
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
       { perror("bind() failed");exit(1);}
  

}
void init_packet_helper()
{
        printf("Recieved first packet\n");
            
                fwrite(dataPacket.data,1,dataPacket.length,outfile);
                base = 0;
        ackPacket.header.ack_no = base;
}
void seq_packet_helper()
{
    printf("Recieved  non initial packet #%d\n", dataPacket.header.seq_no);
                fwrite(dataPacket.data,1,dataPacket.length,outfile);
                base = dataPacket.header.seq_no;
                ackPacket.header.ack_no = base;
}
void out_of_sync_packet_helper()
{
     printf("Received Out of Sync Packet #%d\n", dataPacket.header.seq_no);
                
                  ackPacket.header.ack_no = base;
}
void ack_sender()
{
     if(base >= 0){
                printf("Acknowledgement #%d sent\n", base);
                if (sendto(sock, &ackPacket, sizeof(ackPacket), 0,
                     (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(ackPacket))
                    perror("sendto() sent a different number of bytes than expected");
            } else if (base == -1) {
                printf("Received Teardown Packet\n");
                printf("Sending last Acknowledgement!!!\n");
                if (sendto(sock, &ackPacket, sizeof(ackPacket), 0,
                     (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sizeof(ackPacket))
                {
                    perror("sendto() sent a different number of bytes than expected");
                    exit(0);

                }
                    
            }

}
int main(int argc, char *argv[])
{

    


    if (argc !=4)         
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT> <CHUNK SIZE> <OUTFILE> \n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]); 
    chunkSize = atoi(argv[2]); 
    

    outfile=fopen(argv[3],"wb");
     init_socket();
    base = -2;
    seqNumber = 0;

    while (1)
    {
        
        cliAddrLen = sizeof(echoClntAddr);

       
	
	ackPacket.header.control = 1;
	ackPacket.header.term = 0;
        

        if ((recvMsgSize = recvfrom(sock, &dataPacket, sizeof(dataPacket), 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            perror("recvfrom() failed");

            seqNumber = dataPacket.header.seq_no;

      
            if(dataPacket.header.seq_no == 0 && dataPacket.header.control == 0 && dataPacket.header.term != 1)
            {
                init_packet_helper();
            } else if (dataPacket.header.seq_no == base + 1 && dataPacket.header.term != 1) 
            {
                seq_packet_helper();
            } else if (dataPacket.header.control == 0 && dataPacket.header.seq_no != base + 1 && dataPacket.header.term != 1)
            {
               out_of_sync_packet_helper();
            } else if (dataPacket.header.control == 0 && dataPacket.header.term == 1) {
		          base = -1;
		          ackPacket.header.term = 1;
	    }

       
           ack_sender();
            if(dataPacket.header.control == 0 && base == -1){
                printf("\n file recieved\n \n\n");
                fclose(outfile);
                exit(1);

            }

        

    }
}

