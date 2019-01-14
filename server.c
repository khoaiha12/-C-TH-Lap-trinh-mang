#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "server_params.h"
#include "player.h"
#include "room.h"

char * get_params(char command[]) {
	int i = 0, j;
	while (command[i] != ' ') {
		i++;
	}
	i++;

	char * params = malloc(LENGTH_MSG);
	for (j = 0; i+j < strlen(command); j++) {
		*(params + j) = command[i+j];
	}
	*(params + j) = '\0';
	return params;
}

int main()
{   
    printf("Waiting client!\n");
    //Playerlist
    // player plList[10];
    // int playerCount = 0;
    player *list = (player *)malloc(sizeof(player));

    room roomList[10];
    setDefault(roomList, 10);

    // Tao server socket    
    int serverSocket =  socket(PF_INET,SOCK_STREAM,0);
    if (serverSocket == -1)
    {
        perror("CREATE SOCKET");
        exit(0);
    }

    int i = 1;
    int check = setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(int));
    if (check == -1)
    {
	perror("Set reuse");
    }

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5000);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

	socklen_t len = sizeof(struct sockaddr_in);
	check = bind(serverSocket,(struct sockaddr*)&serverAddress,len);
	if (check == -1)
        {
            perror("BIND ERROR!");
            exit(0);
        }

    check = listen(serverSocket,10);
	if (check == -1)
	{
		perror("Listen ERROR!");
		exit(1);
	}


    struct sockaddr_in clientAddress;
    len = sizeof(struct sockaddr_in);
    fd_set readfds;
    fd_set masterfds; // tập readfds để check các socket, 1 tập để lưu lại nhưng thay đổi của tập readfds.
    FD_ZERO(&masterfds);
    FD_ZERO(&readfds);
    FD_SET(serverSocket,&masterfds); // add serverSock vào tập masterfds.
    int max_fd = serverSocket;
    struct timeval timeout;
    timeout.tv_sec = 90;
    timeout.tv_usec = 0;
    int n_select;
       
       
       
    do
    {
        memcpy(&readfds,&masterfds,sizeof(masterfds));
        n_select = select(max_fd + 1, &readfds,NULL,NULL,NULL);
        if (n_select < 0)
        {
                perror("SELECT");
                exit(0);
        }
        else
        if (n_select == 0)
        {
            printf("Time out\n");
        }
        else
        {
              for (i = 0; i <= max_fd;i++)
                {
                    if (FD_ISSET(i,&readfds))
                    {
                    	int close_fd = 0;
                        if (i == serverSocket)
                        {                          
                            int newCon = accept(serverSocket,(struct sockaddr*)&clientAddress,&len);
                            printf("Connection accepted \n");
                            list = addNewPlayer(list, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), "\0");
                            FD_SET(newCon,&masterfds);
                            if (newCon > max_fd ) max_fd = newCon;
                            
                        }
                        else
                        {
                            char message[100];
                            printf("Receive data in socket %d\n",i);
                            int nrecv = recv(i,message,100,0);
                            // printf("%s\n",message);
                            if (nrecv == -1)
                            {
                                printf("In socket %d\n",i);
                                perror("RECEIVE");
                                close_fd = 1;
                            }
                            else
                            if (nrecv != 0)
                                    {
                                        int isCommand = 0;
                                        message[nrecv] = 0;
                                        printf("%s\n",message);
                                        if(strcmp(message, "/disconnect") == 0) {
                                            isCommand = 1;
                                            printf("[-]Disconnect from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                                            list = playerDisconnect(list, i);
                                            bzero(message, sizeof(message));
                                            break;
                                        }  
                                        if (strcmp(message, "/list") == 0) {
                                            isCommand = 1;
                                            // printf("Go tolist function\n");
                                            printPlayerList(list);
                                            int idx = 0;
                                            int n = countPlayer(list);
                                            char sendBuf[1000];
                                            bzero(sendBuf, sizeof(sendBuf));
                                            for (idx = 1; idx <= n; idx++) {
                                                bzero(sendBuf,sizeof(sendBuf));
                                                strcpy(sendBuf, playerInfo(list, idx));
                                                send(i,sendBuf, 100,0);
                                                // printf("Sent Buffer: %s, lenght = %ld\n", sendBuf, strlen(sendBuf));
                                                bzero(sendBuf,sizeof(sendBuf));
                                            }
                                            char tmp[100];
                                            strcpy(tmp, "/endlist");
                                            // tmp[strlen(tmp)] = '\0';
                                            send(i, tmp, sizeof(tmp), 0);
                                            bzero(tmp, sizeof(tmp));
                                        }
                                        if (strstr(message, "/setname")) {
                                            isCommand = 1;
                                            //recv(i, message, 1024, 0);
                                            //printf("IP: %s:%d. Name: %s\n",inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port) , message);
                                            printf("%s\n", get_params(message));
                                            setPlayerName(list, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), get_params(message), i);
                                            printPlayerList(list);
                                            bzero(message, sizeof(message));

                                        }
                                        if (strstr(message, "/enterRoom")) {
                                            printf("Function EnterRoom\n");
                                            isCommand = 1;
                                            //recv(i, message, 1024, 0);
                                            int roomNumber = atoi(get_params(message));
                                            printf("request for enter room %d\n", roomNumber);
                                            if (enterRoom(i, roomList, roomNumber) == 1) {
                                                printRoomList(roomList);
                                                printf("Success!\n");
                                            } else {
                                                printf("Can't join the room!\n");
                                            }
                                        }
                                        if (strcmp(message, "/leaveRoom") == 0) {
                                            printf("Function Leave Room!\n");
                                            isCommand = 1;
                                            if (leaveRoom(i,roomList) == 1) {
                                                printRoomList(roomList);
                                            } else {
                                                printf("Can't leave the room!\n");
                                            };
                                        }
                                        if (strcmp(message, "/play") == 0) {
                                            printf("Function PlayGame\n");
                                            recv(i, message, 1024, 0);
                                            int location = atoi(message);
                                            int playersRoom = inRoom(i, roomList);
                                            if (playersRoom != -1) {
                                                printf("Player %d of room %d mark in %d\n", i, playersRoom, location);
                                            }
                                        }
                                        if(isCommand == 0) {
                                            printf("Client:  %s\n", message);
                                            send(i,message,strlen(message),0);
                                            bzero(message, sizeof(message));
                                        }

                                    }
                                    else
                                    {
                                    	close_fd = 1;
                                    }
                        }
                        if (close_fd == 1) 
                        {
                        	FD_CLR(i,&masterfds);
                        	close(i);
                        }
                    }
                }
        }
    }while(1);
    return 0;
}
