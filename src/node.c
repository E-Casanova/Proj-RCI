#include "node.h"


node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6])
{

    node_information * tmp = (node_information*)calloc(1, sizeof(node_information));

    if(tmp == NULL){
        exit(1);
    }

    tmp->server_fd = -1;

    tmp->ss_id = -1;
    memset(tmp->ss_ip, 0, sizeof(tmp->ss_ip));
    memset(tmp->ss_port, 0, sizeof(tmp->ss_port));

    tmp->id = -1;
    strcpy(tmp->ipaddr, ippadr);
    strcpy(tmp->port, port);

    tmp->succ_id = -1;
    tmp->succ_fd = -1;
    memset(tmp->succ_ip, 0, sizeof(tmp->succ_ip));
    memset(tmp->succ_port, 0, sizeof(tmp->succ_port));

    tmp->pred_id = -1;
    tmp->pred_fd = -1;
    memset(tmp->pred_ip, 0, sizeof(tmp->pred_ip));
    memset(tmp->pred_port, 0, sizeof(tmp->pred_port));

    tmp->ns_fd = -1;
    strcpy(tmp->ns_ipaddr, ns_ipaddr);
    strcpy(tmp->ns_port, ns_port);

    tmp->temp_fd = -1;
    memset(tmp->temp_ip, 0, sizeof(tmp->temp_ip));
    memset(tmp->temp_port, 0, sizeof(tmp->temp_port));

    tmp->chord_head = (chord_information*)malloc(sizeof(chord_information));

    tmp->chord_head->next = NULL;
    tmp->chord_head->chord_fd = -1;

    return tmp;
}

void free_app_node(node_information * node_info){


    if(node_info->server_fd != -1) close(node_info->server_fd);
    close(node_info->pred_fd);
    close(node_info->succ_fd);
    close(node_info->temp_fd);

    chord_information * tmp  = node_info->chord_head;
    chord_information * tmp2 = tmp ;

    while (tmp != NULL)
    {
        tmp2 = tmp;

        tmp = tmp->next;

        if(tmp2->chord_fd > 0) close(tmp2->chord_fd);
        free(tmp2);

    }
    

    return;
}