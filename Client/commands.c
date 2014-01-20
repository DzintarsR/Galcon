
#include "commands.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

pthread_mutex_t QUEUE_MUTEX = PTHREAD_MUTEX_INITIALIZER;
Message_t *OUT_QUEUE = NULL;


/*****************************
 * Game Data
 */

User_t *USER_LIST = NULL;
User_t *CLIENT_USER = NULL;

Attack_t *ATTACS = NULL;

/*
 *
 ***************************/

int create_threads() {
    pthread_t planet_updater_thread;
    pthread_t user_input_thread;

    if (pthread_create(&planet_updater_thread, NULL, &planet_update_timer,
    NULL)) {
        printf("[ERROR]: Could not create thread!\n");
        return 1;
    }

    if (pthread_create(&user_input_thread, NULL, &receive_user_input,
    NULL)) {
        printf("[ERROR]: Could not create thread!\n");
        return 1;
    }

    return 0;
}


static User_t *create_user(char* username, int uid) {
	if (!username && !uid) {
		return NULL;
	}

    User_t *user = malloc(sizeof(User_t));

    strcpy(user->username, username);
    user->id = uid;
    user->next = NULL;

    return user;
}


int start_game(int socket, char* username) {
    char buf[255];
    char recv_buf[255];
    int start_time, uid;

    sprintf(buf, "J %s", username);
    if (send(socket, buf, strlen(buf), 0) < 0) {
        printf("[ERROR]: Send failed.\n");
        return -1;
    }

    if (recv(socket, recv_buf, 255, 0) < 0) {
        printf("[ERROR]: Recv failed.\n");
        return -1;
    }

    if (sscanf(recv_buf, "J %d %d", &uid, &start_time) == 0) {
        printf("[ERROR]: Could not join. Join operation failed.\n");
        return -1;
    }

    CLIENT_USER = create_user(username, uid);

    if (CLIENT_USER == NULL) {
    	printf("[ERROR]: Could not join. Server returned uid == 0.\n");
    	return -1;
    }

    // Sleep till start of game
    sleep(start_time);
    printf("[NOTICE]: GAME STARTED!\n");
    return 0;
}

void *planet_update_timer() {
    while (1) {
        create_message("M");
        sleep(UPDATE_TIMER);
    }
    return 0;
}

void *receive_user_input() {
    char buf[100];
    while (1) {
        printf("Galcom> ");
        scanf("%s", buf);

        if (strlen(buf) == 0) {
            printf("[ERROR]: please provide command");
            break;
        }
        create_message(buf);
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

void get_username(char *username) {
    while (1) {
        printf("Enter Username: ");
        scanf("%s", username);

        if (strlen(username) > 0)
            break;
    }
}


void parse_resp(char *message) {
    char buf[1000];
    int x = 0, y = 0;

    if (strcmp(&message[0], "A")) {
        // TODO: populate list with attackers
        // TODO: implement thread that apply attack after provided time??? or this is done by updater thread???
    } else if (strcmp(&message[0], "U")) {
        // TODO: at the start of the game populate users. I think that one ask at the start would be sufficient cause data shouldn't change
    } else if (strcmp(&message[0], "M")) {
        // TODO: Populate planet list at the start of the game
        // TODO: Update planet information about planets
    } else if (sprintf(buf, "S %d %d", x, y)) {
        // TODO: Add attack in list
        // TODO: Updater should count down time for attack engagement
    } else {
        // TODO: do nothing?
    }
}

