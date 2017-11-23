typedef struct
{
	int id;
    int address;
    int budget;
	Card* hand;
} Player;

Player newPlayer(Player* players, int nbPlayers) {
	Player* tmp = (Player*) realloc(players, (nbPlayers + 1) * sizeof(Player));
	Player p;
	tmp[nbPlayers] = p;
	return p;
}
