#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 6667 		// az a port, ahova a kliens csatlakozni fog

#define MAXDATASIZE 513 // az egyszerre kapható bájtok maximális értéke

#define FPNUM 1 		// megvalósított funkciók száma

union unified_param		// adatszerkezet, amelyen keresztül egységesíthetőek a függvényhívások
{
	int 	 *i;
	int 	**ia;
	char 	**c;
};

int login()				// a szerrverre, illetve az adott csatornába beléptető függvény
{
	int sockfd, len;
	char buffer[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in addr; // a csatlakozó címinformációja

	he=gethostbyname("irc.quakenet.org");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET; // host byte order
	addr.sin_port = htons(PORT);
	addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);

	connect(sockfd, (struct sockaddr *)&addr,sizeof(struct sockaddr));
	sleep(5);

	send(sockfd, "NICK tbot\r\n", 11, 0);
	send(sockfd, "USER tbot 8 *  : tbot\r\n", 23, 0);
	do
	{
		len = recv(sockfd, buffer, MAXDATASIZE-1, 0);
	} while(strncmp(buffer, "PING :", 6) != 0);

	buffer[1] = 'O';

	send(sockfd, buffer, len, 0);
	sleep(15);
	send(sockfd, "JOIN #konolytalan\r\n", 20, 0) == -1;
	return sockfd;
}
/*
-*	A bot által végezhető feladatokhoz tartozó függvények,
-*	előbb ami felismer, aztán ami végrehajt.
 */
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