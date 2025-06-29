#include "client.h"

aviator_msg msg;
int s;
const char *nickname;

void recieveMsgAsClient() {
	recv(s, &msg, sizeof(msg), 0);
	if (strcmp(msg.type, "start")) {
		printf("Recebido: type=%s | value=%.2f | profit=%.2f\n", msg.type, msg.value, msg.player_profit);
	} else if (strcmp(msg.type, "closed")) {

	} else if (strcmp(msg.type, "multiplier")) {

	} else if (strcmp(msg.type, "explode")) {

	} else if (strcmp(msg.type, "payout")) {

	} else if (strcmp(msg.type, "profit")) {

	} else if (strcmp(msg.type, "bye")) {

	}
}

void sendMsgAsClient() {
	if (strcmp(msg.type, "bet")) {

	} else if (strcmp(msg.type, "cashout")) {

	} else if (strcmp(msg.type, "bye")) {

	}
	send(s, &msg, sizeof(msg), 0);
} 


void usage(int argc, char **argv) { // Modificar pra cobrar o apelido do jogador
	printf("usage: %s <server IP> <server port> -nick <nickname>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511 -nick Flip\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) { 
	if (argc != 5) {
		printf("Error: Invalid number of arguments");
		usage(argc, argv);
	}

	if (strcmp(argv[3], "-nick") != 0) {
		fprintf(stderr, "Error: Expected '-nick' argument\n");
		usage(argc, argv);
	}

	nickname = argv[4];
	if (strlen(nickname) > 13) {
		fprintf(stderr, "Error: Nickname too long (max 13)\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}
	printf("Conectado ao servidor.\n");

	recieveMsgAsClient();

	close(s);
	exit(EXIT_SUCCESS);
}