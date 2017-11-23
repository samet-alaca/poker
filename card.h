typedef struct {
	int val;
	int col;
} Card;

Card* merge(Card* a, Card* b);
bool pair(Card* croupier, Card* hand);
bool twoPair(Card* croupier, Card* hand);
bool threeOfKind(Card* croupier, Card* hand);
bool flush(Card* croupier, Card* hand);
Card* higherHand(Card* croupier, Card** hands, int nbHands);
