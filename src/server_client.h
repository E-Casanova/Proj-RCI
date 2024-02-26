#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H


#include "utility.h"

/*
*
* Starts a TCP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_server_TCP(node_information * node_info);


/*
*
* Tries to connect to TCP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_client_successor(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info);


/*
*
* Tries to connect to UDP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_client_UDP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info);



#endif