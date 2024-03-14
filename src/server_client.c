#include "server_client.h"



int start_server_TCP(node_information * node_info){

    int fd, errcode;
    struct addrinfo hints,*res;

    errcode = start_client_UDP(node_info->ns_ipaddr, node_info->ns_port, node_info);
    if(errcode != 1) return E_FATAL;

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

    errcode = listen(fd, 100); // lots of chords maybe
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


    inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN); //Get the ip

    port = ntohs(((struct sockaddr_in*)&addr)->sin_port); // Get the port
    snprintf(node_info->temp_port, sizeof(node_info->temp_port), "%hu", port);

    node_info->temp_fd = newfd;
    strcpy(node_info->temp_ip, connection_ip);

    printf("\x1b[34m> Connection accepted from %s:%s\x1b[0m\n", node_info->temp_ip, node_info->temp_port);

    return SUCCESS;

}

int process_message_fromtemp(node_information * node_info){

    char buffer[BUFFER_SIZE];

    int n = read(node_info->temp_fd, buffer, BUFFER_SIZE);
    if (n == -1) return E_FATAL;

    if( n == 0) {
        printf("Connection lost with temporary node...\n");
        close(node_info->temp_fd);
        node_info->temp_fd = -1;
        return E_NON_FATAL;
    };

    //printf("temp: %s\n", buffer);

    if(strncmp(buffer, "ENTRY ", 6) == 0){


        return process_ENTRY(node_info, buffer, FROM_TEMP); //Comes from temporary connection

    }

    if(strncmp(buffer, "PRED ", 5) == 0){


        return process_PRED(node_info, buffer, FROM_TEMP); // Comes from someone trying to tell me they are now my predecessor
    }    

    if(strncmp(buffer, "ROUTE ", 6) == 0){


        return process_ROUTE(node_info, buffer, FROM_TEMP); //Not likely to happen and weird
    }

    if(strncmp(buffer, "CHORD ", 6) == 0){


        return process_CHORD(node_info, buffer, FROM_TEMP); //Not likely to happen and weird
    }

    return SUCCESS_HIDDEN;
}

int process_message_frompred(node_information * node_info){


    char buffer[BUFFER_SIZE];

    int n = read(node_info->pred_fd, buffer, BUFFER_SIZE);
    if (n == -1) return E_FATAL;

    //printf("pred: %s\n", buffer);
        
    if( n == 0) {
        printf("\n> Connection closed with predecessor: %s\n", node_info->pred_ip);
        close(node_info->pred_fd);
        node_info->pred_fd = -1;

        if(node_info->pred_id != node_info->succ_id) clear_id_from_tables(node_info, node_info->pred_id);

        node_info->pred_id = -1;
        memset(node_info->pred_ip, 0, sizeof(node_info->pred_ip));
        memset(node_info->pred_port, 0, sizeof(node_info->pred_port));




        return SUCCESS;
    }

    //printf("%s", buffer);

    if(strncmp(buffer, "ENTRY ", 6) == 0){

        return process_ENTRY(node_info, buffer, FROM_PRED); // ignore

    }

    if(strncmp(buffer, "ROUTE ", 6) == 0){

        return process_ROUTE(node_info, buffer, FROM_PRED);

    }

    if(strncmp(buffer, "CHAT ", 5) == 0){

        return process_CHAT(node_info, buffer);

    }

    return SUCCESS_HIDDEN;

}

