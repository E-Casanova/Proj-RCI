#include "node.h"


node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6])
{

    node_information * tmp = (node_information*)calloc(1, sizeof(node_information));

    if(tmp == NULL){
        exit(1);
    }

    tmp->server_fd = -1;

    tmp->id = -1;
    strcpy(tmp->ipaddr, ippadr);
    strcpy(tmp->port, port);

    tmp->succ_id = 0;
    tmp->succ_fd = -1;
    memset(tmp->succ_ip, 0, sizeof(tmp->succ_ip));
    memset(tmp->succ_port, 0, sizeof(tmp->succ_port));

    tmp->pred_id = 0;
    tmp->pred_fd = -1;
    memset(tmp->pred_ip, 0, sizeof(tmp->pred_ip));
    memset(tmp->pred_port, 0, sizeof(tmp->pred_port));

    tmp->ns_fd = -1;
    strcpy(tmp->ns_ipaddr, ns_ipaddr);
    strcpy(tmp->ns_port, ns_port);


    tmp->n_chords = 0;
    for(int i = 0; i < 100; i++){
        tmp->chord_fds[i] = -1;
        memset(tmp->chord_ips[i], 0, sizeof(tmp->chord_ips[i]));
        memset(tmp->chord_ports[i], 0, sizeof(tmp->chord_ports[i]));
    }


    return tmp;
}