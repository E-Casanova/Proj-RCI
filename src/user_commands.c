#include "user_commands.h"
#include "server.h"


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
        
    } ;


    printf("%s\n", buffer_in);


    return 1;

}

int direct_join(node_information * node_info, int node_id, int succ_id, char succ_ip[INET_ADDRSTRLEN], char succ_tcp[6]){

    //Start TCP server

    if(start_server(node_info) != 1){
        printf("\n\x1b[32m Error starting TCP server @ %s:%s", node_info->ipaddr, node_info->port);
    };
    

    return 1;
}