int process_message_fromsucc(node_information * node_info){

    char buffer[BUFFER_SIZE];

    int n = read(node_info->succ_fd, buffer, BUFFER_SIZE);
    if (n == -1) return E_FATAL;

    //printf("succ: %s\n", buffer);
    
    if( n == 0) {

        char id_str[3];
        id_str[2] = '\0';
        idtostr(node_info->id, id_str);

        printf("\n> Connection closed with successor: %s\n", node_info->succ_ip);
        
        close(node_info->succ_fd);
        node_info->succ_fd = -1;
        
        if(node_info->succ_id != node_info->pred_id) clear_id_from_tables(node_info, node_info->succ_id);

        node_info->succ_id = -1;

        memset(node_info->succ_ip, 0, sizeof(node_info->succ_ip));
        memset(node_info->succ_port, 0, sizeof(node_info->succ_port));

        node_info->succ_id = node_info->ss_id;
        strcpy(node_info->succ_ip, node_info->ss_ip);
        strcpy(node_info->succ_port, node_info->ss_port);


        if(node_info->id == node_info->succ_id) { // Only one node remains in ring


            char connection_ip[INET_ADDRSTRLEN];
            struct sockaddr addr;
            socklen_t addrlen = sizeof(addr);


            if(start_client_TCP(node_info->succ_ip, node_info->succ_port, node_info) == E_NON_FATAL) {
                printf("\x1b[33mError connecting to TCP server @ %s:%s\x1b[0m\n", node_info->succ_ip, node_info->succ_port);
                return E_FATAL;
            }

            node_info->pred_fd = accept(node_info->server_fd, &addr, &addrlen);

            node_info->ss_id = node_info->id;


            inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, connection_ip, INET_ADDRSTRLEN);
            short unsigned int port = ntohs(((struct sockaddr_in*)&addr)->sin_port);
            snprintf(node_info->pred_port, sizeof(node_info->temp_port), "%hu", port);

            printf("\x1b[34m> Connection accepted from %s (myself)\x1b[0m\n", connection_ip);

            node_info->pred_id = node_info->id;
            strcpy(node_info->pred_ip, connection_ip);

            /*sprintf(buffer, "ENTRY %s %s %s\n", id_str, node_info->ipaddr, node_info->port);

            int n = write(node_info->succ_fd,buffer, BUFFER_SIZE);
            if (n == -1) return E_FATAL; */

            return SUCCESS;
        }


        if(start_client_TCP(node_info->succ_ip, node_info->succ_port, node_info) == E_NON_FATAL) {
            printf("\x1b[33mError connecting to TCP server @ %s:%s\x1b[0m\n", node_info->succ_ip, node_info->succ_port);
            return E_FATAL;
        }

        sprintf(buffer, "PRED %s\n", id_str);

        n = write(node_info->succ_fd, buffer, BUFFER_SIZE);
        if(n == -1) exit(EXIT_FAILURE);

        idtostr(node_info->succ_id, id_str);

        sprintf(buffer, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

        n = write(node_info->pred_fd, buffer, BUFFER_SIZE);
        if( n == -1) exit(EXIT_FAILURE);

        send_stp_table(node_info, node_info->succ_fd);

        return SUCCESS;
    }

    if(strncmp(buffer, "ENTRY ", 6) == 0){

        return process_ENTRY(node_info, buffer, FROM_SUCC); // Comes from successor

    }

    if(strncmp(buffer, "SUCC ", 5) == 0){

        return process_SUCC(node_info, buffer, FROM_SUCC);

    }

    if(strncmp(buffer, "PRED ", 5) == 0){

        return process_PRED(node_info, buffer, FROM_SUCC);
    }

    if(strncmp(buffer, "ROUTE ", 6) == 0){

        return process_ROUTE(node_info, buffer, FROM_SUCC);
    }

    if(strncmp(buffer, "CHAT ", 5) == 0){

        return process_CHAT(node_info, buffer);
    }

    return SUCCESS_HIDDEN;
}

int process_message_fromchord_out(node_information * node_info){


    char buffer[BUFFER_SIZE];

    int n = read(node_info->chord_fd, buffer, BUFFER_SIZE);
    if (n == -1) return E_FATAL;

    if(n == 0) {
        printf("> Connection closed with chord: %s", node_info->chord_ip);

        clear_id_from_tables(node_info, node_info->chord_id);

        close(node_info->chord_fd);

        node_info->chord_id = -1;
        memset(node_info->chord_ip, 0, INET_ADDRSTRLEN);
        memset(node_info->chord_port, 0, 6);
        node_info->chord_fd = -1;

        return SUCCESS;

    }

    //printf("\n%s\n", buffer);

    if(strncmp(buffer, "ROUTE ", 6) == 0){

        return process_ROUTE(node_info, buffer, FROM_CHORD);
    }

    if(strncmp(buffer, "CHAT ", 5) == 0){

        return process_CHAT(node_info, buffer);
    }


    return SUCCESS_HIDDEN;

}

