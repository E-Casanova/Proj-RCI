#include "server.h"



int start_server(node_information * node_info){


    return -1;
}



int start_client_TCP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info){

    return -1;
}


int start_client_UDP(char addr[INET_ADDRSTRLEN], char port[6], node_information * node_info){

    // Start UDP Client

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) return -1;


    node_info->ns_fd = fd;

    return 1;
};
