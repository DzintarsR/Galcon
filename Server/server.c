/*
 * server.c
 *
 *  Created on: Jan 18, 2014
 *      Author: ktaube
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "dummy.h"


char *parse_request(int sock, char *message);
void *connection_handler(void *);

int main(int argc, char *argv[]) {
	int socket_desc, client_sock, c, *new_sock;
	struct sockaddr_in server, client;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("bind failed. Error");
		return 1;
	}

	listen(socket_desc, 3);

	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	// Create new thread for each client connection
	while ((client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t*) &c))) {
		puts("Connection accepted");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;

		if (pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
			perror("could not create thread");
			break;
		}
	}

	if (client_sock < 0) {
		perror("accept failed");
		return 1;
	}

	return 0;
}

/*
 * Client connection handler.
 * */
void *connection_handler(void *socket_desc) {
	int sock = *(int*) socket_desc;
	int read_size;
	char client_message[2000];
	char *server_message;

	// read and answer requests
	while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		printf("<--: %s\n", client_message);
		server_message = parse_request(sock, client_message);

		if (server_message == NULL) {
			printf("Client message could not be parsed! msg=%s\n", client_message);
			break;
		}

		write(sock, server_message, strlen(server_message));
		printf("-->: %s\n", server_message);
	}

	if (read_size == 0) {
		printf("Client disconnected\n");
	} else if (read_size == -1) {
		perror("recv faile\n");
	}

	free(socket_desc);
	return 0;
}

char *parse_request(int sock, char *message) {
	struct sockaddr_in client;
	int addr_size = sizeof(client);
	getsockname(sock, (struct sockaddr*)&client, (socklen_t *)&addr_size);

	char username[125];
	int planet_from_id, planet_to_id, ships;

	switch (message[0]) {
		case 'J':
			if (sscanf(message, "J %s", username))
				return join_game(username, inet_ntoa(client.sin_addr));
			break;
		case 'M':
			return get_map_data();
			break;
		case 'S':
			if (sscanf(message, "S %d %d %d", &planet_from_id, &planet_to_id, &ships))
				return create_attack(planet_from_id, planet_to_id, ships, inet_ntoa(client.sin_addr));
			break;
		case 'U':
			return get_user_data();
			break;
		case 'A':
			return get_attack_data(inet_ntoa(client.sin_addr));
			break;
		default:
			return NULL;

	}
	return NULL;
}








