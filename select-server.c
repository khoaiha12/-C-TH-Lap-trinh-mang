//Các file header
//For socket()
#include <sys/types.h>
#include <sys/socket.h>
//For inet_addr()
#include <netinet/in.h>
#include <arpa/inet.h>
// For ..
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct player {
    char ip_addr[20];
    int port;
    char name[20];
    struct player *next;
    struct player *prev;
}player;

int main()
{   
    //Playerlist
    player plList[10];
    int playerCount = 0;


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
            perror("BIND");
            exit(0);
        }
        //Gio ta cho server lang nghe ket noi
        check = listen(serverSocket,10);
	if (check == -1)
	{
		perror("Listen");
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
                            strcpy(plList[playerCount].ip_addr, inet_ntoa(clientAddress.sin_addr));
                            plList[playerCount].port = ntohs(clientAddress.sin_port);
                            // printf("CLient %d : %s:%d\n", playerCount, plList[playerCount].ip_addr, plList[playerCount].port);
                            playerCount++;

                          FD_SET(newCon,&masterfds);
                          if (newCon > max_fd ) max_fd = newCon;
                        }
                        else
                        {
                            char message[100];
                            printf("Receive data in socket %d\n",i);
                            int nrecv = recv(i,message,100,0);
                            // printf("%s",message);
                            if (nrecv == -1)
                            {
                                printf("In socket %d\n",i);
                                perror("RECEIVE");
                                close_fd = 1;
                            }
                            else
                            if (nrecv != 0)
                                    {
                                        message[nrecv] = 0;
                                        // printf("Client say : %s\n",message);
                                        // int nsent = send(i,message,strlen(message),0);
                                        // if (nsent == -1)
                                        // {
                                        //     printf("In socket %d\n",i);
                                        //     perror("SEND");
                                        //     close_fd = 1;
                                        // }
                                        if(strcmp(message, ":disconnect") == 0) {
                                            printf("[-]Disconnect from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                                            bzero(message, sizeof(message));
                                            break;
                                        } 
                                        else 
                                        if (strcmp(message, ":list") == 0) {
                                            int idx;
                                            char sendBuf[1000];
                                            bzero(sendBuf, sizeof(sendBuf));
                                            char tmp[100];
                                            for (idx=0; idx<playerCount; idx++) {
                                                printf("LIST: Client %d : %s:%d. Name: %s\n", idx, plList[idx].ip_addr, plList[idx].port, plList[idx].name);
                                                strcat(sendBuf,plList[idx].ip_addr);
                                                strcat(sendBuf, ":");
                                                sprintf(tmp, "%d", plList[idx].port);
                                                strcat(sendBuf,tmp);
                                                strcat(sendBuf, "\t");
                                                strcat(sendBuf, plList[idx].name);
                                                send(i,sendBuf,strlen(sendBuf),0);
                                                // printf("SendBuff: %s\n",sendBuf); }
                                                bzero(tmp, sizeof(tmp));
                                                bzero(sendBuf, sizeof(sendBuf));
                                            }
                                            strcpy(tmp, "endlist");
                                            send(i, tmp, sizeof(tmp), 0);
                                            bzero(tmp, sizeof(tmp));
                                        }
                                        else if (strcmp(message, ":setname") == 0) {
                                            recv(i, message, 1024, 0);
                                            printf("IP: %s:%d. Name: %s\n",inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port) , message);
                                            int i;
                                            for(i = 0; i<playerCount; i++) {
                                                if(strcmp(inet_ntoa(clientAddress.sin_addr),plList[i].ip_addr) == 0 && ntohs(clientAddress.sin_port) == plList[i].port) {
                                                    strcpy(plList[i].name, message);
                                                }
                                            }
                                            bzero(message, sizeof(message));

                                        }
                                        else {
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
