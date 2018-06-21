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

/*
BOT_COMMAND
This service seeks for commands sent to the bot by users.
If there is one, then it prepares the message for another service.
*/

int seek_bot_command(struct message *msg, union unified_param p)
{
	char param[MAXMSGSIZE];
	int len;
	len = sprintf(param, "%s :%%", p.name);

	return (!strcmp(msg->command, "PRIVMSG") && !strncmp(msg->param, param, len));
}

int serv_bot_command(struct message *msg, union unified_param p)
{
	int len;
	char temp[MAXMSGSIZE];
	char *compos = strchr(msg->param, ' ') + 2;
	char *parpos;
	if ((parpos = strchr(compos, ' ')) == NULL) // there are no parameters
	{
		strcpy(msg->command, compos);
		msg->param[0] = 0;
	}
	else
	{
		len = parpos - compos;
		memcpy(msg->command, compos, len);
		msg->command[len] = 0;
		parpos += 1;
		strcpy(temp, parpos);
		strcpy(msg->param, temp);
	}
	printf("COMAND:%s\nPARAMS:%s\n", msg->command, msg->param);
	return 0;
}