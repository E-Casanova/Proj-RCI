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

    int active;
    int chord_id;
    int chord_fd;
    char chord_ip[INET_ADDRSTRLEN];
    char chord_port[6];
    struct chord_information * next;

} chord_information;


typedef struct node_information
{

    //FORWARDING

    char   fwd_table[100][100][100]; // fwd_table[dest][neighbour]
    char   stp_table[100][100]; // stp_table[dest]
    int    exp_table[100]; // exp_table[dest]

    //NODE SERVER//////////
    
    //Node server ip address
    char ns_ipaddr[INET_ADDRSTRLEN];
    //Node server UDP Port
    char ns_port[6];
    //Node server's socket file descriptor
    int ns_fd; 
    /////////////////////

    //TCP server's file descriptor
    int server_fd;
    // Node id
    unsigned int id;
    //Node IP
    char ipaddr[INET_ADDRSTRLEN];
    //Node's TCP server port
    char port[6];
    //Successor id
    unsigned int succ_id;
    //Successor IP address
    char succ_ip[INET_ADDRSTRLEN];
    //Successor TCP port
    char succ_port[6];
    //Successor's file descriptor
    int succ_fd;
    //Successor's address information
    succ_info succ;
    //Predecessor id
    unsigned int pred_id;
    //Predecessor IP address
    char pred_ip[INET_ADDRSTRLEN];
    //Predecessor TCP Port
    char pred_port[6];
    
    int pred_fd;//Predecessor's file descriptor
    
    pred_info pred;    //Predecessor's address information


    int  ss_id;    //Second successor's id

    char ss_ip[INET_ADDRSTRLEN];    //Second successor's ip

    
    char ss_port[6];//Second sucessor's port

    int  temp_fd;
    char temp_ip[INET_ADDRSTRLEN];
    char temp_port[6];

    //////CHORDS//////////

    
    chord_information * chord_head; // Chords connected to me

    int  chord_fd;
    int  chord_id;    //Chord im connected to

    char chord_ip[INET_ADDRSTRLEN];
    char chord_port[6];

    /////////////////////


    /////AUXILLIARIES////

    char id_str[3];
    char ring_id_str[4];

    //////////////////////

} node_information;


/*
*
* Initializes a node_information object
*
* @returns a node_information object
*/
node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6]);



void free_app_node(node_information * node_info);


#endif