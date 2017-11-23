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

int randomInt(int min, int max);
Card* merge(Card* a, Card* b);
bool pair(Card* croupier, Card* hand);
bool twoPair(Card* croupier, Card* hand);
bool threeOfKind(Card* croupier, Card* hand);
bool flush(Card* croupier, Card* hand);
Card* higherHand(Card* croupier, Card** hands, int nbHands);
void display(Card c);
Card* createDeck();
Card getFromDeck(Card* deck, int index, int length);
void addToDeck(Card* deck, Card c, int length);
char* cardToString(Card c);
int handValue(Card* croupier, Card* hand);

Player newPlayer(Player* players, int nbPlayers);

int main(int argc, char** argv) {

	int i = 0, nbPlayers = 0;
	srand(time(NULL));

	if(argc < 3) {
		int sockserveur, newsockfd, lg;
		struct sockaddr_in coord_client;
		struct sockaddr_in mes_coord;
		struct tm *m;
		char* pc;
		char msg[100];
	/* Création d'un socket */
		sockserveur = socket(AF_INET, SOCK_STREAM, 0);
	/* Serveur : appel BIND */
		bzero(&mes_coord, sizeof(mes_coord));
		mes_coord.sin_family = AF_INET;
		mes_coord.sin_port = htons(2000);
		mes_coord.sin_addr.s_addr = htonl(INADDR_ANY);
		if(bind(sockserveur, (struct sockaddr*) &mes_coord, sizeof(mes_coord)) < 0) {
			printf("Erreur BIND\n");
			exit(0);
		}
		/* Serveur : appel LISTEN */
		if (listen(sockserveur, 5) < 0) {
			printf("Erreur LISTEN\n");
			exit(0);
		}

		Player* players = NULL;

		int remainingCards = 52;
		Card* deck = createDeck();
		Card* croupier = (Card*) malloc(sizeof(Card) * 3);
		for(i = 0; i < 3; i++) {
			croupier[i] = getFromDeck(deck, randomInt(0, remainingCards), remainingCards);
			remainingCards--;
		}

		/* Serveur : appel ACCEPT */
		while(1) {
			lg = sizeof(coord_client);

			Player p = newPlayer(players, nbPlayers);
			p.budget = 100;
			p.hand = (Card*) malloc(sizeof(Card) * 2);
			p.address = accept(sockserveur, (struct sockaddr*) &coord_client, &lg);
			if(p.address == (-1)) {
				printf("Erreur ACCEPT\n");
				exit(0);
			}

			nbPlayers++;
			int hand[2];
			for(i = 0; i < 2; i++) {
				hand[i] = randomInt(0, remainingCards);
				p.hand[i] = getFromDeck(deck, hand[i], remainingCards);
				remainingCards--;
			}

			char msg_srv1[100] = "Votre main : \n";
			strcat(msg_srv1, cardToString(p.hand[0]));
			strcat(msg_srv1, " - ");
			strcat(msg_srv1, cardToString(p.hand[1]));
			write(p.address, msg_srv1, sizeof(msg_srv1));

			sleep(1);
			char msg_srv2[100] = "La main du croupier : \n";
			strcat(msg_srv2, "\t 1ère carte : ");
			strcat(msg_srv2, cardToString(croupier[0]));
			strcat(msg_srv2, "\nMiser (1) ou Arrêter (2)");
			write(p.address, msg_srv2, sizeof(msg_srv2));

			char msg_srv3[100];
			read(p.address, msg_srv3, sizeof(msg_srv3));
			if(strcmp(msg_srv3, "2") == 0) {
				free(deck);
				free(croupier);
				free(players);
				close(p.address);
				close(sockserveur);
				exit(EXIT_SUCCESS);
			}

			char msg_srv4[100] = "";
			strcat(msg_srv4, "\t 2è carte : ");
			strcat(msg_srv4, cardToString(croupier[1]));
			strcat(msg_srv4, "\nMiser (1) ou Arrêter (2)");
			write(p.address, msg_srv4, sizeof(msg_srv4));

			char msg_srv5[100];
			read(p.address, msg_srv5, sizeof(msg_srv5));
			if(strcmp("2", msg_srv5) == 0) {
				free(deck);
				free(croupier);
				free(players);
				close(p.address);
				close(sockserveur);
				exit(EXIT_SUCCESS);
			}

			char msg_srv6[100] = "";
			strcat(msg_srv6, "\t 3è carte : ");
			strcat(msg_srv6, cardToString(croupier[2]));
			strcat(msg_srv6, "\nMiser (1) ou Arrêter (2)");
			write(p.address, msg_srv6, sizeof(msg_srv6));

			char msg_srv7[100];
			read(p.address, msg_srv7, sizeof(msg_srv7));
			if(strcmp("2", msg_srv7) == 0) {
				free(deck);
				free(croupier);
				free(players);
				close(p.address);
				close(sockserveur);
				exit(EXIT_SUCCESS);
			}

			char msg_srv8[100] = "Votre meilleure combinaison : ";
			int v = handValue(croupier, p.hand);
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
		free(croupier);
		free(players);
		close(sockserveur);

	} else {

		struct sockaddr_in coord_serveur;
		int sockclient;
		struct tm *m;
		char* pc;
		u_short port;
		int nb;

		port = atoi(argv[2]);
		sockclient = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&coord_serveur, sizeof(coord_serveur));
		coord_serveur.sin_family = AF_INET;
		coord_serveur.sin_port = htons(port);
		coord_serveur.sin_addr.s_addr = inet_addr(argv[1]);
		if(connect(sockclient, (struct sockaddr *) &coord_serveur, sizeof(coord_serveur)) < 0) {
			printf ("Erreur connect\n");
			exit(EXIT_SUCCESS);
		}

		char msg_cl1[100];
		read(sockclient, msg_cl1, sizeof(msg_cl1));
		printf("%s\n", msg_cl1);

		char msg_cl2[100];
		read(sockclient, msg_cl2, sizeof(msg_cl2));
		printf("%s\n", msg_cl2);

		char msg_cl3[100];
		int input = 0;
		while(input != 1 && input != 2) {
			scanf("%d", &input);
			if(input == 2) {
				close(sockclient);
				exit(EXIT_SUCCESS);
			}
		}
		sprintf(msg_cl3, "%d", input);
		write(sockclient, msg_cl3, sizeof(msg_cl3));

		char msg_cl4[100];
		read(sockclient, msg_cl4, sizeof(msg_cl4));
		printf("%s\n", msg_cl4);

		char msg_cl5[100];
		input = 0;
		while(input != 1 && input != 2) {
			scanf("%d", &input);
			if(input == 2) {
				close(sockclient);
				exit(EXIT_SUCCESS);
			}
		}
		sprintf(msg_cl5, "%d", input);
		write(sockclient, msg_cl5, sizeof(msg_cl5));

		char msg_cl6[100];
		read(sockclient, msg_cl6, sizeof(msg_cl6));
		printf("%s\n", msg_cl6);

		char msg_cl7[100];
		input = 0;
		while(input != 1 && input != 2) {
			scanf("%d", &input);
			if(input == 2) {
				close(sockclient);
				exit(EXIT_SUCCESS);
			}
		}
		sprintf(msg_cl7, "%d", input);
		write(sockclient, msg_cl7, sizeof(msg_cl7));

		char msg_cl9[100];
		read(sockclient, msg_cl9, sizeof(msg_cl9));
		printf("%s\n", msg_cl9);

		close(sockclient);
	}

	return EXIT_SUCCESS;
}

