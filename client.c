#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "client_params.h"
#include "interface.h"
#include "request.c"
#include "queue.c"

flag_turn=1;
flag_win=-1;
char * get_data(char command[]) {
	int i = 0, j;
	while (command[i] != ' ') {
		i++;
	}
	i++;

	char * data = malloc(LENGTH_MSG);
	for (j = 0; i+j < strlen(command); j++) {
		data[j] = command[i+j];
	}
	data[j] = '\0';
	return data;
}

void recv_msg() {
	char *receive_message = malloc(LENGTH_MSG);
	memset(receive_message, 0, strlen(receive_message)+1);
	int receive = recv(clientSocket, receive_message, LENGTH_MSG, 0);
	
	if (receive > 0) {
		receive_message[receive] = '\0';
		puts(receive_message);
		enQueue(responses, receive_message);
        //puts(receive_message);
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
        if (strstr(msg, "room_list")) {
			data = get_data(msg);
            printf("%s\n",msg);
            puts(data);
            server_respond_choose_room_button(data);
		}
		if (strstr(msg, "join_room_error")) {
			puts(msg);
            init_result_game_window();
            room_full_notice();
		}
		if (strstr(msg, "new_opponent_message")) {
			data = get_data(msg);
            set_message(data);

		}
		if (strstr(msg, "wait_player")) {	
            //hide_room_select();
            wait_player_window("wait_key");
		}
        if (strstr(msg, "refresh_room")) {
            data = get_data(msg);
            //puts(data);
            hide_room_select();
            init_play_window(data);
		}
        if (strstr(msg, "new_play")) {
			data = get_data(msg);
            puts(data);
            printf("\n");
            set_move(data);	
		}
        if (strstr(msg, "your_turn")) {
            flag_turn = 1;
            printf("turn: %d\n", flag_turn );
		}
        if (strstr(msg, "opponent_turn")) {
            flag_turn = 0;
            printf("turn: %d\n", flag_turn );
		}
        if (strstr(msg, "you_won_game")) {
            init_result_game_window();
            win_game();
            //flag_win = 1;
		}
        if (strstr(msg, "you_lose_game")) {
            init_result_game_window();
            lose_game();
            //flag_win = 0;
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
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | O_ASYNC))
        printf("Error in setting socket to async, nonblock mode");

    signal(SIGIO, recv_msg); // assign SIGIO to the handler
    // set this process to be the process owner for SIGIO signal
    
    if (fcntl(clientSocket, F_SETOWN, getpid()) < 0)
        printf("Error in setting own to socket");
    char message[100];
    strcpy(message, "/confirm_connection");
    send(clientSocket, message, strlen(message)+1, 0);

    init_home_window();



	gtk_main();
    gdk_threads_leave();
	close(clientSocket);
	return 0;		
}
