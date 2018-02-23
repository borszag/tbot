#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 6667 		// port used to connect to the irc server
#define MAXDATASIZE 513 // the maximal size of messages recieved from the server
#define FPNUM 1 		// number of functions in main scan-exec loop

union unified_param		// used to unify the parameter list of exec functions
{
	int 	 *i;
	int 	**ia;
	char 	**c;
};

struct function
{
	union unified_param param;
	int (*scan)(char *, unsigned);
	int (*exec)(char *, unsigned, union unified_param);
};

int login(const char *server_ip, const char *bot_name, const char *channel_name)				// it connects the bot to the server and joins it into a channel
{
	int len, sockfd = socket(PF_INET, SOCK_STREAM, 0);
	char *token, buffer[MAXDATASIZE];
	struct sockaddr_in addr; 
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(server_ip);

	connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	recv(sockfd, buffer, MAXDATASIZE-1, 0);
	len = sprintf(buffer, "NICK %s\r\n", bot_name);
	send(sockfd, buffer, len, 0);
	len = sprintf(buffer, "USER %s 8 *  : %s\r\n", bot_name, bot_name);
	send(sockfd, buffer, len, 0);
	do
	{
		len = recv(sockfd, buffer, MAXDATASIZE-1, 0);
		buffer[len] = 0;
	}
	while(strncmp(buffer, "PING :", 6) != 0);
	buffer[1] = 'O';

	send(sockfd, buffer, len, 0);
	do
	{
		len = recv(sockfd, buffer, MAXDATASIZE-1, 0);
		buffer[len] = 0;
		token = strchr(buffer, ' ');
	}
	while(strncmp(token, " 376", 4) != 0);
	len = sprintf(buffer, "JOIN %s\r\n", channel_name);
	send(sockfd, buffer, len, 0);
	return sockfd;
}
/*/
-*-----------------------------------------------------
-* All scan-exec functions are implemented after this mark
-* Naming convention: scan functions begin with s_ prefix, while exec ones with e_
-*-----------------------------------------------------
/*/
int s_ping(char *msg, unsigned msi)
{
	return !strncmp(msg, "PING :", 6);
}

int e_ping(char *msg, unsigned msi, union unified_param up)
{
	int ret;
	msg[1] = 'O';
	ret = send(*(up.i), msg, msi, 0);
	msg[1] = 'I';
	return ret;
}

int main(void)
{
	int i, sockfd, ret_size;
	char buffer[MAXDATASIZE];

	struct function functions[FPNUM] = {
		{.param.i = &sockfd, .scan = s_ping, .exec = e_ping} /*Function resoinding to the PINGs of the server.*/
	};

	sockfd = login("83.140.172.212", "tbot", "#konolytalan");
	while(1)
	{
		ret_size=recv(sockfd, buffer, MAXDATASIZE-1, 0);
		buffer[ret_size] = '\0';
		printf("%s", buffer);
		for (i = 0; i < FPNUM; ++i)
		{
			if(functions[i].scan(buffer, ret_size))
			{
				functions[i].exec(buffer, ret_size, functions[i].param);
			}
		}
	}
	shutdown(sockfd, SHUT_RDWR);
	return 0;
}