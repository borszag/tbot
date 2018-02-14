#ifndef TBOT_GLOBAL
#define TBOT_GLOBAL

/*
 * Global structures and type definitions
 */

typedef struct {
	int is_running; //running flag, replace with bitfield later
	int sockfd; //what the hell does fd mean
} core;

typedef union {
	int*   i;
	int**  ia;
	char** cr;
} unified_param;

/*
 * Global function prototypes
 */

int load(const char*);
/*
 * loads bot settings from file, returns 1 if all goes well
 * loadable settings are quakenet ip, channel name, nickname, user database, file pointers
 * to log files, to pm files, misc data (version, last online date)
 */

int login(const char*, const char*, const char*);
/*
 * connects the bot to specified irc server and channel, using the specified nickname
 * needs to handle occupied nickname!
 * returns sockfd T please name your variables like a normal person...
 */


#endif
