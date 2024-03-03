#include "server_client.h"



int start_server_TCP(node_information * node_info){

    int fd, errcode;
    struct addrinfo hints,*res;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, node_info->port, &hints, &res);
    if (errcode != 0) return -1;

    errcode = bind(fd, res->ai_addr, res->ai_addrlen);
    if (errcode == -1) return -1;

    errcode = listen(fd, 100);
    if (errcode == -1) return -1;

    node_info->pred.res = res;
    node_info->server_fd = fd;
    



    return 1;
}


int accept_inbound_connection(node_information * node_info){

    char connection_ip[INET_ADDRSTRLEN];
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int port;

    int newfd = accept(node_info->server_fd, &addr, &addrlen);
    if (newfd == -1) return E_FATAL;


    inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN);

    printf("\x1b[34mConnection accepted from %s\x1b[0m\n", connection_ip);

    node_info->temp_fd = newfd;
    strcpy(node_info->temp_ip, connection_ip);

    port = ntohs(((struct sockaddr_in*)&addr)->sin_port);
    snprintf(node_info->temp_port, sizeof(node_info->temp_port), "%hu", port);

    return SUCCESS;


}

int process_message_fromtemp(node_information * node_info){

    char buffer[128];

    int n = read(node_info->temp_fd, buffer, 128);
    if (n == -1) return E_FATAL;

    if( n == 0) {
        close(node_info->temp_fd);
        node_info->temp_fd = -1;
        return E_NON_FATAL;
    };

    printf("\n%s", buffer);

    if(strncmp(buffer, "ENTRY ", 6) == 0){


        return process_ENTRY(node_info, buffer, 0); //Comes from temporary connection

    }

    if(strncmp(buffer, "PRED ", 5) == 0){


        return process_PRED(node_info, buffer, 0); // Comes from someone trying to tell me they are now my predecessor
    }

    return SUCCESS;
}

int process_message_frompred(node_information * node_info){


    char buffer[128];

    int n = read(node_info->pred_fd, buffer, 128);
    if (n == -1) return E_FATAL;

        
    if( n == 0) {
        printf("\nConnection closed with predecessor: %s\n", node_info->pred_ip);
        close(node_info->pred_fd);
        node_info->pred_id = -1;
        node_info->pred_fd = -1;
        memset(node_info->pred_ip, 0, sizeof(node_info->pred_ip));
        memset(node_info->pred_port, 0, sizeof(node_info->pred_port));
        return E_NON_FATAL;
    }

    printf("%s", buffer);

    if(strncmp(buffer, "ENTRY ", 6) == 0){

        return process_ENTRY(node_info, buffer, -1); // ignore

    }

    return SUCCESS;

}

int process_message_fromsucc(node_information * node_info){

    char buffer[128];

    int n = read(node_info->succ_fd, buffer, 128);
    if (n == -1) return E_FATAL;

    printf("%s", buffer);
    
    if( n == 0) {

        printf("\nConnection closed with successor: %s\n", node_info->succ_ip);
        close(node_info->succ_fd);
        node_info->succ_id = -1;
        node_info->succ_fd = -1;
        memset(node_info->succ_ip, 0, sizeof(node_info->succ_ip));
        memset(node_info->succ_port, 0, sizeof(node_info->succ_port));

        node_info->succ_id = node_info->ss_id;
        strcpy(node_info->succ_ip, node_info->ss_ip);
        strcpy(node_info->succ_port, node_info->ss_port);


        if(node_info->id == node_info->succ_id) { // Only one node remains in ring
        
            char id_str[3];
            id_str[2] = '\0';
            idtostr(node_info->id, id_str);


            char connection_ip[INET_ADDRSTRLEN];
            struct sockaddr addr;
            socklen_t addrlen = sizeof(addr);


            if(start_client_successor(node_info->succ_ip, node_info->succ_port, node_info) == - 1) {
                printf("\x1b[33mError connecting to TCP server @ %s:%s\x1b[0m\n", node_info->succ_ip, node_info->succ_port);
                return E_FATAL;
            }

            node_info->pred_fd = accept(node_info->server_fd, &addr, &addrlen);

            node_info->pred_id = node_info->id;
            strcpy(node_info->pred_ip, node_info->ipaddr);
            strcpy(node_info->pred_port, node_info->port);

            node_info->ss_id = node_info->id;


            inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN);

            printf("\x1b[34mConnection accepted from %s (myself)\x1b[0m\n", connection_ip);

            sprintf(buffer, "ENTRY %s %s %s\n", id_str, node_info->ipaddr, node_info->port);

            int n = write(node_info->succ_fd,buffer, 128);
            if (n == -1) return E_FATAL;

            return 1;
        }


        n = start_client_successor(node_info->succ_ip, node_info->succ_port, node_info);



        return SUCCESS;
    }

    if(strncmp(buffer, "ENTRY ", 6) == 0){

        return process_ENTRY(node_info, buffer, 1); // Comes from successor

    }

    if(strncmp(buffer, "SUCC ", 5) == 0){

        return process_SUCC(node_info, buffer, 1);

    }

    if(strncmp(buffer, "PRED ", 4) == 0){

        return process_PRED(node_info, buffer, 1);
    }

    return SUCCESS;
}




