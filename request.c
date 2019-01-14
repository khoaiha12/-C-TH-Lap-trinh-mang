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

