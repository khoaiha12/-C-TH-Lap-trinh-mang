//Các file header
//For socket()
#include <sys/types.h>
#include <sys/socket.h>
//For inet_addr()
#include <netinet/in.h>
#include <arpa/inet.h>
// For ..
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_ROOM 10


#include "player.h"
#include "room.h"

int main()
{   
    printf("Waiting client!\n");
    player *list = (player *)malloc(sizeof(player));
    room roomList[10];
    // room *roomList = (room*)malloc(10*sizeof(room));
    setDefault(roomList, 10);

    // Tao server socket    
    int serverSocket =  socket(PF_INET,SOCK_STREAM,0);
    if (serverSocket == -1)
    {
        perror("CREATE SOCKET");
        exit(0);
    }
    // Set reuse option
   int i = 1;
    int check = setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(int));
    if (check == -1)
    {
	perror("Set reuse");
    }
    // Binding to a address
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5000);
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        // Neu chay trong mang thi phai dung :
        // serverAddress.sin_addr.s_addr = inet_addr(address);
	socklen_t len = sizeof(struct sockaddr_in);
	check = bind(serverSocket,(struct sockaddr*)&serverAddress,len);
	if (check == -1)
        {
            perror("BIND ERROR!");
            exit(0);
        }
        //Gio ta cho server lang nghe ket noi
        check = listen(serverSocket,10);
	if (check == -1)
	{
		perror("Listen ERROR!");
		exit(1);
	}
        // Mỗi client kết nối đến server, nếu được chấp nhận thì server sẽ tạo ra một socket để giao tiếp với client đó đến khi 1 trong 2 bên đóng.
        // Ở đây ta dùng select(), nó sẽ nhìn (watch) trên nhiều file desciptor, socket cũng là 1 loại file descriptor) để xem có sự kiện nào xảy ra
        // không?
        // Có 3 loại sự kiện trên 1 file desciptor mà ta có xem xét là : ready to read, ready to write, exception.
        // Ở đây ta chỉ quan tâm đến ready to read, sự kiện có 1 kết nối đến server coi như là ột /sự kiện ready to read trên server Socket
        // ta cần có 1 tập chứa các socket

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
                                if (strcmp(message, "/setname") == 0) {
                                    isCommand = 1;
                                    recv(i, message, 1024, 0);
                                    printf("IP: %s:%d. Name: %s\n",inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port) , message);
                                    setPlayerName(list, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), message, i);
                                    bzero(message, sizeof(message));

                                }
                                if (strcmp(message, "/enterRoom") == 0) {
                                    printf("Function EnterRoom\n");
                                    isCommand = 1;
                                    recv(i, message, 1024, 0);
                                    int roomNumber = atoi(message);
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
    }
    while(1);
    return 0;
}
