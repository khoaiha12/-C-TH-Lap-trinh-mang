#define MAX_ROOM 10

typedef struct Room {

} room;

int enterRoom(int player, room roomList[], int roomNumber);
int leaveRoom(int player, room roomList[]);
int inRoom(int player, room roomList[]);
void printRoomList(room roomList[]);
void setDefault(room roomList[], int max);
void changeTurn(room roomList[], int roomNumber);