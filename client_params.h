#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8888

#define LENGTH_MSG 1024
#define BUFF_SIZE 1024

#define ROOM_SIZE 2
#define ROOM_NUM 10

typedef struct Room1 {
    int id;
    int client_num;
} Room1;

typedef struct Client {
    int connfd;
    int room_id;
    char name[100];
} Client;

int clientSocket;
Room1 room_arr[ROOM_NUM]; 
struct Queue *responses;
int iLocation [2];
int flag_turn;