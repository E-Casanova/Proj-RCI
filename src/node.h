#ifndef NODE_H
#define NODE_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct succ_info {
    struct addrinfo *res;
} succ_info;

typedef struct pred_info {
    struct addrinfo *res;
} pred_info;

typedef struct chord_information {

    int active;                              // Indicates whether the chord is the one trying to send me a message (used for select)
    int chord_id;                            // Chord ID
    int chord_fd;                            // Chord file descriptor
    char chord_ip[INET_ADDRSTRLEN];          // Chord IP address
    char chord_port[6];                      // Chord TCP port
    struct chord_information *next;          // Pointer to the next chord information structure

} chord_information;



typedef struct node_information
{
    char fwd_table[100][100][100];       // Forwarding table: fwd_table[dest][neighbour]
    char stp_table[100][100];            // Shortest path table: stp_table[dest]
    int exp_table[100];                  // Expedition table: exp_table[dest]

    char ns_ipaddr[INET_ADDRSTRLEN];     // Node server IP address
    char ns_port[6];                     // Node server UDP Port
    int ns_fd;                           // Node server's socket file descriptor


    int server_fd;                       // TCP server's file descriptor
    unsigned int id;                     // Node id
    char ipaddr[INET_ADDRSTRLEN];        // Node IP address
    char port[6];                        // Node's TCP server port


    unsigned int succ_id;                // Successor id
    char succ_ip[INET_ADDRSTRLEN];       // Successor IP address
    char succ_port[6];                   // Successor TCP port
    int succ_fd;                         // Successor's file descriptor
    succ_info succ;                      // Successor's address information


    unsigned int pred_id;                // Predecessor id
    char pred_ip[INET_ADDRSTRLEN];       // Predecessor IP address
    char pred_port[6];                   // Predecessor TCP Port
    int pred_fd;                         // Predecessor's file descriptor
    pred_info pred;                      // Predecessor's address information


    int ss_id;                           // Second successor's id
    char ss_ip[INET_ADDRSTRLEN];         // Second successor's IP address
    char ss_port[6];                     // Second successor's TCP port


    int temp_fd;                         // Temporary node's file descriptor
    char temp_ip[INET_ADDRSTRLEN];       // Temporary node's IP address
    char temp_port[6];                   // Temporary node's TCP port


    chord_information *chord_head;       // Chords connected to me


    int chord_fd;                        // Chord server file descriptor
    int chord_id;                        // Chord I'm connected to
    char chord_ip[INET_ADDRSTRLEN];      // Chord IP address
    char chord_port[6];                  // Chord TCP port


    char id_str[3];                      // String representation of node id
    char ring_id_str[4];                 // String representation of ring id

} node_information;



/*
*
* Initializes a node_information object
*
* @returns a node_information object
*/
node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6]);



/*
 * Frees memory associated with an application node.
 * 
 * @param node_info Pointer to the node_information structure
 */
void free_app_node(node_information * node_info);


#endif