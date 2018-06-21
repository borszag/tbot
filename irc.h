#ifndef IRC_H
#define IRC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 6667 		// port used to connect to the irc server
#define MAXMSGSIZE 512 // the maximal size of messages recieved from the server
#define BUFFERSIZE 4096	// size of the buffer

struct message
{
	char *prefix;
	char *command;
	char *param;
};

int bot_read(int, struct message *);
int bot_send(int, struct message *);
int bot_login(const char *, const char *, const char *);

#endif