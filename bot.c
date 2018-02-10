#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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

int login()				// it connects the bot to the server and joins it into a channel
{
	int len, sockfd;
	char buffer[MAXDATASIZE];
	struct sockaddr_in addr; 
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("83.140.172.212");

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Login failed during socket creation");
		exit(1);
	}
	if (connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1)
	{
		perror("Login failed during connection to the server");
		exit(1);
	}
	sleep(5);
	if (send(sockfd, "NICK tbot\r\n", 11, 0) == -1)
	{
		perror("Login failed due to communication issues");
		exit(1);
	}
	if (send(sockfd, "USER tbot 8 *  : tbot\r\n", 23, 0) == -1)
	{
		perror("Login failed due to communication issues");
		exit(1);
	}
	do
	{
		if ((len = recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1)
		{
		perror("Login failed due to communication issues");
		exit(1);
		}
	}
	while(strncmp(buffer, "PING :", 6) != 0);
	buffer[1] = 'O';
	if (send(sockfd, buffer, len, 0) == -1)
	{
		perror("Login failed due to communication issues");
		exit(1);
	}
	sleep(15);
	if (send(sockfd, "JOIN #konolytalan\r\n", 20, 0) == -1)
	{
		perror("Login failed due to communication issues");
		exit(1);
	}
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
	int (*scan[FPNUM])(char *, unsigned) = {s_ping};
	int (*exec[FPNUM])(char *, unsigned, union unified_param) = {e_ping};
	union unified_param param[FPNUM] = {{.i = &sockfd}};

	sockfd = login();
	while(1)
	{
		ret_size=recv(sockfd, buffer, MAXDATASIZE-1, 0);
		buffer[ret_size] = '\0';
		printf("%s", buffer);
		for (i = 0; i < FPNUM; ++i)
		{
			if((*scan[i])(buffer, ret_size))
			{
				(*exec[i])(buffer, ret_size, param[i]);
			}
		}
	}
	shutdown(sockfd, SHUT_RDWR);
	return 0;
}