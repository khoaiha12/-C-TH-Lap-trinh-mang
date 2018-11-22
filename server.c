#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 4444

typedef struct player {
    char ip_addr[20];
    int port;
    char name[20];
}player;

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process  terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}


int main() {
    int sockfd, ret;
    struct sockaddr_in serverAddr;
    player plList[10];
    int playerCount = 0;

    int newSocket;
    struct sockaddr_in newAddr;
    socklen_t addr_size;

    char buffer[1024];
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
        printf("[-]Error in connection.\n");
        exit(1);
    }

    printf("[+]Server Socket is created!\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //server address

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (ret<0) {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", 4444);

    if (listen(sockfd, 10) == 0) {
        printf("Listening....\n");
    } else {
        printf("[-]Error in binding.\n");
    }

    /* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

    while(1) {
        newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
        if (newSocket < 0) {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
        // plList[playerCount].ip_addr = newAddr.sin_addr;
        strcpy(plList[playerCount].ip_addr, inet_ntoa(newAddr.sin_addr));
        plList[playerCount].port = ntohs(newAddr.sin_port);
        // printf("CLient %d : %s:%d\n", playerCount, plList[playerCount].ip_addr, plList[playerCount].port);
        playerCount++;
        
        if ((childpid = fork()) == 0) {
            close(sockfd);
        int n;
            while(recv(newSocket, buffer, 1024, 0) > 0) {
                // buffer[strlen(buffer)-1] = '\0';
                if(strcmp(buffer, ":disconnect") == 0) {
                    printf("[-]Disconnect from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    bzero(buffer, sizeof(buffer));
                    break;
                } 
                else 
                if (strcmp(buffer, ":list") == 0) {
                    int i;
                    char sendBuf[1000];
                    char tmp[100];
                    for (i=0; i<playerCount; i++) {
                        printf("LIST: Client %d : %s:%d\n", i, plList[i].ip_addr, plList[i].port);
                        strcat(sendBuf,plList[i].ip_addr);
                        strcat(sendBuf, ":");
                        sprintf(tmp, "%d", plList[i].port);
                        strcat(sendBuf,tmp);
                        // printf("SendBuff: %s\n",sendBuf);
                        send(newSocket, sendBuf, strlen(sendBuf), 0);
                        bzero(tmp, sizeof(tmp));
                        bzero(sendBuf, sizeof(sendBuf));
                    }
                    send(newSocket, "", 0, 0);

                }
                else {
                    printf("Client:  %s\n", buffer);
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }

            }
        }
    }
    
    close(newSocket);
    
    return 0;
}