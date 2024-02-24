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


    struct timeval DEFAULT_TIMEOUT =  { .tv_sec = 5, .tv_usec = 0};


    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&DEFAULT_TIMEOUT, sizeof(DEFAULT_TIMEOUT));

    node_info->ns_fd = fd;

    return 1;
};
