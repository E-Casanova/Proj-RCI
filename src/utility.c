#include "utility.h"




int isport(const char * str){

    if(strlen(str) > 5) return -1;

    for(int i = 0; i < strlen(str); i++){

        if(str[i] < '0' || str[i] > '9') return -1;

    }

    return 1;
}

int isip(const char * str){
    struct sockaddr_in s_addr;
    
    return inet_pton(AF_INET, str, &s_addr.sin_addr);

}


cor_interrupt wait_for_interrupt(node_information * node_info){


    int fdmax = 0;

    struct timeval TIMEOUT = { .tv_sec = 0, .tv_usec = 1000}; // 1ms

    //Initializing file descriptor set
    fd_set readfds;
    FD_ZERO(&readfds);

    FD_SET(STDIN_FILENO, &readfds);
    fdmax = STDIN_FILENO > fdmax ? STDIN_FILENO : fdmax;


    //Check if connectrd to node server
    if(node_info->ns_fd > 0) {
        FD_SET(node_info->ns_fd, &readfds);
        fdmax = node_info->ns_fd > fdmax ? node_info->ns_fd : fdmax;
    }


    //Add remaining file descriptors



    int n = select(fdmax+1, &readfds, NULL, NULL, &TIMEOUT);

    if(n < 0){
        printf("Select error \n");
        exit(1);
    }

    if(n > 0) {

        if(FD_ISSET(STDIN_FILENO, &readfds)) {
            FD_CLR(STDIN_FILENO, &readfds);
            return I_USER_COMMAND;
        } 
        else if (node_info->ns_fd != -1 && FD_ISSET(node_info->ns_fd, &readfds)) {
            // Message from node server
            return I_NODE_SERVER_MSG;
        }

    }

    return I_TIMEOUT;

}