int process_message_fromchord_in(node_information * node_info){


    char buffer[BUFFER_SIZE];

    chord_information * tmp = node_info->chord_head;
    chord_information * tmp2 = tmp; // this will lag behind the main pointer

    tmp = tmp->next;

    while (tmp != NULL)
    {
        if(tmp->active == 1) {
            int n = read(tmp->chord_fd, buffer, BUFFER_SIZE);
            if (n == -1) return E_FATAL;

            tmp->active = 0;

            if(n == 0) {
                printf("> Connection closed with chord: %s\n", tmp->chord_ip);

                clear_id_from_tables(node_info, tmp->chord_id);

                close(tmp->chord_fd);

                tmp->chord_id = -1;
                memset(tmp->chord_ip, 0, INET_ADDRSTRLEN);
                memset(tmp->chord_port, 0, 6);
                tmp->chord_fd = -1;

                tmp2->next = tmp->next;

                free(tmp);

                return SUCCESS;

            }

            //printf("\n%s\n", buffer);

            if(strncmp(buffer, "ROUTE ", 6) == 0){

                return process_ROUTE(node_info, buffer, FROM_CHORD);
            }

            if(strncmp(buffer, "CHAT ", 5) == 0){

                return process_CHAT(node_info, buffer);
            }


            break;
        }

        tmp2 = tmp2->next;
        tmp = tmp->next;
    }

    return SUCCESS_HIDDEN;

}


int start_client_TCP(char succ_ip[INET_ADDRSTRLEN], char port[6], node_information * node_info){

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

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        errno = EWOULDBLOCK;
        return E_NON_FATAL;
    }


    node_info->succ.res = res;
    node_info->succ_fd = fd;
    strcpy(node_info->succ_ip , succ_ip);
    strcpy(node_info->succ_port , port);

    return SUCCESS_HIDDEN;
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



