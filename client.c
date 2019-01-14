#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "client_params.h"
#include "interface.h"
#include "queue.c"
#include "request.c"

void recv_msg() {
	char *receive_message = malloc(LENGTH_MSG);
	memset(receive_message, 0, strlen(receive_message)+1);
	int receive = recv(client_sock, receive_message, LENGTH_MSG, 0);
	
	if (receive > 0) {
		receive_message[receive] = '\0';
		puts(receive_message);
		enQueue(responses, receive_message);
	} else if (receive == 0) {
		// break;
	} else { 
		// -1 
	}
}

gboolean timer_exe(gpointer p)
{
    char msg[1024], *data;
	struct QNode * response = deQueue(responses);
    if (response != NULL) {
		strcpy(msg, response->key);
		if (strstr(msg, "new_client_success")) {
			data = get_data(msg);
			choose_zoom_screen(data);
		}
		if (strstr(msg, "join_room_success")) {	
			data = get_data(msg);
			wait_friend_screen(data);
		}
		if (strstr(msg, "join_room_error")) {
			puts(msg);
		}
		if (strstr(msg, "refresh_friend_room")) {
			data = get_data(msg);
			refresh_friend_room(data);
		}
		if (strstr(msg, "new_message_success")) {
			data = get_data(msg);
			append_message(data);	
		}
		if (strstr(msg, "answer_true")) {
			q_cur++;
			append_message(get_data(msg));
			new_question();	
		}
		if (strstr(msg, "answer_false")) {
			if (strstr(msg, "You") != 0)
				running = FALSE;
			new_question();
			data = get_data(msg);
			append_message(data);
		}
	}
    return TRUE;
}

int main (int argc, char *argv[])
{	
    responses = createQueue(); 
    if (!g_thread_supported ()){ g_thread_init(NULL); }
	// initialize GDK thread support
	gdk_threads_init();
	gdk_threads_enter();
	g_timeout_add(100, (GSourceFunc)timer_exe, NULL);
	gtk_init(&argc, &argv);

	gtk_init (&argc, &argv);
	
    clientSocket = socket(PF_INET,SOCK_STREAM,0);

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

    // Signal driven I/O mode and NONBlOCK mode so that recv will not block
    if (fcntl(client_sock, F_SETFL, O_NONBLOCK | O_ASYNC))
        printf("Error in setting socket to async, nonblock mode");

    signal(SIGIO, recv_msg); // assign SIGIO to the handler
    // set this process to be the process owner for SIGIO signal
    if (fcntl(client_sock, F_SETOWN, getpid()) < 0)
        printf("Error in setting own to socket");

    init_home_window();


    // test body
    // char message[100];

    // strcpy(message, "/setname");
    // send(clientSocket, message, strlen(message)+1, 0);
    // printf("Choose a name: ");
    // char name[20];
    // scanf("%s", name);
    // send(clientSocket, name, strlen(name)+1, 0);
    // printf("\n");
    // bzero(message, sizeof(message));

    // do
    // {

    // printf("Message : ");
    // scanf("%s",message);
    // int n_sent = send(clientSocket,message,strlen(message),0);
    // if (n_sent == -1) // Gui loi
    // {
    //     perror("SEND");
    //     exit(0);
    // }

    // printf("Send Buff: %s\n",message);
    //     if (strcmp(message, "/disconnect") == 0) {
    //         printf("[-]Disconnect from server!\n");
    //         bzero(message, sizeof(message));
    //         exit(1);
    //     }

    //     if (strcmp(message, "/list") == 0) {
    //         while(recv(clientSocket,message,100,0) > 0) {
    //             if (strcmp(message, "/endlist") == 0) break;
    //             printf("Player List: \t%s\n", message);
    //             bzero(message, sizeof(message));
    //         }
    //         bzero(message, sizeof(message));
    //     }
    
    // }while (1);

	gtk_main();
	return 0;		
}
