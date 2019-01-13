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

int main()
{
    //Tạo TCP socket
    int clientSocket = socket(PF_INET,SOCK_STREAM,0);

    if (clientSocket == -1) // Neu tao socket bi loi
    {
        perror("CREATE_SOCKET");
        exit(0); // Thoat 
    }
    // De ket noi den mot server, truoc het can co dia chi cua no
    // De truyen dia chi ta can mot cau truc
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // Address family
    serverAddress.sin_port = htons(5000); // Port
// Trong truong hop dung chay local host 
    serverAddress.sin_addr.s_addr = INADDR_ANY;
// Neu muon dung trong mang phai thay bang  
//:serverAddress.sin_addr.s_addr = inet_addr(address);
// Trong do, address : la ip may hien tai trong mang

   // Them mot tham so ve kich thuoc dia chi
    socklen_t len = sizeof (struct sockaddr_in);

 // Gio ta ket noi den server
    int check = connect(clientSocket,(struct sockaddr*)&serverAddress,len);
    if (check == -1) // Ket noi bi loi
    {
        perror("CONNECT");
        exit(0);
    }    
    // Gio ta se giao tiep voi server qua clientSocket
    // Gui mot message den servser
    char message[100];
    // strcpy(message,"Hi server");

    strcpy(message, "/setname");
    send(clientSocket, message, strlen(message)+1, 0);
    printf("Choose a name: ");
    char name[20];
    scanf("%s", name);
    send(clientSocket, name, strlen(name)+1, 0);
    printf("\n");
    bzero(message, sizeof(message));

    do
    {

    printf("Message : ");
    // gets(message);
    scanf("%s",message);
    int n_sent = send(clientSocket,message,strlen(message),0);
    if (n_sent == -1) // Gui loi
    {
        perror("SEND");
        exit(0);
    }

    printf("Send Buff: %s\n",message);
        if (strcmp(message, "/disconnect") == 0) {
            printf("[-]Disconnect from server!\n");
            bzero(message, sizeof(message));
            exit(1);
        }

        if (strcmp(message, "/list") == 0) {
            while(recv(clientSocket,message,100,0) > 0) {
                if (strcmp(message, "/endlist") == 0) break;
                printf("Player List: \t%s\n", message);
                bzero(message, sizeof(message));
            }
            bzero(message, sizeof(message));
        }

        if (strcmp(message, "/enterRoom") == 0) {
            bzero(message, sizeof(message));
            int roomNumber;
            printf("RoomNumber = ");
            scanf("%d",&roomNumber);
            sprintf(message, "%d",roomNumber);
            printf("\nSend ms :%s\n",message);
            send(clientSocket,message,strlen(message),0);
            bzero(message, sizeof(message));
        }
        if (strcmp(message, "/play") == 0) {
            bzero(message, sizeof(message));
            int location;
            printf("location = ");
            scanf("%d",&location);
            sprintf(message, "%d",location);
            printf("\nSend ms :%s\n",message);
            send(clientSocket,message,strlen(message),0);
            bzero(message, sizeof(message));
        }
    }
    while (1);
    return 0;
}
