/*
 * summy.h
 *
 *  Created on: Jan 19, 2014
 *      Author: ktaube
 */

#ifndef SUMMY_H_
#define SUMMY_H_

char *join_game(char *username, char *user_ip);
char *get_attack_data(char *user_ip);
char *get_user_data();
char *get_map_data();
char *create_attack(int planet_from_id, int planet_to_id, int ships, char *attacker_ip);

#endif /* SUMMY_H_ */
