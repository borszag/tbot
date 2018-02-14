#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../headers/constants.h"
#include "../headers/global.h"

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

