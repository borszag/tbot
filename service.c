#include "service.h"

/*
PING
This service responds the pings sent by the server.
*/

int seek_ping(struct message *msg, union unified_param p)
{
	return !strcmp(msg->command, "PING");
}

int serv_ping(struct message *msg, union unified_param p)
{
	msg->command[1] = 'O';
	return bot_send(p.sockfd, msg);
}
