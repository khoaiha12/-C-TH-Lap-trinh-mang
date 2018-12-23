#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "interface.h"
#include "client_params.h"

int main (int argc, char *argv[])
{	
	gtk_init (&argc, &argv);
	init_home_window();

    int clientSocket = socket(PF_INET,SOCK_STREAM,0);

    if (clientSocket == -1)
    {
        perror("CREATE_SOCKET");
        exit(0); // Thoat 
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // Address family
    serverAddress.sin_port = htons(5000); // Port
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof (struct sockaddr_in);

    int check = connect(clientSocket,(struct sockaddr*)&serverAddress,len);
    if (check == -1)
    {
        perror("CONNECT");
        exit(0);
    }    

    char message[100];

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
    
    }while (1);

	gtk_main();
	return 0;		
}
