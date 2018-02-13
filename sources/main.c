#include "../headers/constants.h"
#include "../headers/global.h"

int main(void) {
	core program;
	program.is_running = load(SETTINGS_FILE); //handle file errors here?
	program.sockfd     = login(DEFAULT_SERVER_IP, DEFAULT_BOT_NICKNAME, DEFAULT_BOT_CHANNEL); //handle login errors here?
	
	while (program.is_running) {
		//magic
	}

	return 0;
}
