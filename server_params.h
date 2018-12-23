#define PORT 8888
#define CLIENT_MAX 10

#define LENGTH_MSG 1024
#define BUFF_SIZE 1024

#define ROOM_SIZE 2
#define ROOM_NUM 8

typedef struct Room {
    int id;
    int client_num;
} Room;