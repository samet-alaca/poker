void serveur() {
	int sockserveur, lg, i = 0, nbPlayers = 0, remainingCards = 52, hand[2], v = 0;
	struct sockaddr_in coord_client;
	struct sockaddr_in mes_coord;
	struct tm *m;
	char* pc;
	char msg_srv1[100], msg_srv2[100], msg_srv3[100], msg_srv4[100], msg_srv5[100],
		msg_srv6[100], msg_srv7[100], msg_srv8[100];
	Card* deck = createDeck(), croupier[3];
	Player* players = NULL;

	sockserveur = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&mes_coord, sizeof(mes_coord));
	mes_coord.sin_family = AF_INET;
	mes_coord.sin_port = htons(2000);
	mes_coord.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockserveur, (struct sockaddr*) &mes_coord, sizeof(mes_coord)) < 0) {
		printf("Erreur BIND\n");
		exit(EXIT_FAILURE);
	}

	if(listen(sockserveur, 5) < 0) {
		printf("Erreur LISTEN\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < 3; i++) {
		croupier[i] = getFromDeck(deck, randomInt(0, remainingCards), remainingCards);
		remainingCards--;
	}

	while(1) {
		lg = sizeof(coord_client);

		Player p = newPlayer(players, nbPlayers);
		p.budget = 100;
		p.hand = (Card*) malloc(sizeof(Card) * 2);
		p.address = accept(sockserveur, (struct sockaddr*) &coord_client, &lg);
		if(p.address == (-1)) {
			printf("Erreur ACCEPT\n");
			exit(EXIT_FAILURE);
		}

		nbPlayers++;

		for(i = 0; i < 2; i++) {
			hand[i] = randomInt(0, remainingCards);
			p.hand[i] = getFromDeck(deck, hand[i], remainingCards);
			remainingCards--;
		}

		strcpy(msg_srv1, "Votre main : \n");
		strcat(msg_srv1, cardToString(p.hand[0]));
		strcat(msg_srv1, " - ");
		strcat(msg_srv1, cardToString(p.hand[1]));
		write(p.address, msg_srv1, sizeof(msg_srv1));

		sleep(1);

		strcpy(msg_srv2, "La main du croupier : \n");
		strcat(msg_srv2, "\t 1ère carte : ");
		strcat(msg_srv2, cardToString(croupier[0]));
		strcat(msg_srv2, "\nMiser (1) ou Arrêter (2)");
		write(p.address, msg_srv2, sizeof(msg_srv2));

		read(p.address, msg_srv3, sizeof(msg_srv3));
		if(strcmp(msg_srv3, "2") == 0) {
			free(deck);
			free(players);
			close(p.address);
			close(sockserveur);
			exit(EXIT_SUCCESS);
		}

		strcpy(msg_srv4, "\t 2è carte : ");
		strcat(msg_srv4, cardToString(croupier[1]));
		strcat(msg_srv4, "\nMiser (1) ou Arrêter (2)");
		write(p.address, msg_srv4, sizeof(msg_srv4));

		read(p.address, msg_srv5, sizeof(msg_srv5));
		if(strcmp("2", msg_srv5) == 0) {
			free(deck);
			free(players);
			close(p.address);
			close(sockserveur);
			exit(EXIT_SUCCESS);
		}

		strcpy(msg_srv6, "\t 3è carte : ");
		strcat(msg_srv6, cardToString(croupier[2]));
		strcat(msg_srv6, "\nMiser (1) ou Arrêter (2)");
		write(p.address, msg_srv6, sizeof(msg_srv6));

		read(p.address, msg_srv7, sizeof(msg_srv7));
		if(strcmp("2", msg_srv7) == 0) {
			free(deck);
			free(players);
			close(p.address);
			close(sockserveur);
			exit(EXIT_SUCCESS);
		}

		strcpy(msg_srv8, "Votre meilleure combinaison : ");
		v = handValue(croupier, p.hand);
		switch(v) {
			case 1:
				strcat(msg_srv8, "pair");
				break;
			case 2:
				strcat(msg_srv8, "double pair");
				break;
			case 3:
				strcat(msg_srv8, "brelan");
				break;
			case 4:
				strcat(msg_srv8, "couleur");
				break;
			default:
				strcat(msg_srv8, " rien...");
				break;
		}
		write(p.address, msg_srv8, sizeof(msg_srv8));

		close(p.address);
	}
	free(deck);
	free(players);
	close(sockserveur);
}
