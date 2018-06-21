/*
Services can modify the message so it's possible to prepare a modified message for another service.
Example: Bot gets a PRIVMSG and the message contains some kind of command to execute.
*/

#include "irc.h"
#include "service.h"

int main(void)
{
	int i, sockfd, len;
	char pre[512], com[512], par[512];
	struct message msg = {.prefix = pre, .command = com, .param = par};

	sockfd = bot_login("83.140.172.212", "tbot", "#konolytalan");
	
	struct service services[SERVICECOUNT] = {
		{.p.sockfd = sockfd, .seek = seek_ping, .serv = serv_ping} /*Service responding to the PINGs of the server.*/
	};
	while(1)
	{
		bot_read(sockfd, &msg);
		for (i = 0; i < SERVICECOUNT; ++i)
		{
			if(services[i].seek(&msg))
			{
				services[i].serv(&msg, services[i].p);
			}
		}
	}
	shutdown(sockfd, SHUT_RDWR);
	return 0;
}