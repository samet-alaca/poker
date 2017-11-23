void client(char** argv) {
	char msg_cl1[100], msg_cl2[100], msg_cl3[100], msg_cl4[100],
		msg_cl5[100], msg_cl6[100], msg_cl7[100], msg_cl8[100];
	struct sockaddr_in coord_serveur;
	int sockclient;
	struct tm *m;
	char* pc;
	u_short port;
	int nb, input = 0;

	port = atoi(argv[2]);
	sockclient = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&coord_serveur, sizeof(coord_serveur));
	coord_serveur.sin_family = AF_INET;
	coord_serveur.sin_port = htons(port);
	coord_serveur.sin_addr.s_addr = inet_addr(argv[1]);
	if(connect(sockclient, (struct sockaddr *) &coord_serveur, sizeof(coord_serveur)) < 0) {
		printf ("Erreur connect\n");
		exit(EXIT_FAILURE);
	}

	read(sockclient, msg_cl1, sizeof(msg_cl1));
	printf("%s\n", msg_cl1);

	read(sockclient, msg_cl2, sizeof(msg_cl2));
	printf("%s\n", msg_cl2);

	while(input != 1 && input != 2) {
		scanf("%d", &input);
		if(input == 2) {
			close(sockclient);
			exit(EXIT_SUCCESS);
		}
	}
	sprintf(msg_cl3, "%d", input);
	write(sockclient, msg_cl3, sizeof(msg_cl3));

	read(sockclient, msg_cl4, sizeof(msg_cl4));
	printf("%s\n", msg_cl4);

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

	read(sockclient, msg_cl6, sizeof(msg_cl6));
	printf("%s\n", msg_cl6);

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

	read(sockclient, msg_cl8, sizeof(msg_cl8));
	printf("%s\n", msg_cl8);

	close(sockclient);
}
