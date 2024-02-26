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

    int newfd = accept(node_info->server_fd, &addr, &addrlen);
    if (newfd == -1) return E_FATAL;


    inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN);

    printf("%s\n", connection_ip);

    return SUCCESS;


}


int start_client_successor(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info){

    int fd, errcode;
    ssize_t n;
    struct addrinfo hints,*res;


    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(EXIT_FAILURE);
    
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