int start_client_successor(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info){

    int fd, errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    struct timeval timeout = {.tv_sec = 5 , .tv_usec = 0};


    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(EXIT_FAILURE);
    
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(succ_ip,port,&hints,&res);
    if( errcode!= 0) return -1;

    node_info->succ.res = res;
    node_info->succ_fd = fd;
    strcpy(node_info->succ_ip , succ_ip);
    strcpy(node_info->succ_port , port);

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        errno = EWOULDBLOCK;
        return -1;
    }

    return 1;
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



int process_ENTRY(node_information * node_info, char buffer[BUFFER_SIZE], int whofrom){

    char message[128];
    int id;
    char ip[INET_ADDRSTRLEN];
    char port[6];
    char command[32];
    int n;
    char id_str[3];


    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    n = sscanf(buffer, "%s %d %s %s\n", command, &id, ip, port);
    if(n != 4){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }
    sscanf(buffer, "%s %s %s %s\n", command, id_str, ip, port);


    if(whofrom == 1) { // If ENTRY comes from my successor, it means a new node has joined him


        sprintf(message, "SUCC %s %s %s\n", id_str, ip, port);

        n = write(node_info->pred_fd, message, 128); //SUCC i i.IP i.TCP\n
        if (n == -1) exit(1);


        node_info->ss_id = node_info->succ_id;
        node_info->succ_id = id;
        strcpy(node_info->succ_ip, ip);
        strcpy(node_info->succ_port, port);

        close(node_info->succ_fd);

        n = getaddrinfo(ip, port, &hints, &res);
        if (n == -1) exit(1);

        node_info->succ_fd = socket(AF_INET, SOCK_STREAM, 0);

        n = connect(node_info->succ_fd, res->ai_addr, res->ai_addrlen);
        if (n == -1) exit(1);

        idtostr(node_info->id, id_str);

        sprintf(message, "PRED %s\n", id_str);

        n = write(node_info->succ_fd, message, 128); //PRED l\n
        if (n == -1) exit(1);

    } else if(whofrom == 0) { // Message comes from random node

        if((node_info->succ_id == node_info->id ) || (node_info->pred_id == node_info->id)){ // only one node in ring

            printf("Only one node in ring...\n");

            node_info->ss_id = node_info->id;

            node_info->succ_id = id;
            node_info->pred_id = id;

            strcpy(node_info->succ_ip, ip);
            strcpy(node_info->pred_ip, ip);

            strcpy(node_info->succ_port, port);
            strcpy(node_info->pred_port, node_info->temp_port);

            close(node_info->pred_fd);
            close(node_info->succ_fd);
            node_info->pred_fd = node_info->temp_fd;
            
            sprintf(message, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

            n = write(node_info->pred_fd, message, 128);
            if (n == -1) exit(1);


            n = getaddrinfo(node_info->succ_ip, node_info->succ_port, &hints, &res);
            if (n != 0) exit(1);

            close(node_info->succ_fd);

            node_info->succ_fd = socket(AF_INET, SOCK_STREAM, 0);


            n = connect(node_info->succ_fd, res->ai_addr, res->ai_addrlen);

            idtostr(node_info->id, id_str);

            sprintf(message, "PRED %s\n", id_str);

            n = write(node_info->succ_fd, message, 128);
            if (n == -1) exit(1);


            return SUCCESS;

        }
        


        n = write(node_info->pred_fd, buffer, 128);
        if (n == -1) exit(1);

        node_info->pred_id = id;
        strcpy(node_info->pred_ip, ip);
        strcpy(node_info->pred_port, node_info->temp_port);

        close(node_info->pred_fd);
        node_info->pred_fd = node_info->temp_fd;

        idtostr(node_info->succ_id, id_str);

        sprintf(message, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

        n = write(node_info->pred_fd, message, 128);
        if (n == -1) exit(1);


    } else if(whofrom == -1) {


        if(node_info->pred_id == node_info->id){
            printf("Started a new ring...\n");
        }

    };

    return SUCCESS;

}

int process_SUCC(node_information * node_info, char buffer[BUFFER_SIZE],int whofrom){

    int id;
    char ip[INET_ADDRSTRLEN];
    char port[6], id_str[3];
    char command[32];
    int n;

    id_str[2] = '\0';

    n = sscanf(buffer, "%s %d %s %s\n", command, &id, ip, port);
    if(n != 4){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

    idtostr(id, id_str);

    if (whofrom == 1) {
        printf("Set node %s as second successor...\n", id_str);
        node_info->ss_id = id;
        strcpy(node_info->ss_ip, ip);
        strcpy(node_info->ss_port, port);
    }


    return SUCCESS;

}

int process_PRED(node_information * node_info, char buffer[BUFFER_SIZE], int whofrom){
    
    
    int id;
    char id_str[3];
    char command[32];
    int n;

    id_str[2] = '\0';

    n = sscanf(buffer, "%s %d\n", command, &id);
    if(n != 2){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

    idtostr(id, id_str);

    if (whofrom == 0) { // random node (temp)
        printf("Set node %s as predecessor...\n", id_str);
        node_info->pred_id = id;
        node_info->pred_fd = node_info->temp_fd;
        strcpy(node_info->pred_ip, node_info->temp_ip);
        strcpy(node_info->pred_port, node_info->temp_port);


    }

    if (whofrom == 1) { // coming from sucessor, only happens if there are only 2 nodes in ring
        printf("Set node %s as predecessor...\n", id_str);
        node_info->pred_id = node_info->succ_id;
        strcpy(node_info->pred_ip, node_info->succ_ip);
        strcpy(node_info->pred_port, node_info->succ_port);

    }

    return SUCCESS;
}