void send_name(char *send_buffer)
{
    char setname_cmd[LENGTH_MSG];
	sprintf(setname_cmd, "/setname %s", send_buffer);
    send(clientSocket, setname_cmd, strlen(setname_cmd)+1, 0);
}
void send_disconnect()
{
    send(clientSocket, "/disconnect", strlen("/disconnect"), 0);
}
void send_room(char *send_buffer)
{
    char room_cmd[LENGTH_MSG];
	sprintf(room_cmd, "/enterRoom %s", send_buffer);
    send(clientSocket, room_cmd, strlen(room_cmd)+1, 0);
}
void send_leave_room()
{
    send(clientSocket, "/leaveRoom", strlen("/leaveRoom"), 0);
}
void send_play(int x, int y)
{
    char play_cmd[LENGTH_MSG];
    sprintf(play_cmd,"/play %d",iLocation[0] + iLocation[1]*10);
    send(clientSocket, play_cmd, strlen(play_cmd)+1, 0);
}
void send_choose_room()
{
    send(clientSocket, "/chooseRoom", strlen("/chooseRoom"), 0);
}
void send_msg(char *send_buffer)
{
    char msg_cmd[LENGTH_MSG];
	sprintf(msg_cmd, "/new_message %s", send_buffer);
    send(clientSocket, msg_cmd, strlen(msg_cmd)+1, 0);
}

void send_world_message(char *message) {
    char msg_cmd[LENGTH_MSG];
	sprintf(msg_cmd, "/new_world_message %s", message);
    send(clientSocket, msg_cmd, strlen(msg_cmd)+1, 0);

}