int process_ENTRY(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who){

    char message[BUFFER_SIZE];
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


    if(who == FROM_SUCC) { // If ENTRY comes from my successor, it means a new node has joined him


        sprintf(message, "SUCC %s %s %s\n", id_str, ip, port);

        n = write(node_info->pred_fd, message, BUFFER_SIZE); //SUCC i i.IP i.TCP\n
        if (n == -1) exit(1);


        node_info->ss_id = node_info->succ_id;
        node_info->succ_id = id;
        strcpy(node_info->succ_ip, ip);
        strcpy(node_info->succ_port, port);

        close(node_info->succ_fd);
        node_info->succ_fd = -1;

        n = getaddrinfo(ip, port, &hints, &res);
        if (n == -1) exit(1);

        if(node_info->ss_id != node_info->pred_id) clear_id_from_tables(node_info, node_info->ss_id); // this might be wrong

        node_info->succ_fd = socket(AF_INET, SOCK_STREAM, 0);


        n = connect(node_info->succ_fd, res->ai_addr, res->ai_addrlen);
        if (n == -1) exit(1);

        idtostr(node_info->id, id_str);

        sprintf(message, "PRED %s\n", id_str);

        n = write(node_info->succ_fd, message, BUFFER_SIZE); //PRED l\n
        if (n == -1) exit(1);

        //Now send him all my spt table


        send_stp_table(node_info, node_info->succ_fd);



    } else if(who == FROM_TEMP) { // Message comes from random node

        if((node_info->succ_id == node_info->id ) || (node_info->pred_id == node_info->id)){ // only one node in ring

            //printf("Only one node in ring...\n");

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

            node_info->temp_fd = -1;
            
            sprintf(message, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

            n = write(node_info->pred_fd, message, BUFFER_SIZE);
            if (n == -1) exit(1);


            n = getaddrinfo(node_info->succ_ip, node_info->succ_port, &hints, &res);
            if (n != 0) exit(1);

            //close(node_info->succ_fd);

            node_info->succ_fd = socket(AF_INET, SOCK_STREAM, 0);


            n = connect(node_info->succ_fd, res->ai_addr, res->ai_addrlen);

            idtostr(node_info->id, id_str);

            sprintf(message, "PRED %s\n", id_str);

            n = write(node_info->succ_fd, message, BUFFER_SIZE);
            if (n == -1) exit(1);



            //Send him all my stp table

            send_stp_table(node_info, node_info->pred_fd);


            return SUCCESS_HIDDEN;

        }
        


        n = write(node_info->pred_fd, buffer, BUFFER_SIZE);
        if (n == -1) exit(1);

        strcpy(node_info->pred_ip, ip);
        strcpy(node_info->pred_port, node_info->temp_port);

        close(node_info->pred_fd);
        node_info->pred_fd = node_info->temp_fd;

        node_info->temp_fd = -1;

        if(node_info->succ_id != node_info->pred_id) clear_id_from_tables(node_info, node_info->pred_id);

        node_info->pred_id = id;

        idtostr(node_info->succ_id, id_str);

        sprintf(message, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

        n = write(node_info->pred_fd, message, BUFFER_SIZE);
        if (n == -1) exit(1);


        //Send him all my spt table
        
        send_stp_table(node_info, node_info->pred_fd);



    } else if(who == FROM_PRED) { // Comes from predecessor


        if(node_info->pred_id == node_info->id){
            //printf("Started a new ring...\n");
        }

    };

    return SUCCESS_HIDDEN;

}

int process_SUCC(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who){

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

    if (who == FROM_SUCC) { // comming from my successor
        //printf("Set node %s as second successor...\n", id_str);
        node_info->ss_id = id;
        strcpy(node_info->ss_ip, ip);
        strcpy(node_info->ss_port, port);
    }


    return SUCCESS_HIDDEN;

}

int process_PRED(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who){
    
    
    int id;
    char id_str[3];
    char command[32];
    int n;
    char buffer_to_send[BUFFER_SIZE];

    id_str[2] = '\0';

    n = sscanf(buffer, "%s %d\n", command, &id);
    if(n != 2){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

    idtostr(id, id_str);

    if (who == FROM_TEMP) { // random node (temp)
        //printf("Set node %s as predecessor...\n", id_str);
        node_info->pred_id = id;
        node_info->pred_fd = node_info->temp_fd;

        node_info->temp_fd = -1;

        strcpy(node_info->pred_ip, node_info->temp_ip);
        strcpy(node_info->pred_port, node_info->temp_port);

        idtostr(node_info->succ_id, id_str);

        sprintf(buffer_to_send, "SUCC %s %s %s\n", id_str, node_info->succ_ip, node_info->succ_port);

        n = write(node_info->pred_fd, buffer_to_send, BUFFER_SIZE);
        if( n == -1) exit(EXIT_FAILURE);

        send_stp_table(node_info, node_info->pred_fd);


    }

    if (who == FROM_SUCC) { // coming from sucessor, only happens if there are only 2 nodes in ring
        //printf("Set node %s as predecessor...\n", id_str);
        node_info->pred_id = node_info->succ_id;
        strcpy(node_info->pred_ip, node_info->succ_ip);
        strcpy(node_info->pred_port, node_info->succ_port);

    }

    return SUCCESS_HIDDEN;
}


int process_ROUTE(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who){

    int id_neighbour, id_dest;
    char command[32], route[300], route_cpy[300];
    char * token, *token2, tmp[300];
    int n, i, count1;
    int n_hops;
    //int pos, min;
    int is_valid = 1;
    

    n = sscanf(buffer, "%s %d %d %s\n", command, &id_neighbour, &id_dest, route);
    if((n > 4) || (n < 3)){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

   /* if(n == 3) { // this means we got a clear route instruction, the path is no longer valid, im just going to assume the node does not exist anymore
        memset(route, 0, 300); // the assumption was retarded
        route[0] = '-';

        pos = -1;
        min = 100;

        if((node_info->fwd_table[id_dest][id_neighbour][0] != '\0') && (strcmp(node_info->fwd_table[id_dest][id_neighbour], node_info->stp_table[id_dest]) == 1))
        {
            memset(buffer, 0, BUFFER_SIZE);

            for(int j = 0; j < 100; j++) {
                if(node_info->fwd_table[id_dest][j][0] == '\0') continue;

                count1 = 0;

                if(j == id_dest) continue;

                for (int k=0; node_info->fwd_table[id_dest][j][k]; k++) {
                    count1 += (node_info->fwd_table[id_dest][j][k] == '-');
                }
                if( count1 < min ){
                    min = count1;
                    pos = j;
                } 

            }

            if (pos == -1) { // we found no path
                memset(node_info->stp_table[id_dest], 0, 100);
                node_info->exp_table[id_dest] = 0;
                announce_shortest_path(node_info, buffer, node_info->id, id_dest);
            }

            if( pos > -1) {
                strcpy(node_info->stp_table[id_dest], node_info->fwd_table[id_dest][pos]);
                node_info->exp_table[id_dest] = pos;
                announce_shortest_path(node_info, node_info->stp_table[id_dest], node_info->id, id_dest);
            }

        }


        memset(node_info->fwd_table[id_dest][id_neighbour], 0, 100);

        //maybe remove from the others too lol

        return SUCCESS_HIDDEN;
    }*/
    //First we check if path is valid...

    if(n == 3) is_valid = 0;

    if(is_valid) {
        strcpy(route_cpy, route);
        
        token = strtok(route_cpy, "\n");

        strcpy(tmp, token);

        token2 = strtok(tmp, "-");

        n_hops = -1; 

        while (token2 != NULL)
        {

            //printf("%s\n", token2);

            sscanf(token2, "%d", &n);

            if(n == node_info->id) {
                //printf("> Invalid path recieved...\n");
                is_valid = 0;
                break;
            }

            n_hops++;

            token2 = strtok(NULL, "-");

        }
        
        token = strtok(route, "\n");
    }
    //printf("%s\n %d %d %d hops\n", token, id_neighbour, id_dest, n_hops);

    //MAYBE CHECK IF ROUTE EXISTS??? Probably impossible to do but can check if comes from neighbour

    if(node_info->fwd_table[id_dest][id_neighbour][0] == '\0'){

        if(is_valid == 1) sprintf(node_info->fwd_table[id_dest][id_neighbour], "%d-%s", node_info->id, route);
        

        //printf("Added entry to forwarding table with path: %s\n", node_info->fwd_table[id_dest][id_neighbour]);


    } else {


        if(strcmp(node_info->fwd_table[id_dest][id_neighbour], node_info->stp_table[id_dest]) == 0){
            // I have to update my spt too

            if(is_valid == 1)  sprintf(node_info->fwd_table[id_dest][id_neighbour], "%d-%s", node_info->id, route);
            if(is_valid == 0)   memset(node_info->fwd_table[id_dest][id_neighbour], 0, 100);

            // Now i have to see which path is shortest

            int min = 100, pos = -1;

            for(int j = 0; j < 100; j++){

                if(node_info->fwd_table[id_dest][j][0] == '\0') continue;

                for (i=0, count1=0; node_info->fwd_table[id_dest][j][i]; i++) count1 += (node_info->fwd_table[id_dest][j][i] == '-');

                if( count1 < min ) {
                    min = count1;
                    pos = j;
                }

            }

            if(pos == -1) memset(node_info->stp_table[id_dest], 0, 100);

            if(pos > -1)  strcpy(node_info->stp_table[id_dest], node_info->fwd_table[id_dest][pos]);

            //printf("1> Added entry %d on stp table with %s\n", id_dest, node_info->stp_table[id_dest]);


            //Update expedition table

            if(pos == -1) node_info->exp_table[id_dest] = 0;

            if(pos > -1)  node_info->exp_table[id_dest] = pos;

            //Gotta anounce it to all our neighbours, gonna write a function for that

            return announce_shortest_path(node_info, node_info->stp_table[id_dest], node_info->id, id_dest);


        }


        if(is_valid == 1) sprintf(node_info->fwd_table[id_dest][id_neighbour], "%d-%s", node_info->id, route);
        if(is_valid == 0)  memset(node_info->fwd_table[id_dest][id_neighbour], 0, 100);
        //printf("Updated forwarding table entry with path: %s\n", node_info->fwd_table[id_dest][id_neighbour]);

    }


    //Now we check the shortest path table (stp)


    if(node_info->stp_table[id_dest][0] == '\0') { // If it doesn't exist we add it

        if(is_valid == 1){

            strcpy(node_info->stp_table[id_dest], node_info->fwd_table[id_dest][id_neighbour]);
            //printf("2> Added entry %d on stp table with %s\n", id_dest, node_info->stp_table[id_dest]);


            //Update expedition table

            node_info->exp_table[id_dest] = id_neighbour;

            //Gotta anounce it to all our neighbours, gonna write a function for that

            return announce_shortest_path(node_info, node_info->stp_table[id_dest], node_info->id, id_dest);
        }

    } else { // We have to count THE '-' !!! bruh.  Because 30-20-10 is a longer string than 1-2-3-4 but shorter path

        //TEMOS QUE FAZER ISTO OU NAO???

        if(is_valid == 0) return SUCCESS_HIDDEN;

        for (i=0, count1=0; node_info->stp_table[id_dest][i]; i++) count1 += (node_info->stp_table[id_dest][i] == '-');
        if(n_hops < (count1 - 1)) { //Means our new path is shorter
            strcpy(node_info->stp_table[id_dest], node_info->fwd_table[id_dest][id_neighbour]);
            //printf("> Replaced entry %d on stp table with %s\n", id_dest, node_info->stp_table[id_dest]);

            //Update expedition table

            node_info->exp_table[id_dest] = id_neighbour;

            //Gotta anounce it to all our neighbours, gonna write a function for that

            return announce_shortest_path(node_info, node_info->stp_table[id_dest], node_info->id, id_dest);
        }


        //if the path is longer we do nothing

    }

    return SUCCESS_HIDDEN;

}


int process_CHAT(node_information * node_info, char buffer[CHAT_BUFFER_SIZE]){

    char sent[CHAT_BUFFER_SIZE];
    char message[128], command[32];
    int from, dest, n, next_node;
    char id_str[3];
    
    id_str[2] = '\0';

    n = sscanf(buffer, "%s %d %d %[^\n]s\n", command, &from, &dest, message);

    if((n != 4)){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

    idtostr(from, id_str);

    if(dest == node_info->id) {

        printf("\x1b\n[35m> --- INCOMING MESSAGE FROM NODE: %s ---\n", id_str);
        printf("\n %s \n", message);
        printf("\x1b[0m\n> ");

        return SUCCESS_HIDDEN;

    }


    next_node = node_info->exp_table[dest];

    sprintf(sent, "CHAT %d %d %s\n", from, dest, message);

    if(next_node == node_info->succ_id){
        n = write(node_info->succ_fd, sent, CHAT_BUFFER_SIZE);
        if(n == -1) exit(EXIT_FAILURE);
        printf("\x1b[32m> Sent...\x1b[0m\n");
        return SUCCESS;
    }

    if(next_node == node_info->pred_id){
        n = write(node_info->pred_fd, sent, CHAT_BUFFER_SIZE);
        if(n == -1) exit(EXIT_FAILURE);
        printf("\x1b[32m> Sent...\x1b[0m\n");
        return SUCCESS;
    }

    if(next_node == node_info->chord_id){
        n = write(node_info->chord_fd, sent, CHAT_BUFFER_SIZE);
        if(n == -1) exit(EXIT_FAILURE);
        printf("\x1b[32m> Sent...\x1b[0m\n");
        return SUCCESS;
    }

    chord_information * tmp;

    tmp = node_info->chord_head;

    while (tmp != NULL)
    {
        if((tmp->chord_id == next_node) && (tmp->chord_fd != -1)){
            n = write(tmp->chord_fd, sent, CHAT_BUFFER_SIZE);
            if(n == -1) exit(EXIT_FAILURE);
            printf("\x1b[32m> Sent...\x1b[0m\n");
            return SUCCESS;
        }

        tmp = tmp->next;

    }
    


    printf("\x1b[33m> Message was not sent\x1b[0m\n");
    return E_NON_FATAL;
}


int process_CHORD(node_information * node_info, char buffer[BUFFER_SIZE], whofrom who){


    char cmd[32];
    int id;
    int n;

    n = sscanf(buffer, "%s %d\n", cmd, &id);
    if((n != 2)){
        printf("\x1b[33mError - Bad format...\x1b[0m\n");
        return E_NON_FATAL;
    }

    if(who == FROM_SUCC || who == FROM_PRED) return E_NON_FATAL; //weird stuff that will never happen, i will ignore it anyways

    if(who == FROM_TEMP){

        chord_information * tmp = (chord_information*)malloc(sizeof(chord_information));
        chord_information * tmp2;

        tmp->chord_fd = node_info->temp_fd;
        tmp->chord_id = id;
        tmp->active = 0;

        strcpy(tmp->chord_ip, node_info->temp_ip);
        strcpy(tmp->chord_port, node_info->temp_port);

        node_info->temp_fd = -1;

        tmp2 = node_info->chord_head;

        while (tmp2->next != NULL)
        {
            tmp2 = tmp2->next;
        }
        
        tmp2->next = tmp;

        printf("> Set chord %i\n", tmp->chord_id);

        send_stp_table(node_info, tmp->chord_fd);

    }


    return SUCCESS;

}


int announce_shortest_path(node_information * node_info, char path[BUFFER_SIZE], int start, int end){


    char buffer_out[BUFFER_SIZE];
    int n; 

    sprintf(buffer_out, "ROUTE %d %d %s\n", start, end, path); //We format the message to send to all our neighbours


    if((node_info->succ_fd != -1) && (node_info->succ_id != node_info->id)) {

        n = write(node_info->succ_fd, buffer_out, BUFFER_SIZE); // tell it to my successor
        if(n == -1) return E_FATAL;
    }

    if((node_info->pred_fd != -1) && (node_info->pred_id != node_info->id)) {

        n = write(node_info->pred_fd, buffer_out, BUFFER_SIZE); // tell it to my predecessor
        if(n == -1) return E_FATAL;

    }


    if((node_info->chord_fd != -1) && (node_info->chord_id != node_info->id)) {

        n = write(node_info->chord_fd, buffer_out, BUFFER_SIZE); // tell it to the chord that's connected to me
        if(n == -1) return E_FATAL;

    }
    
    chord_information * tmp = node_info->chord_head; // tell it to all the chords connected to me

    while (tmp != NULL)
    {
        if(tmp->chord_fd != -1) {
            n = write(tmp->chord_fd, buffer_out, BUFFER_SIZE); // tell it to the chord that's connected to me
            if(n == -1) return E_FATAL;
        }

        tmp = tmp->next;
    }
    


    return SUCCESS_HIDDEN;
}


int announce_shortest_path_neighbour(node_information * node_info, char path[BUFFER_SIZE], int start, int end, int fd){

    char buffer_out[BUFFER_SIZE];
    int n; 

    sprintf(buffer_out, "ROUTE %d %d %s\n", start, end, path); //We format the message to send to all our neighbours


    if((fd != -1)) {

        n = write(fd, buffer_out, BUFFER_SIZE); // tell it to my successor
        if(n == -1) return E_FATAL;
    }


    return SUCCESS;

}

void send_stp_table(node_information * node_info, int fd){

    char message[BUFFER_SIZE];
    int n;

    for(int i = 0; i < 100; i++){
        if(node_info->stp_table[i][0] != '\0') {
            sprintf(message, "ROUTE %d %d %s\n", node_info->id, i, node_info->stp_table[i]);
            n = write(fd, message, BUFFER_SIZE); // Spam routes to my predecessor because i just established a connection
            if (n == -1) exit(1);
        }
    }
}

void clear_id_from_tables(node_information * node_info, int id){

    int min = 100, pos = -1, count1;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);


    for(int i = 0; i < 100; i++) {

        if((node_info->fwd_table[i][id][0] != '\0') && (strcmp(node_info->fwd_table[i][id], node_info->stp_table[i]) == 0)){

            for(int j = 0; j < 100; j++) {
                if(node_info->fwd_table[i][j][0] == '\0') continue;

                count1 = 0;

                if(j == id) continue;

                for (int k=0; node_info->fwd_table[i][j][k]; k++) {
                     count1 += (node_info->fwd_table[i][j][k] == '-');
                }
                if( count1 < min ){
                    min = count1;
                    pos = j;
                } 

            }

            if (pos == -1) { // we found no path
                memset(node_info->stp_table[i], 0, 100);
                node_info->exp_table[i] = 0;
                announce_shortest_path(node_info, buffer, node_info->id, id);
            }

            if( pos > -1) {
                strcpy(node_info->stp_table[i], node_info->fwd_table[i][pos]);
                node_info->exp_table[i] = pos;
                announce_shortest_path(node_info, node_info->stp_table[i], node_info->id, i);
            }

        }

            memset(node_info->fwd_table[i][id], 0, 100);
        }

        for(int i = 0; i < 100; i++) {
            if(node_info->exp_table[i] == id) node_info->exp_table[i] = 0;
        }
    

}


