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


/*
*
* Accepts a new inbound connection 
* 
* @returns 1 if success
*/
int accept_inbound_connection(node_information * node_info);


int process_message_fromtemp(node_information * node_info);

int process_message_frompred(node_information * node_info);

int process_message_fromsucc(node_information * node_info);


int process_ENTRY(node_information * node_info, char buffer[BUFFER_SIZE], int whofrom);

int process_SUCC(node_information * node_info, char buffer[BUFFER_SIZE],int whofrom);

int process_PRED(node_information * node_info, char buffer[BUFFER_SIZE], int whofrom);


#endif