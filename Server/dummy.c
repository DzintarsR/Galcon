
#include "dummy.h"

char *join_game(char *username, char *user_ip) {
	return "J 1 15";
}

char *get_attack_data(char *user_ip) {
	return "A 2 1_50_3_2_45 2_100_4_2_15";
}

char *get_user_data() {
	return "U 2 1_karlis 2_janis";
}

char *get_map_data() {
	return "M 2 1_12_15_1_100_50 2_20_45_2_90_40";
}

char *create_attack(int planet_from_id, int planet_to_id, int ships, char *attacker_ip) {
	return "S 30 65";
}
