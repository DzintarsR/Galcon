
#include "commands.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

pthread_mutex_t QUEUE_MUTEX = PTHREAD_MUTEX_INITIALIZER;

int create_threads() {
    pthread_t planet_updater_thread;
    pthread_t user_input_thread;

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

void *receive_user_input() {
    char buf[100];
    while (1) {
        printf("Galcom> ");
        scanf("%s", buf);

        if (strlen(buf) == 0) {
            printf("ERROR: please provide command");
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
    int x, y;

    if (sprintf(buf, "J %d %d", &x, &y)) {
        // TODO: create user with uid y
        // TODO: set timeout till game starts
    } else if (strcmp(message[0], "A")) {
        // TODO: populate list with attackers
        // TODO: implement thread that apply attack after provided time??? or this is done by updater thread???
    } else if (strcmp(message[0], "U")) {
        // TODO: at the start of the game populate users. I think that one ask at the start would be sufficient cause data shouldn't change
    } else if (strcmp(message[0], "M")) {
        // TODO: Populate planet list at the start of the game
        // TODO: Update planet information about planets
    } else if (sprintf(buf, "S %d %d", &x, &y)) {
        // TODO: Add attack in list
        // TODO: Updater should count down time for attack engagement
    } else {
        // TODO: do nothing?
    }
}

