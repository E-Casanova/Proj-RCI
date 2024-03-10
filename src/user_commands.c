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

    /*SHOW TOPOLOGY*/
    if(strncmp("show topology ", buffer, 14) == 0 || strncmp("st ", buffer, 3) == 0
        || strncmp("show topology\n", buffer, 14) == 0 || strncmp("st\n", buffer, 3) == 0)
        {
            
            char pred_id_str[3];
            char succ_id_str[3];
            char id_str[3];
            char ss_id_str[3];

            pred_id_str[2] = '\0'; succ_id_str[2] = '\0'; id_str[2] = '\0'; ss_id_str[2] = '\0';

            idtostr(node_info->id, id_str); idtostr(node_info->pred_id, pred_id_str); idtostr(node_info->succ_id, succ_id_str); idtostr(node_info->ss_id, ss_id_str);

            printf("\x1b[36m> ------------SHOW TOPOLOGY--------------\n> -- Node:          %s %s:%s --\n> -- Successor:     %s %s:%s --\n> -- Predecessor:   %s %s:%s --\n> -- 2nd Successor: %s %s:%s --\n> ---------------------------------------\x1b[0m\n",
                id_str, node_info->ipaddr, node_info->port , succ_id_str, node_info->succ_ip, node_info->succ_port,
                pred_id_str, node_info->pred_ip, node_info->pred_port, ss_id_str, node_info->ss_ip, node_info->ss_port);
            
            return SUCCESS;

        }

    if(strncmp("show routing ", buffer, 13) == 0 || strncmp("sr ", buffer, 3) == 0
        || strncmp("show routing\n", buffer, 13) == 0 || strncmp("sr\n", buffer, 3) == 0)
        {
            //encaminhamento em portugues é forwarding mas ok

            char cmd[32];
            int dest_id;

            int status = sscanf(buffer, "%s %d", cmd, &dest_id);
            if(status != 2 || (dest_id > 99) || (dest_id < 0))
            {
                printf("Incorrect use of command show routing: show routing dest[0-99]\n");
                return SUCCESS;
            }


            printf("\x1b[36m> ---- ROUTES TO NODE %d ---- \x1b[0m\n", dest_id);
            for(int i = 0; i < 100; i++){
                if(node_info->fwd_table[dest_id][i][0] != '\0') printf("\x1b[36m> %s \x1b[0m\n",node_info->fwd_table[dest_id][i]);
            }

            return SUCCESS;

        }
        
    if(strncmp("show path ", buffer, 10) == 0 || strncmp("sp ", buffer, 3) == 0
        || strncmp("show path\n", buffer, 13) == 0 || strncmp("sp\n", buffer, 3) == 0)
        {
            //encaminhamento em portugues é forwarding mas ok

            char cmd[32];
            int dest_id;

            int status = sscanf(buffer, "%s %d", cmd, &dest_id);
            if(status != 2 || (dest_id > 99) || (dest_id < 0))
            {
                printf("Incorrect use of command show path: show path dest[0-99]\n");
                return SUCCESS;
            }


            printf("\x1b[36m> ---- SHORTEST PATH TO NODE %d ---- \x1b[0m\n", dest_id);
            if(node_info->stp_table[dest_id][0] != '\0'){
                 printf("\x1b[36m> %s \x1b[0m\n",node_info->stp_table[dest_id]);
            } else {
                printf("\x1b[36m> NO PATH AVAILABE \x1b[0m\n");
            }


            return SUCCESS;

        }

    if(strncmp("show forwarding ", buffer, 16) == 0 || strncmp("sf ", buffer, 3) == 0
        || strncmp("show forwarding\n", buffer, 16) == 0 || strncmp("sf\n", buffer, 3) == 0)
        {
            
            printf("\x1b[36m> ---- EXPEDITION TABLE ---- \x1b[0m\n");

            for(int i = 0; i < 100; i++) {
                if(node_info->exp_table[i] > 0) printf("\x1b[36m> %d %d\x1b[0m\n", i, node_info->exp_table[i]);
            }

            return SUCCESS;

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

        if(node_info->succ_fd != -1) {
            printf("\x1b[33mNode already running on this app...\x1b[0m\n");
            return E_NON_FATAL;
        }

        errcode = direct_join(node_info, node_id, succ_id, succ_ip, succ_port);

        return errcode;

    }

    if(strncmp("leave\n", buffer, 6) == 0 || strncmp("l\n", buffer, 2) == 0
        || strncmp("leave ", buffer, 6) == 0 || strncmp("l ", buffer, 2) == 0)
    {


        //CLEAN THE ROUTING TABLES

        char buffer_out[BUFFER_SIZE], buffer_in[BUFFER_SIZE];
        struct addrinfo hints, *res;
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);


        int n = start_client_UDP(node_info->ns_ipaddr, node_info->ns_port, node_info);
        if(n != 1) return E_FATAL;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        int errcode = getaddrinfo(node_info->ns_ipaddr, node_info->ns_port, &hints, &res);
        if( errcode == -1) exit(1);



        sprintf(buffer_out, "UNREG %s %s", node_info->ring_id_str, node_info->id_str);

        n = sendto(node_info->ns_fd, buffer_out, sizeof(buffer_out), 0, res->ai_addr, res->ai_addrlen);
        if(n == -1) {
            printf("Could not sendto %s:%s", node_info->ns_ipaddr, node_info->ns_port);
            fflush(stdout);
            return E_FATAL;
        }

        n = recvfrom(node_info->ns_fd, buffer_in, sizeof(buffer_in), 0, (struct sockaddr*)&addr, &addrlen);

        if(strncmp(buffer_in, "OKUNREG", 7) != 0) {
            printf("\x1b[33m>Invalid UNREG response...\x1b[0m\n");
            return E_NON_FATAL;
        }

        memset(node_info->exp_table, 0, sizeof(node_info->exp_table));
        memset(node_info->stp_table, 0, sizeof(node_info->stp_table));
        memset(node_info->fwd_table, 0, sizeof(node_info->fwd_table));

        if(node_info->succ_fd != -1) close(node_info->succ_fd); //Close the succ connection if it exists

        node_info->succ_fd = -1;
        node_info->succ_id = -1;
        memset(node_info->succ_ip, 0, sizeof(node_info->succ_ip));
        memset(node_info->succ_port, 0, sizeof(node_info->succ_port));

        if(node_info->pred_fd != -1) close(node_info->pred_fd); //Close the pred connection if it exists

        node_info->pred_fd = -1;
        node_info->pred_id = -1;
        memset(node_info->pred_ip, 0, sizeof(node_info->pred_ip));
        memset(node_info->pred_port, 0, sizeof(node_info->pred_port));

        if(node_info->temp_fd != -1) close(node_info->temp_fd); //Close any temporary connections

        node_info->temp_fd = -1;
        memset(node_info->temp_ip, 0, sizeof(node_info->temp_ip));
        memset(node_info->temp_ip, 0, sizeof(node_info->temp_port));

        node_info->ss_id = -1;
        memset(node_info->ss_ip, 0, sizeof(node_info->ss_ip));
        memset(node_info->ss_port, 0, sizeof(node_info->ss_port));

        printf("\x1b[32m> Successfully left ring %s\x1b[0m\n", node_info->ring_id_str);

        memset(node_info->ring_id_str, 0, sizeof(node_info->ring_id_str));
        memset(node_info->id_str, 0, sizeof(node_info->id_str));

        return SUCCESS;

    }

    //EXIT COMMAND
    if(strncmp("exit\n", buffer, 5) == 0 || strncmp("x\n", buffer, 2) == 0
        || strncmp("exit ", buffer, 5) == 0 || strncmp("x ", buffer, 2) == 0)
    {
        return 10;
    }


    //CLEAR COMMAND (NOT NECESSARY)
    if(strncmp("clear\n", buffer, 5) == 0 || strncmp("clear ", buffer, 5) == 0) {
        int n = system("clear");
        if( n == -1) exit(1);

        return SUCCESS;
    }

    return UNKNOWN_COMMAND;
}



