#include <string.h>
#include "../../headers/constants.h"
#include "../../headers/global.h"
#include "../../headers/interpreters.h"

int i_ping(char* message, unsigned int msi) {
	return !strncmp(message, TRIGGER_PING, TRIGGER_PING_LENGTH);
}
