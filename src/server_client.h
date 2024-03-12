#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H


#include "utility.h"

typedef enum whofrom {

    FROM_TEMP,
    FROM_SUCC,
    FROM_PRED,
    FROM_CHORD

} whofrom;


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
int start_client_TCP(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info);


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

int process_message_fromchord_in(node_information * node_info);

int process_message_fromchord_out(node_information * node_info);


int process_ENTRY(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

int process_ROUTE(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

int process_SUCC(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

int process_PRED(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

int process_CHAT(node_information * node_info, char buffer[CHAT_BUFFER_SIZE]);

int process_CHORD(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);



int announce_shortest_path(node_information * node_info, char path[BUFFER_SIZE], int start, int end);

int announce_shortest_path_neighbour(node_information * node_info, char path[BUFFER_SIZE], int start, int end, int fd);

void send_stp_table(node_information * node_info, int fd);

void clear_id_from_tables(node_information * node_info, int id);


#endif