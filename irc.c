/*
Naming conventions!
i prefix means index in given array
l prefix means length of array
*/

#include "irc.h"

/*
Receives the next raw message from the server.
The function assumes that the server sends \r
chars only as part of the message separator-
*/

int bot_raw_read(int sockfd, char *msg)
{
	static char buffer[BUFFERSIZE] = {0};
	static char *bufpos = buffer;

	int lmsg;
	int lbuf;
	char *bufmsg;
	char *msgpos = msg;

	while ((bufmsg = strchr(bufpos, '\r')) == NULL)													// if the buffer contains just a part of a message one needs to receive the remainder from the socket
	{
		strcpy(msgpos, bufpos);																		// saving the first part into the msg array
		msgpos += strlen(msgpos);																	// store the position where the remainder will be copied

		lbuf = recv(sockfd, buffer, BUFFERSIZE-1, 0);												// receive data into the buffer, restore the buffer position
		buffer[lbuf] = 0;
		bufpos = buffer;

		bufmsg = strchr(bufpos, '\r');																// search for the end of the remainder
	}
	lmsg = bufmsg - bufpos + 2;																		// length of the message (note that bufmsg points to the beginnig of the searched string)
	memcpy(msgpos, bufpos, lmsg);
	msgpos[lmsg] = 0;
	bufpos += lmsg;
	return strlen(msg);
}

/*
Parses a raw message and stores the parts in a message struct.
*/

int bot_read(int sockfd, struct message *msg)
{
	int len;
	char rawmsg[MAXMSGSIZE];
	char *rawpos = rawmsg;
	char *end;

	bot_raw_read(sockfd, rawmsg);
	if (rawpos[0] == ':') 																			// the message has a prefix
	{
		rawpos += 1;
		end = strchr(rawpos, ' ');																	// search for the end of the prefix
		len = end - rawpos;
		memcpy(msg->prefix, rawpos, len);
		msg->prefix[len] = 0;
		rawpos += len + 1;																			// setting the raw message pointer to the first char of the command
	}
	else
	{
		msg->prefix[0] = 0;																			// if the message has no prefix the msg->prefix string has to be a null string
	}
	if ((end = strchr(rawpos, ' ')) == NULL)														// the message has no parameters
	{
		end = strchr(rawpos, '\r');
		end[0] = 0;
		strcpy(msg->command, rawpos);																// copying the command
		msg->param[0] = 0;																			// parameters is a null string
	}
	else																							// end points to space right after the command
	{
		len = end - rawpos;
		memcpy(msg->command, rawpos, len);
		msg->command[len] = 0;
		rawpos += len + 1;
		end = strchr(rawpos, '\r');
		end[0] = 0;
		strcpy(msg->param, rawpos);
	}
	return 0;
}

/*
Sends the given message to the server.
Note that a client - like this bot - SHOULD NOT use a prefix when sending a message, 
the only valid prefix is the registered nickname. Due to that the prefix string
is ignored.
*/

int bot_send(int sockfd, struct message *msg)
{
	int len;
	char rawmsg[MAXMSGSIZE+1];
	len = sprintf(rawmsg, "%s %s\r\n", msg->command, msg->param);
	return send(sockfd, rawmsg, len, 0);
}

/*
Connects the bot to the given channel on the specified server.
*/
int bot_login(const char *server_ip, const char *channel_name, const char *bot_name)
{
	int len, sockfd = socket(PF_INET, SOCK_STREAM, 0);												// initializing communication
	char *token, msg[MAXMSGSIZE+1];
	struct sockaddr_in addr; 
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(server_ip);

	connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));							// connecting to the server
	bot_raw_read(sockfd, msg);
	len = sprintf(msg, "NICK %s\r\n", bot_name);
	send(sockfd, msg, len, 0);
	len = sprintf(msg, "USER %s 8 *  : %s\r\n", bot_name, bot_name);
	send(sockfd, msg, len, 0);
	do
	{
		len = bot_raw_read(sockfd, msg);
		msg[len] = 0;
	}
	while(strncmp(msg, "PING :", 6) != 0);
	msg[1] = 'O';
	send(sockfd, msg, len, 0);
	do
	{
		len = bot_raw_read(sockfd, msg);
		msg[len] = 0;
		token = strchr(msg, ' ');
	}
	while(strncmp(token, " 376", 4) != 0);
	len = sprintf(msg, "JOIN %s\r\n", channel_name);
	send(sockfd, msg, len, 0);
	return sockfd;
}
