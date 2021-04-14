#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define MAX 1024

void handle_first_shake(void *);
void handle_second_shake(void *,int rt);
int socket_desc , client_sock[100];

int main(int argc , char *argv[])
{
    int c;
    struct sockaddr_in server , client;
    int port=atoi(argv[1]);
    int max_sd,sd,activity,value,new_socket;
    fd_set readfds;
    char buffer[1025];
    
    
    for (int i=0; i<100; i++) {
        client_sock[i]=0;
    }
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        exit(1);
    }

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        exit(1);
    }
    listen(socket_desc , 3);
    
    c = sizeof(struct sockaddr_in);
    
    int i=0;
    
    while (1) {
        
        FD_ZERO(&readfds);
        
        FD_SET(socket_desc,&readfds);
        max_sd=socket_desc;
        
        for ( i = 0 ; i < 100 ; i++)
        {
            sd = client_sock[i];
            if(sd > 0)
                FD_SET( sd , &readfds);
                 
            if(sd > max_sd)
                max_sd = sd;
        }
        
        activity = select(max_sd+1,&readfds,NULL,NULL,NULL);
        if ((activity < 0) && (errno!|EINTR))
        {
            printf("select error");
        }
                    
        if(FD_ISSET(socket_desc,&readfds)){
            
            if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c ) ) <0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            for (i = 0; i < 100; i++)
            {
                if( client_sock[i] == 0 )
                {
                    client_sock[i] = new_socket;
                    break;
                }
            }
            
        }
        for (i = 0; i < 100; i++)
        {
            sd = client_sock[i];
            if (FD_ISSET( sd , &readfds))
            {
                
                handle_first_shake(&sd);
                getpeername(sd,(struct sockaddr*)&client ,(socklen_t*)&c);
                close(sd);
                client_sock[i] = 0;
                
            }
        }
    }
    
        
    close(socket_desc);
    
    return 0;
}

void handle_first_shake(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[1024];
    char buff[MAX];
    int tmp=0;

    
    if( (read_size = recv(sock , client_message , 1024 , 0)) > 0 )
    {
        client_message[read_size]='\0';
        printf("%s\n",client_message);
        tmp=atoi(&client_message[6]);
        tmp++;
                    
    }
    if(read_size == 0)
    {
        perror("Client disconnected");

    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    
    bzero(buff, sizeof(buff));
    sprintf(buff, "HELLO %d", tmp);
    
    if(send(sock , buff , strlen(buff) , 0) < 0)
    {
        perror("Send failed");
    }

    handle_second_shake(socket_desc,tmp);

}
void handle_second_shake(void *socket_desc,int rt)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[1024];
    char buff[MAX];
    int tmp=0;

    if( (read_size = recv(sock , client_message , 1024 , 0)) > 0 )
    {

        client_message[read_size]='\0';
        tmp=atoi(&client_message[6]);
        if (tmp==rt+1) {
            printf("%s\n",client_message);
            memset(client_message, 0, 1024);
        }
        else{
            perror("ERROR");
        }

    }
    if(read_size == 0)
    {
        perror("Client disconnected");
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

}
