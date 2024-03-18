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

size_t strnlen_2(const char * s, size_t maxlen){

    int result = 0;

    for(int i = 0; i < maxlen; i++){

        if(s[i] == '\0') return result;
        result++;

    }

    return result;

}



cor_interrupt wait_for_interrupt(node_information * node_info){


    int fdmax = 0;

    struct timeval TIMEOUT = { .tv_sec = 0, .tv_usec = 5000}; // 1ms

    //Initializing file descriptor set
    fd_set readfds;
    FD_ZERO(&readfds);

    FD_SET(STDIN_FILENO, &readfds);
    fdmax = STDIN_FILENO > fdmax ? STDIN_FILENO : fdmax;

    if(node_info->ns_fd > 0) {
        FD_SET(node_info->ns_fd, &readfds);
        fdmax = node_info->ns_fd > fdmax ? node_info->ns_fd : fdmax;
    }
    if(node_info->succ_fd > 0) {
        FD_SET(node_info->succ_fd, &readfds);
        fdmax = node_info->succ_fd > fdmax ? node_info->succ_fd : fdmax;
    }
    if(node_info->pred_fd > 0) {
        FD_SET(node_info->pred_fd, &readfds);
        fdmax = node_info->pred_fd > fdmax ? node_info->pred_fd : fdmax;
    }
    if(node_info->server_fd > 0) {
        FD_SET(node_info->server_fd, &readfds);
        fdmax = node_info->server_fd > fdmax ? node_info->server_fd : fdmax;
    }
    if(node_info->temp_fd > 0) {
        FD_SET(node_info->temp_fd, &readfds);
        fdmax = node_info->temp_fd > fdmax ? node_info->temp_fd : fdmax;
    }
    if(node_info->chord_fd > 0) {
        FD_SET(node_info->chord_fd, &readfds);
        fdmax = node_info->chord_fd > fdmax ? node_info->chord_fd : fdmax;
    }

    //ADICIONAR CORDAS

    chord_information * tmp = node_info->chord_head;

    while (tmp != NULL)
    {
        if(tmp->chord_fd > 0) {
            FD_SET(tmp->chord_fd, &readfds);
            fdmax = tmp->chord_fd > fdmax ? tmp->chord_fd : fdmax;
        }

        tmp = tmp->next;

    }
    


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

        if (node_info->ns_fd != -1 && FD_ISSET(node_info->ns_fd, &readfds)) {
            //Message from node server
            return I_NODE_SERVER_MSG;
        }
        if (node_info->succ_fd != -1 && FD_ISSET(node_info->succ_fd, &readfds)) {
            //Message from successor
            return I_MESSAGE_SUCCESSOR;
        }
        if (node_info->pred_fd != -1 && FD_ISSET(node_info->pred_fd, &readfds)) {
            //Message from predecessor
            return I_MESSAGE_PREDECESSOR;
        }
        if (node_info->temp_fd != -1 && FD_ISSET(node_info->temp_fd, &readfds)) {
            //Message from someone i dont know (yet)
            return I_MESSAGE_TEMP;
        }
        if (node_info->server_fd != -1 && FD_ISSET(node_info->server_fd, &readfds)) {
            //Someone is trying to connect
            return I_NEW_CONNECTION;
        }

        if (node_info->chord_fd != -1 && FD_ISSET(node_info->chord_fd, &readfds)) {
            //Chord is trying to speak
            return I_MESSAGE_CHORD_OUT;
        }


        tmp = node_info->chord_head;

        while (tmp != NULL)
        {
            if(tmp->chord_fd != -1 && FD_ISSET(tmp->chord_fd, &readfds)) {
                //Chord is trying to speak

                tmp->active = 1; // this means this one is the chord trying to speak, usefull when we have many connected to us

                return I_MESSAGE_CHORD_IN;
            }

            tmp = tmp->next;

        }

    }

    return I_TIMEOUT;

}


void idtostr(int id, char str[2]){

    str[0] ='0' + (id / 10);
    str[1] ='0' + (id % 10);

}

void strtoid(int id, char str[2]){
    id = (str[0] - '0') * 10 + str[1] - '0';
}