int join(node_information * node_info, char ring_id[3], char node_id[2]){

    char buffer_out[32] , buffer_in[BUFFER_SIZE], tmp[BUFFER_SIZE];
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
   
    //printf("%s\n", buffer_in);
    strcpy(tmp, buffer_in);

    char * token;

    token = strtok(buffer_in, "\n");

    token = strtok(NULL, "\n"); // Skip over NODESLIST xxx

    while (token != NULL)
    {
        
        if(token[0] > '9' || token[0] < '0') {
            token = NULL;
            break;
        }


        n_nodes++;

        sscanf(token, "%lu", &n);

        ids[n_nodes-1] = n;

        token = strtok(NULL, "\n");
    }
    
    if(n_nodes == 0) {

        printf("> No nodes in ring, joining with id %s\n", node_id);

        sprintf(buffer_out, "REG %s %s %s %s", ring_id, node_id, node_info->ipaddr, node_info->port);
        sprintf(buffer_in, " ");

        strcpy(node_info->ring_id_str, ring_id);
        strcpy(node_info->id_str, node_id);

        n = sendto(node_info->ns_fd, buffer_out, sizeof(buffer_out), 0, res->ai_addr, res->ai_addrlen);

        if(n == -1) {

            printf("Could not sendto %s:%s", node_info->ns_ipaddr, node_info->ns_port);
            fflush(stdout);
            return E_FATAL;
        }

        n = recvfrom(node_info->ns_fd, buffer_in, sizeof(buffer_in), 0, (struct sockaddr*)&addr, &addrlen);
        if(n == -1){

            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("\x1b[33m> Node Server timed out...\x1b[0m\n");
                return E_NON_FATAL;
            } else {
                return E_FATAL;
            }
        }

        //printf("%s\n", buffer_in);

        if(strncmp(buffer_in, "OKREG", 5) != 0){
            printf("> Invalid register...\n");
            return E_NON_FATAL;
        }

        return direct_join(node_info, node_id_int, node_id_int, node_info->ipaddr, node_info->port); //joining myself
        
    }

    for(int j = 0; j < n_nodes; j++){
        if(ids[j] == node_id_int){
            printf("> ID already taken...\n");
            node_id_int = 1;
            break;
        }
    }

    for(int j = 0; j < n_nodes; j++){

        if(node_id_int > 99) {
            printf("> Ring is full...\n");
            return E_NON_FATAL;
        }

        if(ids[j] == node_id_int){
            node_id_int++;
            j = 0;
        }
    }

    node_info->id = (unsigned int)node_id_int;

    int succ_id;
    char succ_tcp[6];
    char succ_ip[INET_ADDRSTRLEN];

    token = strtok(tmp, "\n");

    token = strtok(NULL, "\n"); // Skip over NODESLIST xxx

    sscanf(token, "%d %s %s", &succ_id, succ_ip, succ_tcp);

    idtostr((int)node_id_int, node_id);

    printf("\x1b[32m> Joining ring %s with id %lu\x1b[0m\n", ring_id, node_id_int);
    //printf("Picked node %d for my successor...\n", succ_id);

    strcpy(node_info->ring_id_str, ring_id);
    strcpy(node_info->id_str, node_id);           

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
            printf("\x1b[33m> Node Server timed out...\x1b[0m\n");
            return E_NON_FATAL;
        } else {
            return E_FATAL;
        }
    }

    if(strncmp(buffer_in, "OKREG", 5) != 0){
        printf("> Invalid register...\n");
        return E_NON_FATAL;
    }

    close(node_info->ns_fd);
    node_info->ns_fd = -1;

    return direct_join(node_info, node_id_int, succ_id, succ_ip, succ_tcp);

}

