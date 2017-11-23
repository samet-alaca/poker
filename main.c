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

Player newPlayer(Player* players, int nbPlayers);

int main(int argc, char** argv) {

	Player* players = NULL;
	int i = 0, nbPlayers;
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
		mes_coord.sin_port = htons(2125);
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
			newsockfd = accept(sockserveur, (struct sockaddr*) &coord_client, &lg);
			if(newsockfd == (-1))
			{
				printf("Erreur ACCEPT\n");
				exit(0);
			}

			Player p = newPlayer(players, nbPlayers);
			p.address = newsockfd;
			p.budget = 100;
			p.hand = (Card*) malloc(sizeof(Card) * 2);
			nbPlayers++;
			int hand[2];
			for(i = 0; i < 2; i++) {
				hand[i] = randomInt(0, remainingCards);
				p.hand[i] = getFromDeck(deck, hand[i], remainingCards);
				remainingCards--;
			}

			char msg2[100] = "Votre main : \n";
			strcat(msg2, cardToString(p.hand[0]));
			strcat(msg2, " - ");
			strcat(msg2, cardToString(p.hand[1]));
			write(p.address, msg2, sizeof(msg2));

			

			sleep(1);
			strcpy(msg2, "La main du croupier : \0");
			strcat(msg2, "\n\t 1ère carte : ");
			strcat(msg2, cardToString(croupier[0]));
			strcat(msg2, "\nMiser (1) ou Arrêter (2)");
			write(p.address, msg2, sizeof(msg2));
			
			//msg2[0] = '\0';
			char msg3[100];
			read(p.address, msg3, 1);
			printf("%s\n", msg3);
			//int result = (int) strtol(msg2, (char **) NULL, 10);
			//printf("Réponse : %d", result);

			close(newsockfd);
		}
		free(deck);
		free(croupier);
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
		bzero(&coord_serveur,sizeof(coord_serveur));
		coord_serveur.sin_family = AF_INET;
		coord_serveur.sin_port = htons(port);
		coord_serveur.sin_addr.s_addr = inet_addr(argv[1]);
		if(connect(sockclient, (struct sockaddr *) &coord_serveur, sizeof(coord_serveur)) <0) 
		{ 
			printf ("erreur de connexion \n");
			exit(0);
		}

		char msg2[100];
		read(sockclient, msg2, sizeof(msg2));
		printf("%s\n", msg2);
		

		read(sockclient, msg2, sizeof(msg2));
		printf("%s\n", msg2);

		int input = 0;
		while(input != 1 && input != 2) {
			scanf("%d", &input);
		}
		sprintf(msg2, "%d", input);
		write(sockclient, msg2, sizeof(msg2));
	
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
	int i = 0, higher = 0;
	for(i = 0; i < nbHands; i++) {
		if(flush(croupier, hands[i])) {
			higher = 4;
			result = hands[i];
		}
		if(higher < 3 && threeOfKind(croupier, hands[i])) {
			higher = 3;
			result = hands[i];
		}
		if(higher < 2 && twoPair(croupier, hands[i])) {
			higher = 2;
			result = hands[i];
		}
		if(higher < 1 && pair(croupier, hands[i])) {
			higher = 1;
			result = hands[i];
		}
	}
	printf("\nHigher -> %d\n", higher);
	return result;
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
