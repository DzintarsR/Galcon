#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define UPDATE_TIMER 3

typedef struct Message {
	char *msg;
	struct Message *next;
} Message_t;

int start_game(int socket, char *username);
char *get_next_message();
void get_username(char *username);
void *receive_user_input();
void create_message(char *msg);
char *get_in_message();
void *planet_update_timer();

Message_t *OUT_QUEUE = NULL;

pthread_mutex_t QUEUE_MUTEX = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	char *input_buf;
	char recv_buf[2000];
	char username[200];

	pthread_t planet_updater_thread;
	pthread_t user_input_thread;

	get_username(username);

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(8888);
	server.sin_family = AF_INET;

	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	if (!start_game(sock, username)) {
		perror("Could not join the server!\n");
		return 1;
	}

	if (pthread_create(&planet_updater_thread, NULL, &planet_update_timer,
	NULL)) {
		perror("could not create thread");
		return 1;
	}

	if (pthread_create(&user_input_thread, NULL, &receive_user_input,
	NULL)) {
		perror("could not create thread");
		return 1;
	}

	// Main loop
	while (1) {
		if (OUT_QUEUE) {
			input_buf = get_next_message();
			if (send(sock, input_buf, strlen(input_buf), 0) < 0) {
				perror("send failed");
				return 1;
			}
			free(input_buf);

			if (recv(sock, recv_buf, 2000, 0) < 0) {
				perror("recv failed");
				break;
			}
		}
	}

	close(sock);
	return 0;
}

int start_game(int socket, char* username) {
	char buf[255];
	char recv_buf[255];
	char start_time[10];

	sprintf(buf, "J %s", username);
	if (send(socket, buf, strlen(buf), 0) < 0) {
		perror("send failed");
		return -1;
	}

	if (recv(socket, recv_buf, 2000, 0) < 0) {
		perror("recv failed");
		return -1;
	}

	if (sscanf(recv_buf, "J %s", start_time) == 0) {
		perror("Could not join. Join operation failed.");
		return -1;
	}
	return atol(start_time);
}

void *planet_update_timer() {
	while (1) {
		create_message("M");
		sleep(UPDATE_TIMER);
	}
	return 0;
}

char *get_next_message() {
	Message_t *msg;
	char *m;

	pthread_mutex_lock(&QUEUE_MUTEX);
	msg = OUT_QUEUE;
	OUT_QUEUE = OUT_QUEUE->next;
	pthread_mutex_unlock(&QUEUE_MUTEX);

	m = msg->msg;
	free(msg);
	return m;
}

void create_message(char *msg) {
	Message_t *tmp = OUT_QUEUE;
	Message_t *message = malloc(sizeof(Message_t));
	message->msg = malloc(sizeof(char) * strlen(msg));
	strcpy(message->msg, msg);
	message->next = NULL;

	pthread_mutex_lock(&QUEUE_MUTEX);
	if (tmp == NULL) {
		OUT_QUEUE = message;
	} else {
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = message;
	}
	pthread_mutex_unlock(&QUEUE_MUTEX);
}

void *receive_user_input() {
	char buf[100];
	while (1) {
		printf("Galcom> ");
		scanf("%s", buf);

		create_message(buf);
	}
	return 0;
}

void get_username(char *username) {
	printf("Enter Username: ");
	scanf("%s", username);
}