int direct_join(node_information * node_info, int node_id, int succ_id, char succ_ip[INET_ADDRSTRLEN], char succ_tcp[6]){

    char buffer[128];
    char connection_ip[INET_ADDRSTRLEN];
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    char id_str[3];
    id_str[2] = '\0';

    node_info->id = node_id;

    idtostr(node_id, id_str);

    //Start TCP client

    if(start_client_successor(succ_ip, succ_tcp, node_info) == - 1) {
        printf("\x1b[33m> Error connecting to TCP server @ %s:%s\x1b[0m\n", succ_ip, succ_tcp);
        return E_FATAL;
    }

    if(node_id == succ_id){
        node_info->pred_fd = accept(node_info->server_fd, &addr, &addrlen);

        short unsigned int port = ntohs(((struct sockaddr_in*)&addr)->sin_port);
        snprintf(node_info->pred_port, sizeof(node_info->temp_port), "%hu", port);

        node_info->pred_id = node_id;

        node_info->ss_id = node_id;
        strcpy(node_info->ss_ip, node_info->ipaddr);
        strcpy(node_info->ss_port, node_info->port);

        inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN);
        strcpy(node_info->pred_ip, connection_ip);

        printf("\x1b[34m> Connection accepted from %s:%s (myself)\x1b[0m\n", node_info->pred_ip, node_info->pred_port);
    }

    node_info->succ_id = succ_id;
    strcpy(node_info->succ_ip, succ_ip);
    strcpy(node_info->succ_port, succ_tcp);

    sprintf(node_info->stp_table[node_id], "%d", node_id);

    sprintf(buffer, "ENTRY %s %s %s\n", id_str, node_info->ipaddr, node_info->port);

    int n = write(node_info->succ_fd,buffer, 128);
    if (n == -1) return E_FATAL;


    //send the spt table to the guy

    send_stp_table(node_info, node_info->succ_fd);

    return SUCCESS;
}