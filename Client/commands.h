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

typedef struct User {
    int id;
    char username[50];
    struct User *next;
} User_t;

typedef struct Planet {
    int id;
    int x;
    int y;
    int capacity;
    int ships;

    struct User *user;
    struct Planet *next;
} Planet_t;

typedef struct Attack {
    int ships;
    int time_left;

    int to;
    int from;

    struct Attack *next;
} Attack_t;

typedef struct Chat {
    int user_to_id;
    int user_from_id;

    char chat_time[9];
	char chat_message[81];

    struct Chat *next;
} Chat_t;

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
