#ifndef SERVER_H
#define SERVER_H


#include "utility.h"

/*
*
* Starts a TCP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_server(node_information * node_info);


/*
*
* Tries to connect to TCP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_client_TCP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info);


/*
*
* Tries to connect to UDP server and saves file descriptor in the node_information structure
*
* @returns 1 if success
*
*/
int start_client_UDP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info);



#endif