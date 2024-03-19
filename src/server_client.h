#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H


#include "utility.h"

typedef enum whofrom {
    FROM_TEMP,      // Message is from a temporary node
    FROM_SUCC,      // Message is from a successor node
    FROM_PRED,      // Message is from a predecessor node
    FROM_CHORD      // Message is from a Chord
} whofrom;


/*
*
* Starts a TCP server and saves file descriptor in the node_information structure
*
* @returns SUCCESS if success
*
*/
int start_server_TCP(node_information * node_info);


/*
*
* Tries to connect to TCP server and saves file descriptor in the node_information structure
*
* @returns SUCCESS if success
*
*/
int start_client_TCP(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info);


/*
*
* Tries to connect to UDP server and saves file descriptor in the node_information structure
*
* @returns SUCCESS if success
*
*/
int start_client_UDP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info);


/*
*
* Accepts a new inbound connection 
* 
* @returns SUCCESS_HIDDEN if success
*/
int accept_inbound_connection(node_information * node_info);

/*
 * Processes a message received from a temporary node
 * 
 * @param node_info Pointer to the node_information structure
 * @returns SUCCESS_HIDDEN if successful
 */
int process_message_fromtemp(node_information * node_info);

/*
 * Processes a message received from a predecessor node
 * 
 * @param node_info Pointer to the node_information structure
 * @returns SUCCESS_HIDDEN if successful
 */
int process_message_frompred(node_information * node_info);

/*
 * Processes a message received from a successor node
 * 
 * @param node_info Pointer to the node_information structure
 * @returns SUCCESS_HIDDEN if successful
 */
int process_message_fromsucc(node_information * node_info);

/*
 * Processes a message received from a chord thats connected to me
 * 
 * @param node_info Pointer to the node_information structure
 * @returns SUCCESS_HIDDEN if successful
 */
int process_message_fromchord_in(node_information * node_info);

/*
 * Processes a message received from a chord thats created by me
 * 
 * @param node_info Pointer to the node_information structure
 * @returns SUCCESS_HIDDEN if successful
 */
int process_message_fromchord_out(node_information * node_info);

/*
 * Processes an ENTRY message received from a specified source.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @param who Source of the message (FROM_TEMP, FROM_SUCC, FROM_PRED, FROM_CHORD)
 * @returns SUCCESS_HIDDEN if successful
 */
int process_ENTRY(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

/*
 * Processes a ROUTE message received from a specified source.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @param who Source of the message (FROM_TEMP, FROM_SUCC, FROM_PRED, FROM_CHORD)
 * @returns SUCCESS_HIDDEN if successful
 */
int process_ROUTE(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

/*
 * Processes a SUCC message received from a specified source.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @param who Source of the message (FROM_TEMP, FROM_SUCC, FROM_PRED, FROM_CHORD)
 * @returns SUCCESS_HIDDEN if successful
 */
int process_SUCC(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

/*
 * Processes a PRED message received from a specified source.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @param who Source of the message (FROM_TEMP, FROM_SUCC, FROM_PRED, FROM_CHORD)
 * @returns SUCCESS_HIDDEN if successful
 */
int process_PRED(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);

/*
 * Processes a CHAT message received.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @returns SUCCESS_HIDDEN if successful
 */
int process_CHAT(node_information * node_info, char buffer[CHAT_BUFFER_SIZE]);

/*
 * Processes a CHORD message received.
 * 
 * @param node_info Pointer to the node_information structure
 * @param buffer Buffer containing the message data
 * @param who Source of the message (FROM_TEMP, FROM_SUCC, FROM_PRED, FROM_CHORD)
 * @returns SUCCESS_HIDDEN if successful
 */
int process_CHORD(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who);




/*
 * Announces a specific shortest path to all neighbours.
 * 
 * @param node_info Pointer to the node_information structure
 * @param path Buffer containing the shortest path information
 * @param start Starting node of the path
 * @param end Ending node of the path
 * @returns SUCCESS_HIDDEN if successful
 */
int announce_shortest_path(node_information * node_info, char path[BUFFER_SIZE], int start, int end);

/*
 * Sends all entries on the shortest path table to someone.
 * 
 * @param node_info Pointer to the node_information structure
 * @param fd File descriptor to which the table is sent
 */
void send_stp_table(node_information * node_info, int fd);

/*
 * Clears the specified node ID from the forwarding, expedition and shortest path tables.
 * 
 * @param node_info Pointer to the node_information structure
 * @param id ID of the node to be cleared
 */
void clear_id_from_tables(node_information * node_info, int id);



#endif