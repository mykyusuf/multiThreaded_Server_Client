#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define MAX 1024
#define SA struct sockaddr

void signalHandler(int sig);
int handle_first_shake(int sockfd,int num);
int handle_second_shake(int sockfd,int num);
int sockfd;

int main(int argc,char** argv)
{
    int connfd;
    struct sockaddr_in servaddr, cli;
    char *ip=argv[1];
    int port=atoi(argv[2]);
    int seqNum=atoi(argv[3]);

    signal(SIGINT,signalHandler);
    signal(SIGTSTP,signalHandler);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        //printf("socket creation failed...\n");
        exit(0);
    }
    //printf("Socket successfully created..\n");
    
    bzero(&servaddr, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);
    
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        //printf("connection with the server failed...\n");
        exit(0);
    }
    else{
        //printf("connected to the server..\n");
        int rt=handle_first_shake(sockfd,seqNum);
        if(rt==seqNum+1){
            handle_second_shake(sockfd,rt+1);
        }
        else{
            printf("ERROR");
        }
        
    }
    
    close(sockfd);
    
}

int handle_first_shake(int sockfd,int num)
{
    char buff[MAX];
    char client_message[1024];
    int read_size;
    
    bzero(buff, sizeof(buff));
    sprintf(buff, "HELLO %d", num);
    
    if(send(sockfd , buff , strlen(buff) , 0) < 0)
    {
        //perror("Send failed");
        exit(0);
    }
    
    if( (read_size = recv(sockfd , client_message , 1024 , 0)) > 0 )
    {
        client_message[read_size]='\0';
        printf("%s\n",client_message);
        
    }
    if(read_size == 0)
    {
        //perror("Client disconnected");
        fflush(stdout);
        exit(0);
    }
    else if(read_size == -1)
    {
        //perror("recv failed");
        exit(0);
    }
    
    return atoi(&client_message[6]);
}

int handle_second_shake(int sockfd,int num)
{
    char buff[MAX];
    char client_message[1024];
    int read_size;
    
    bzero(buff, sizeof(buff));
    sprintf(buff, "HELLO %d", num);
    
    if(send(sockfd , buff , strlen(buff) , 0) < 0)
    {
        //perror("Send failed");
        exit(0);
    }
    
    return num;
}
void signalHandler(int sig){
    
    close(sockfd);
    
    printf("Exit by Signal!\n");
    exit(0);
    
}
