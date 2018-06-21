/*
Services can modify the message so it's possible to prepare a modified message for another service.
Example: Bot gets a PRIVMSG and the message contains some kind of command to execute.
*/

#include "irc.h"
#include "service.h"

int main(int argc, char *argv[])
{
	int i, sockfd, len;
	char pre[512], com[512], par[512];
	struct message msg = {.prefix = pre, .command = com, .param = par};
	sockfd = bot_login(argv[1], argv[2], argv[3]);
	
	struct service services[SERVICECOUNT] = {
		/*PING service*/
		{
			.seek = seek_ping,
			.seek_param.sockfd = sockfd,
			.serv = serv_ping,
			.serv_param.sockfd = sockfd
		},
		/*BOT_COMMAND service*/
		{
			.seek = seek_bot_command,
			.seek_param.name = argv[3],
			.serv = serv_bot_command,
			.serv_param.name = argv[3]
		}
	};
	while(1)
	{
		bot_read(sockfd, &msg);
		for (i = 0; i < SERVICECOUNT; ++i)
		{
			if (services[i].seek(&msg, services[i].seek_param))
			{
				services[i].serv(&msg, services[i].serv_param);
			}
		}
	}
	shutdown(sockfd, SHUT_RDWR);
	return 0;
}