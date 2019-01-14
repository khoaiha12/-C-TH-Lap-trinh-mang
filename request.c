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
