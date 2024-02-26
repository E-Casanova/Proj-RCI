#include "user_commands.h"
#include "server_client.h"
#include "utility.h"


int execute_user_command(node_information *node_info){

    char buffer[128];
    char * out = fgets(buffer, sizeof(buffer), stdin); 
    int errcode;

    if(buffer != out) return -1;


    //JOIN COMMAND
    if(strncmp("join ", buffer, 5) == 0 || strncmp("j ", buffer, 2) == 0
        || strncmp("join\n", buffer, 5) == 0 || strncmp("j\n", buffer, 2) == 0)
    {

        char ring_id[3];
        char node_id[2];
        int status;
        char cmd[64];

        status = sscanf(buffer, "%s %s %s", cmd, ring_id, node_id);

        if(status != 3 || (atoi(ring_id) < 0 || atoi(ring_id) > 999) || (atoi(node_id) < 0 || atoi(node_id) > 99)){
            printf("Incorrect use of command join: join ring[0-999] id[0-99]\n");
            return SUCCESS;
        }

        /*int ring_id_length = strlen(ring_id);  //bro this makes no sense
        int node_id_length = strlen(node_id);

        printf("%d\n", ring_id_length);
        printf("%d\n", node_id_length);
        */
        if (strnlen_2(ring_id, 3) != 3 || strnlen_2(node_id, 2) != 2) {
            printf("\x1b[33mInvalid: ring_id must have exactly 3 characters and node_id must have exactly 2 characters.\x1b[0m\n");
            return SUCCESS;
        }

        errcode = join(node_info, ring_id, node_id);

        return errcode;

    }

    if(strncmp("direct join ", buffer, 12) == 0 || strncmp("dj ", buffer, 3) == 0
        || strncmp("direct join\n", buffer, 5) == 0 || strncmp("dj\n", buffer, 2) == 0)
    {    

        int node_id = -1;
        int succ_id = -1;

        char succ_ip[INET_ADDRSTRLEN] = "\0";
        char succ_port[6] = "\0";

        int status;
        char cmd[64];

        status = sscanf(buffer, "%s %d %d %s %s", cmd, &node_id, &succ_id, succ_ip, succ_port);

        if(status != 5 || (node_id < 0 || node_id > 99) || (succ_id < 0 || succ_id > 99) || (isip(succ_ip) != 1 || (isport(succ_port) != 1))){
            printf("Incorrect use of command direct join: direct join id[0-99] succid[0-99] succIP[ipv4] succTCP[0-65535]\n");
            return 1;
        }


        return SUCCESS;

    }

    //EXIT COMMAND
    if(strncmp("exit\n", buffer, 5) == 0 || strncmp("x\n", buffer, 2) == 0
        || strncmp("exit ", buffer, 5) == 0 || strncmp("x ", buffer, 2) == 0)
    {
        return 10;
    }



    return UNKNOWN_COMMAND;
}



int join(node_information * node_info, char ring_id[3], char node_id[2]){

    char buffer_out[32] , buffer_in[128];
    struct sockaddr_in addr;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    ssize_t n;

    ssize_t node_id_int;
    ssize_t ids[100];
    int n_nodes = 0; // how many nodes in ring

    if(node_id[0] < '0' || node_id[0] > '9' || node_id[1] < '0' || node_id[1] > '9') {
        printf("Invalid node id...\n");
        return E_NON_FATAL;
    }

    node_id_int = atoi(node_id);

    n = start_client_UDP(node_info->ns_ipaddr, node_info->ns_port, node_info);
    if(n != 1) return E_FATAL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int errcode = getaddrinfo(node_info->ns_ipaddr, node_info->ns_port, &hints, &res);
    if(errcode != 0){
        printf("Error: could not get node server address info from %s\n", node_info->ns_ipaddr);
        fflush(stdout);
        exit(EXIT_FAILURE);
    } 



    // The command to get the list of nodes
    sprintf(buffer_out, "NODES %s", ring_id);

    n = sendto(node_info->ns_fd, buffer_out, sizeof(buffer_out), 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        printf("Could not sendto %s:%s", node_info->ns_ipaddr, node_info->ns_port);
        fflush(stdout);
        return E_FATAL;
    }

    addrlen = sizeof(addr);
    n = recvfrom(node_info->ns_fd, buffer_in, sizeof(buffer_in), 0, (struct sockaddr*)&addr, &addrlen);
    if(n == -1){

        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("\x1b[33mNode Server timed out...\x1b[0m\n");
            return E_NON_FATAL;
        } else {
            return E_FATAL;
        }
        
    }
   
    printf("%s\n", buffer_in);

    char * token;

    token = strtok(buffer_in, "\n");

    token = strtok(NULL, "\n"); // Skip over NODESLIST xxx

    while (token != NULL)
    {

        n_nodes++;

        sscanf(token, "%lu", &n);

        ids[n_nodes-1] = n;

        token = strtok(NULL, "\n");
    }
    
    if(n_nodes == 0) {

        printf("No nodes in ring, joining with id %s\n", node_id);

        sprintf(buffer_out, "REG %s %s %s %s", ring_id, node_id, node_info->ipaddr, node_info->port);
        sprintf(buffer_in, " ");

        n = sendto(node_info->ns_fd, buffer_out, sizeof(buffer_out), 0, res->ai_addr, res->ai_addrlen);

        if(n == -1) {

            printf("Could not sendto %s:%s", node_info->ns_ipaddr, node_info->ns_port);
            fflush(stdout);
            return E_FATAL;
        }

        n = recvfrom(node_info->ns_fd, buffer_in, sizeof(buffer_in), 0, (struct sockaddr*)&addr, &addrlen);
        if(n == -1){

            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("\x1b[33mNode Server timed out...\x1b[0m\n");
                return E_NON_FATAL;
            } else {
                return E_FATAL;
            }
        }

        //printf("%s\n", buffer_in);

        if(strncmp(buffer_in, "OKREG", 5) != 0){
            printf("Invalid register...\n");
            return E_NON_FATAL;
        }

    

        node_info->succ_id = (unsigned int)node_id_int;

        return SUCCESS;
    }

    for(int j = 0; j < n_nodes; j++){
        if(ids[j] == node_id_int){
            printf("ID already taken...\n");
            node_id_int = 1;
            break;
        }
    }

    for(int j = 0; j < n_nodes; j++){

        if(node_id_int > 99) {
            printf("Ring is full...\n");
            return E_NON_FATAL;
        }

        if(ids[j] == node_id_int){
            node_id_int++;
            j = 0;
        }
    }

    node_info->id = (unsigned int)node_id_int;

    printf("Joining node %s with id %lu\n", ring_id, node_id_int);


    
    return E_NON_FATAL;

}

int direct_join(node_information * node_info, int node_id, int succ_id, char succ_ip[INET_ADDRSTRLEN], char succ_tcp[6]){

    //Start TCP server

    if(start_server_TCP(node_info) != 1){
        printf("\n\x1b[32m Error starting TCP server @ %s:%s", node_info->ipaddr, node_info->port);
    };

    if(start_client_successor(succ_ip, succ_tcp, node_info) == - 1) {
        printf("\n\x1b[32m Error starting TCP client @ %s:%s", succ_ip, succ_tcp);
    }
    

    return 1;
}