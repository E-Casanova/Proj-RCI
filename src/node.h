#ifndef NODE_H
#define NODE_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


typedef struct succ_info {
    struct addrinfo *res;
} succ_info;

typedef struct pred_info {
    socklen_t addrlen;
    struct addrinfo *res;
    struct sockaddr_in addr;
} pred_info;


typedef struct node_information
{


    //NODE SERVER//////////
    
    //Node server ip address
    char ns_ipaddr[INET_ADDRSTRLEN];
    //Node server UDP Port
    char ns_port[6];
    //Node server's socket file descriptor
    int ns_fd; 
    /////////////////////

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
    //Predecessor's file descriptor
    int pred_fd;

    pred_info pred;
} node_information;

/*
*
* Initializes a node_information object
*
* @returns a node_information object
*/
node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6]);


#endif