#ifndef USER_COMMANDS_H
#define USER_COMMANDS_H

#include "utility.h"

/*
*
* Reads and executes the user commands from the terminal
*
* @returns 1 if success, -2 if the command is unknown and 10 if the exit command is selected
*
*/
int execute_user_command(node_information *node_info);

/*
*
* Executes the join command
*
* @returns 1 if success 
*/
int join(node_information *node_info, char ring_id[3], char node_id[2]);

/*
*
* Executes the direct join command
* does not need to talk to the node server
* @returns 1 if success
*/
int direct_join(node_information * node_info, int node_id, int succ_id, char succ_ip[INET_ADDRSTRLEN], char succ_tcp[6]);

#endif