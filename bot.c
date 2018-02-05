#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 6667 		// az a port, ahova a kliens csatlakozni fog

#define MAXDATASIZE 513 // az egyszerre kapható bájtok maximális értéke

#define FPNUM 1 		// megvalósított funkciók száma

union indef_param		// adatszerkezet, amelyen keresztül egységesíthetőek a függvényhívások
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

	if ((he=gethostbyname("irc.quakenet.org")) == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET; // host byte order
	addr.sin_port = htons(PORT);
	addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);

	if (connect(sockfd, (struct sockaddr *)&addr,sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

	sleep(5);

	if (send(sockfd, "NICK tbot\r\n", 11, 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(sockfd, "USER tbot 8 *  : tbot\r\n", 23, 0) == -1) {
		perror("send");
		exit(1);
	}

	do
	{
		if ((len = recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}

	}while(strncmp(buffer, "PING :", 6) != 0);

	buffer[1] = 'O';

	if (send(sockfd, buffer, len, 0) == -1) {
		perror("send");
		exit(1);
	}
	sleep(15);

	if (send(sockfd, "JOIN #konolytalan\r\n", 20, 0) == -1) {
		perror("send");
		exit(1);
	}

	return sockfd;
}
/*
-*	A bot által végezhető feladatokhoz tartozó függvények,
-*	előbb ami felismer, aztán ami végrehajt.
 */
int isping(char *msg, unsigned msi)
{
	return !strncmp(msg, "PING :", 6);
}

int doping(char *msg, unsigned msi, union indef_param ipa)
{
	int ret;
	msg[1] = 'O';
	ret = send(*(ipa.i), msg, msi, 0);
	msg[1] = 'I';
	if (ret == -1)
	{
		perror("send");
		exit(1);
	}
	return ret;
}

int main(void)
{
	int i, sockfd, numbytes;
	char buffer[MAXDATASIZE];
	int (*scan[FPNUM])(char *, unsigned) = {isping};
	int (*exec[FPNUM])(char *, unsigned, union indef_param) = {doping};
	union indef_param param[FPNUM] = {{.i = &sockfd}};

	sockfd = login();
	while(1)
	{
		if ((numbytes=recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		buffer[numbytes] = '\0';
		printf("%s", buffer);
		for (i = 0; i < FPNUM; ++i)
		{
			if((*scan[i])(buffer, numbytes))
			{
				(*exec[i])(buffer, numbytes, param[i]);
			}
		}

	}

	close(sockfd);

	return 0;
}