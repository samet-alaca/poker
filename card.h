typedef struct {
	int val;
	int col;
} Card;

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
