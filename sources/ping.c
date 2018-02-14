#include <sys/socket.h>
#include "../headers/global.h"
#include "../headers/executors.h"

int e_ping(char* message, int msi, unified_param up) {
	int ret;
	message[1] = 'O';
	ret = send(*(up.i), message, msi, 0);
	message[1] = 'I';
	return ret;
}
