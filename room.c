#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "room.h"
#include "player.h"




void setDefault(room roomList[], int max) {
    int i=0;
    for(i=0; i < max; i++) {
        roomList[i].id = i;
        roomList[i].Player1 = 0;
        roomList[i].Player2 = 0;
        roomList[i].inGame = 0;
        roomList[i].turn = 0;
        int x,y;
	    for (y = 0; y <= 9; y++)
	    {
            for (x = 0; x <= 9; x++)
            {
                roomList[i].Board[x][y] = 'E';
            }
	    }
    }
}

int enterRoom(int player, room roomList[], int roomNumber) {
    if (roomList[roomNumber].Player1 == 0 || roomList[roomNumber].Player2 == 0) {
        if(roomList[roomNumber].Player1 == 0) {
            roomList[roomNumber].Player1 = player;
            return 1;
        }
        if(roomList[roomNumber].Player2 == 0) {
            roomList[roomNumber].Player2 = player;
            return 1;
        }
    }
    else return 0;
}

int leaveRoom(int player, room roomList[]) {
    int roomNumber;
    for(roomNumber=0; roomNumber<10; roomNumber++) {
        if (roomList[roomNumber].Player1 == player) {
            roomList[roomNumber].Player1 = 0;
            roomList[roomNumber].inGame = 0;
            return 1;
        }
        if (roomList[roomNumber].Player2 == player) {
            roomList[roomNumber].Player2 = 0;
            roomList[roomNumber].inGame = 0;
            return 1;
        }
    }
    return 0;
}

int inRoom(int player, room roomList[]) {
    int roomNumber;
    for(roomNumber=0; roomNumber<10; roomNumber++) {
        if (roomList[roomNumber].Player1 == player) {
            return roomNumber;
        }
        if (roomList[roomNumber].Player2 == player) {
            return roomNumber;
        }
    }
    return -1;

}
void printRoomList(room roomList[]) {
    int i;
    for(i=0; i<10; i++) {
        if(roomList[i].Player1 != 0 || roomList[i].Player2 != 0) {
            printf("Room %d:\n", i);
            printf("Player 1: %d\n", roomList[i].Player1);
            printf("Player 2: %d\n", roomList[i].Player2);
        }
    }
}
int startGame(room roomList[], int roomNumber) {
    if (roomList[roomNumber].Player1 != 0 && roomList[roomNumber].Player2 != 0) {
        roomList[roomNumber].inGame = 1;
        roomList[roomNumber].turn = 1;
        return 1;
    }
    return 0;
}

void changeTurn(room roomList[], int roomNumber) {
    if (roomList[roomNumber].Player1 != 0 && roomList[roomNumber].Player2 != 0 && roomList[roomNumber].inGame == 1) {
        if (roomList[roomNumber].turn == 1) {
            roomList[roomNumber].turn = 2;
        } else {
            roomList[roomNumber].turn = 1;
        }
    }
}

int countPlayerInRoom(room roomList[], int roomNumber) {
    if(roomList[roomNumber].Player1 == 0 && roomList[roomNumber].Player2 == 0)
        return 0;
    if(roomList[roomNumber].Player1 != 0 && roomList[roomNumber].Player2 == 0)
        return 1;
    if(roomList[roomNumber].Player1 == 0 && roomList[roomNumber].Player2 != 0)
        return 1;
    if(roomList[roomNumber].Player1 != 0 && roomList[roomNumber].Player2 != 0)
        return 2;
}