char* cardToString(Card c) {
	char* result = malloc(sizeof(char) * 100);
	char val[10];

	switch(c.val) {
		case 1:
		strcat(result, "As");
		break;
		case 11:
		strcat(result, "Valet");
		break;
		case 12:
		strcat(result, "Dame");
		break;
		case 13:
		strcat(result, "Roi");
		break;
		default:
		sprintf(val, "%d", c.val);
		strcat(result, val);
		break;
	}
	switch(c.col) {
		case 1:
		strcat(result, " de pique.");
		break;
		case 2:
		strcat(result, " de carreau.");
		break;
		case 3:
		strcat(result, " de trèfle.");
		break;
		case 4:
		strcat(result, " de coeur.");
		break;
	}

	return result;
}

void display(Card c) {
	switch(c.val) {
		case 1:
		printf("As");
		break;
		case 11:
		printf("Valet");
		break;
		case 12:
		printf("Dame");
		break;
		case 13:
		printf("Roi");
		break;
		default:
		printf("%d", c.val);
		break;
	}
	switch(c.col) {
		case 1:
		printf(" de pique.");
		break;
		case 2:
		printf(" de carreau.");
		break;
		case 3:
		printf(" de trèfle.");
		break;
		case 4:
		printf(" de coeur.");
		break;
	}
	printf("\n");
}

