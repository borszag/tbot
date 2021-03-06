/*
If there is a complex service then it can be written in separate files, but its headers should be included here.
The naming convention for separately written services is a "service_" prefix in the filenames.

seek functions should return a boolean, a nonzero value if that service needs to be run and zero if not. 
*/


#ifndef SERVICE_H
#define SERVICE_H 

#include <stdio.h>
#include <string.h>
#include "irc.h"

#define SERVICECOUNT 2 		// number of functions in main seek-serv loop

/*
The purpose of this is to unify the parameter list of the seek-serv functions.
If an serv function needs some special parameter then it should be added to this union.
Note that every member of this union should be some kind of pointer or a data type not
larger than that for performance reasons.
*/
union unified_param
{
	int sockfd;
	char *name;
};

struct service
{
	int (*seek)(struct message *, union unified_param);
	union unified_param seek_param;

	int (*serv)(struct message *, union unified_param);
	union unified_param serv_param;
};

/*
Function declarations
*/

/*PING*/
int seek_ping(struct message *, union unified_param);
int serv_ping(struct message *, union unified_param);
/*BOT_COMMAND*/
int seek_bot_command(struct message *msg, union unified_param p);
int serv_bot_command(struct message *msg, union unified_param p);
#endif