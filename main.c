#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include "card.h"
#include "player.h"
#include "client_poker.h"
#include "serveur_poker.h"

int randomInt(int min, int max);

int main(int argc, char** argv) {
	srand(time(NULL));

	if(argc < 3) {
		serveur();
	} else {
		client(argv);
	}

	return EXIT_SUCCESS;
}

int randomInt(int min, int max) {
	return rand() % (max - min) + min;
}
