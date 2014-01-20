/*
 * commands.h
 *
 *  Created on: Jan 20, 2014
 *      Author: ktaube
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_


#define UPDATE_TIMER 3

typedef struct Message {
    char *msg;
    struct Message *next;
} Message_t;

extern Message_t *OUT_QUEUE;

int start_game(int socket, char *username);
char *get_next_message();
void get_username(char *username);
void *receive_user_input();
void create_message(char *msg);
char *get_in_message();
void *planet_update_timer();
int create_threads();
void parse_resp(char *message);

#endif /* COMMANDS_H_ */