Player newPlayer(Player* players, int nbPlayers) {
	Player* tmp = (Player*) realloc(players, (nbPlayers + 1) * sizeof(Player));
	Player p;
	tmp[nbPlayers] = p;
	return p;
}

Card* createDeck() {
	Card* deck = (Card*) malloc(sizeof(Card) * 52);
	int i = 0, y = 0, c = 0;
	for(i = 1; i <= 4; i++) {
		for(y = 1; y <= 13; y++) {
			deck[c].col = i;
			deck[c].val = y;
			c++;
		}
	}
	return deck;
}

Card getFromDeck(Card* deck, int index, int length) {
	int i;
	Card c = deck[index];
	if(length == 1) {
		free(deck);
		return c;
	}

	for(i = index; i < length - 1; i++) deck[i] = deck[i + 1];
		Card *tmp = (Card*) realloc(deck, (length - 1) * sizeof(Card));
	if (tmp == NULL && length > 1) {
	   /* No memory available */
		exit(EXIT_FAILURE);
	}
	deck = tmp;
	return c;
}

void addToDeck(Card* deck, Card c, int length) {
	Card* tmp = (Card*) realloc(deck, (length + 1) * sizeof(Card));
	tmp[length] = c;
	deck = tmp;
}

Card* higherHand(Card* croupier, Card** hands, int nbHands) {
	Card* result = NULL;
	int i = 0, higher = 0, current = 0;

	for(i = 0; i < nbHands; i++) {
		current = handValue(croupier, hands[i]);
		if(higher < current) {
			higher = current;
			result = hands[i];
		}
	}
	return result;
}

int handValue(Card* croupier, Card* hand) {
	int higher = 0;
	if(flush(croupier, hand)) {
		higher = 4;
	}
	if(higher < 3 && threeOfKind(croupier, hand)) {
		higher = 3;
	}
	if(higher < 2 && twoPair(croupier, hand)) {
		higher = 2;
	}
	if(higher < 1 && pair(croupier, hand)) {
		higher = 1;
	}
	return higher;
}

bool pair(Card* croupier, Card* hand) {
	Card* c = merge(hand, croupier);
	int i = 0, y = 0, count = -5;
	for(y = 0; y < 5; y++) {
		for(i = 0; i < 5; i++) {
			if(c[i].val == c[y].val) {
				count++;
			}
		}
	}
	free(c);

	return (count == 2);
}

bool twoPair(Card* croupier, Card* hand) {
	Card* c = merge(hand, croupier);

	int i = 0, y = 0, count = -5;
	for(y = 0; y < 5; y++) {
		for(i = 0; i < 5; i++) {
			if(c[i].val == c[y].val) {
				count++;
			}
		}
	}
	free(c);

	return (count == 4);
}

bool threeOfKind(Card* croupier, Card* hand) {
	Card* c = merge(hand, croupier);
	int i = 0, y = 0, count = 0, current = 0;

	for(y = 0; y < 5; y++) {
		current = c[y].val;
		for(i = 0; i < 5 ; i++) {
			if(current == c[i].val) {
				count++;
			}
		}
		if(count == 3) {
			free(c);
			return true;
		} else {
			count = 0;
		}
	}

	free(c);
	return false;
}

bool flush(Card* croupier, Card* hand) {
	Card* c = merge(hand, croupier);
	int i = 0, y = 0, count = 0, current = 0;

	for(y = 0; y < 5; y++) {
		current = c[y].col;
		for(i = 0; i < 5 ; i++) {
			if(current == c[i].col) {
				count++;
			}
		}
		if(count == 5) {
			free(c);
			return true;
		} else {
			count = 0;
		}
	}

	free(c);
	return false;
}

Card* merge(Card* a, Card* b) {
	Card *c = (Card*) malloc(sizeof(Card) * 5);
	c[0] = a[0];
	c[1] = a[1];
	int i = 0;
	for(i = 2; i < 5; i++) {
		c[i] = b[i-2];
	}
	return c;
}

int randomInt(int min, int max) {
	return rand() % (max - min) + min;
}
