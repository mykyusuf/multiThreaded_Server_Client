#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threads
#include <sys/time.h>

#define MAX 1024

void *handle_first_shake(void *);
void handle_second_shake(void *,int rt);
int socket_desc , client_sock[100];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
    int sock;
    struct sockaddr address;
    int addr_len;
} connection_t;

int main(int argc , char *argv[])
{
    int c;
    struct sockaddr_in server , client;
    int port=atoi(argv[1]);
    int threadSize=100;
    
    pthread_mutex_init(&lock,NULL);
    pthread_t thread_id[threadSize];
    
    
    for (int i=0; i<100; i++) {
        client_sock[i]=0;
    }
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        //printf("Could not create socket");
        exit(1);
    }
    //printf("Socket created");

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        //error("setsockopt(SO_REUSEADDR) failed");
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //perror("bind failed. Error");
        exit(1);
    }
    //printf("bind done");
    
    listen(socket_desc , 3);
    
    //printf("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    int i=0;
    
    while (1) {
        if( (client_sock[i] = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c ) ) )
        {
            //printf("Connection accepted");
            if(i>=threadSize){
                for(int j=0;j<i;j++){
                    pthread_join(thread_id[j] , NULL);
                    client_sock[j]=0;
                }
                i=0;
            }
            else{
                if( pthread_create( &thread_id[i] , NULL ,  handle_first_shake , &client_sock[i]) < 0)
                {
                    printf("ERROR");
                    return 1;
                }
                else{
                    //thread_id = realloc((pthread_t*),threadSize+1) * sizeof(pthread_t));
                    i++;

                    if(i>=threadSize-1){
                        for(int j=0;j<i;j++){
                            pthread_join(thread_id[j] , NULL);
                            client_sock[j]=0;
                        }
                        i=0;
                    }
                }
            }
        }
        
        else if (client_sock[i] < 0)
        {
            printf("ERROR");
            return 1;
        }
    }
    
        
    close(socket_desc);
    //close(client_sock);
    
    return 0;
}

void *handle_first_shake(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[1024];
    char buff[MAX];
    int tmp=0;

    pthread_mutex_lock(&lock);

    if( (read_size = recv(sock , client_message , 1024 , 0)) > 0 )
    {
        client_message[read_size]='\0';
        printf("%s\n",client_message);
        tmp=atoi(&client_message[6]);
        tmp++;

                    
    }
    if(read_size == 0)
    {
        //printf("Client disconnected");

    }
    else if(read_size == -1)
    {
        //perror("recv failed");

    }
    
    bzero(buff, sizeof(buff));
    sprintf(buff, "HELLO %d", tmp);
    
    if(send(sock , buff , strlen(buff) , 0) < 0)
    {
        //printf("Send failed");
    }

    handle_second_shake(socket_desc,tmp);
    pthread_mutex_unlock(&lock);

    close(sock);
    
    pthread_exit(NULL);
    
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
        //pthread_mutex_lock(&lock);

        client_message[read_size]='\0';
        tmp=atoi(&client_message[6]);
        if (tmp==rt+1) {
            printf("%s\n",client_message);
            memset(client_message, 0, 1024);
        }
        else{
            printf("ERROR");
        }
        //pthread_mutex_unlock(&lock);

    }
    if(read_size == 0)
    {
        //perror("Client disconnected");
    }
    else if(read_size == -1)
    {
        //perror("recv failed");
    }

}
