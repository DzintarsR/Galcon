#include <arpa/inet.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "commands.h"


int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	char *input_buf;
	char recv_buf[2000];
	char username[200];

	get_username(username);

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("[ERROR]: Could not create socket.\n");
	}

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(8888);
	server.sin_family = AF_INET;

	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("[ERROR]: Connect failed.\n");
		return 1;
	}

	if (!start_game(sock, username)) {
		printf("[ERROR]: Could not join the server!\n");
		return 1;
	}

	if (create_threads()) {
	    printf("[ERROR]: Could not create threads!\n");
	    return 1;
	}

	// Main loop
	while (1) {

	    // Wait for input
		if (OUT_QUEUE) {

		    // fetch message from queue and send
			input_buf = get_next_message();
			if (send(sock, input_buf, strlen(input_buf), 0) < 0) {
				printf("[ERROR]: Send failed!\n");
				return 1;
			}
			free(input_buf);

			// receive response
			if (recv(sock, recv_buf, 2000, 0) < 0) {
				printf("[ERROR]: recv failed!\n");
				break;
			}
			printf("\n%s\n", recv_buf);

			// parse response and apply changes
			parse_resp(recv_buf);
		}
	}

	close(sock);
	return 0;
}


