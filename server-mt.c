#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define MAX_CLIENTES 10

int firstConnection = 1;

struct client_data *clientes[MAX_CLIENTES];
int num_clientes = 0;
pthread_mutex_t clientes_mutex = PTHREAD_MUTEX_INITIALIZER;
struct client_data {
    int csock;
    struct sockaddr_storage storage;
};

void enviar_start_para_todos(int segundos) {
    pthread_mutex_lock(&clientes_mutex);

    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i] == NULL){
            continue;
        }
        aviator_msg msg;
        msg.player_id = clientes[i]->csock; // ou outro id único se preferir
        msg.value = (float)segundos;
        strncpy(msg.type, "start", STR_LEN);
        msg.player_profit = 0.0;
        msg.house_profit = 0.0;

        if (send(clientes[i]->csock, &msg, sizeof(msg), 0) != sizeof(msg)) {
            perror("Erro ao enviar start");
        } else {
            printf("[log] Enviado START para cliente %d\n", i);
        }
    }

    pthread_mutex_unlock(&clientes_mutex);
}

void iniciar_contagem_regressiva(int segundos) {
    for (int i = segundos; i > 0; i--) {
        printf("Contagem regressiva: %d segundos restantes...\n", i);
        sleep(1);
    }
}

void rodadaLoop(){
    while(1){
        enviar_start_para_todos(10); // Envia mensagem de início para todos os clientes
        iniciar_contagem_regressiva(10); // Inicia a contagem regressiva de 5 segundos
    }
}


#define BUFSZ 1024

aviator_msg msg;

void recieveMsgAsServer() {
	recv(0, &msg, sizeof(msg), 0);
	if (strcmp(msg.type, "bet") == 0) {

	} else if (strcmp(msg.type, "cashout") == 0) {

	} else if (strcmp(msg.type, "bye") == 0) {

	}
}

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);

    // Exemplo de resposta do servidor
    aviator_msg resposta = {
        .player_id = msg.player_id,
        .value = 2,
        .player_profit = 3,
        .house_profit = 1,
    };
    strncpy(resposta.type, "start", STR_LEN);

    send(cdata->csock, &resposta, sizeof(resposta), 0);

    close(cdata->csock);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata) {
            logexit("malloc");
        }
        cdata->csock = csock;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
        printf("teste");
    }
    exit(EXIT_SUCCESS);
}
