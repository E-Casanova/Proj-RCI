#include "node.h"


node_information* init_node(char ippadr[INET_ADDRSTRLEN], char port[6], int id, char ns_ipaddr[INET_ADDRSTRLEN], char ns_port[6])
{

    node_information * tmp = (node_information*)calloc(1, sizeof(node_information));

    if(tmp == NULL){
        exit(1);
    }

    tmp->id = -1;
    strcpy(tmp->ipaddr, ippadr);
    strcpy(tmp->port, port);

    tmp->succ_id = 0;
    memset(tmp->succ_ip, 0, sizeof(tmp->succ_ip));
    memset(tmp->succ_port, 0, sizeof(tmp->succ_port));

    tmp->pred_id = 0;
    memset(tmp->pred_ip, 0, sizeof(tmp->pred_ip));
    memset(tmp->pred_port, 0, sizeof(tmp->pred_port));

    tmp->ns_fd = -1;
    strcpy(tmp->ns_ipaddr, ns_ipaddr);
    strcpy(tmp->ns_port, ns_port);

    return tmp;
}