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

typedef struct player {
    char ip_addr[20];
    int port;
    char name[20];
    struct player *next;
    struct player *prev;
}player;

void printPlayerList(player *list) {
    player *currentNode = list;
    while (1) {
        if(currentNode->next == NULL) break;
        currentNode = currentNode->next;
        // printf("LINKLIST: IP: %s:%d. Name: %s\n",currentNode->ip_addr, currentNode->port , currentNode->name);
    }
    // free(currentNode);

}

player* addNewPlayer(player *list, char *ip_addr, int port, char *name) {
    player *currentNode = (player *)malloc(sizeof(player));
    currentNode = list;
    while (1) {
        if(currentNode->next == NULL) break;
        currentNode = currentNode->next;
    }

    if ( ip_addr != NULL && name != NULL) {
        player *newPl;
        newPl = (player *)malloc(sizeof(player));
        strcpy(newPl->ip_addr, ip_addr);
        newPl->port = port;
        strcpy(newPl->name, name);
        currentNode->next = newPl;
        newPl->prev=currentNode;
        // printf("Addmode");
        // printPlayerList(newPl);
    }

      return list;

}

void setPlayerName(player *list, char *ip_addr, int port, char *name) {
    player *currentNode = list;
    while (1) {
        if(currentNode->next == NULL) break;
        currentNode = currentNode->next;
        if (strcmp(ip_addr,currentNode->ip_addr) == 0 && port == currentNode->port) {
            strcpy(currentNode->name,name);
        }
    }

}

int countPlayer(player *list) {
    player *currentNode = list;
    int i = 0;
    while (1) {
        if(currentNode->next == NULL) break;
        i++;
        currentNode = currentNode->next;
    }
    return i;
}

char* playerInfo(player* list, int index) {
    player *currentNode = list;
    int i = 0;
    for(i=0; i<index; i++) {
        currentNode = currentNode->next;
        // printf("Index=: %d--LIST: Client %d : %s:%d. Name: %s\n",index, i, currentNode->ip_addr, currentNode->port, currentNode->name);
    }
    char* info;
    info = ( char *)malloc(100);
    char tmp[100];
    bzero(info, sizeof(info));
    // printf("LIST: Client %d : %s:%d. Name: %s\n", idx, plList[idx].ip_addr, plList[idx].port, plList[idx].name);
    strcat(info,currentNode->ip_addr);
    strcat(info, ":");
    sprintf(tmp, "%d", currentNode->port);
    strcat(info,tmp);
    strcat(info, ":");
    strcat(info, currentNode->name);
    info[strlen(info)] = '\0';
    bzero(tmp, sizeof(tmp));
    // printf("Info: %s\n", info);
    return info;
}

int main()
{   
    printf("Waiting client!\n");
    //Playerlist
    // player plList[10];
    // int playerCount = 0;
    player *list = (player *)malloc(sizeof(player));

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
                            // strcpy(plList[playerCount].ip_addr, inet_ntoa(clientAddress.sin_addr));
                            // plList[playerCount].port = ntohs(clientAddress.sin_port);
                            // printf("CLient %d : %s:%d\n", playerCount, plList[playerCount].ip_addr, plList[playerCount].port);
                            // playerCount++;
                            // char message[100];
                            // int nrecv = recv(i,message,100,0);
                            list = addNewPlayer(list,inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), "\0");

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
                                        if(strcmp(message, "/disconnect") == 0) {
                                            printPlayerList(list);
                                            printf("[-]Disconnect from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                                            bzero(message, sizeof(message));
                                            break;
                                        } 
                                        else 
                                        if (strcmp(message, "/list") == 0) {
                                            printPlayerList(list);
                                            int idx = 0;
                                            int n = countPlayer(list);
                                            printf("N = %d\n",n);
                                            // char sendBuf[1000];
                                            // bzero(sendBuf, sizeof(sendBuf));
                                            for (idx = 1; idx <= n; idx++) {
                                                send(i,playerInfo(list, idx), strlen(playerInfo(list, idx)),0);
                                                printf("Sent Buffer: %s, lenght = %ld\n", playerInfo(list, idx), strlen(playerInfo(list, idx)));
                                            }
                                            char tmp[100];
                                            strcpy(tmp, "/endlist");
                                            tmp[strlen(tmp)] = '\0';
                                            send(i, tmp, sizeof(tmp), 0);
                                            bzero(tmp, sizeof(tmp));
                                        }
                                        else if (strcmp(message, "/setname") == 0) {
                                            recv(i, message, 1024, 0);
                                            printf("IP: %s:%d. Name: %s\n",inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port) , message);
                                            setPlayerName(list, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), message);
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
