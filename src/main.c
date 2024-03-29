/*
 * CHAT ON RING
 * PROJETO RCI 2023/24 
 * AUTORIA DE EDUARDO CASANOVA E EDUARDO SILVESTRE
 * GRUPO 56
*/

#include "utility.h"
#include "node.h"
#include "user_commands.h"
#include "server_client.h"

#include <signal.h>


#define DEFFAULT_NS_IP "193.136.138.142"
#define DEFFAULT_NS_PORT "59000"


void usage(char * arg){
    printf("Usage: %s IP TCP (regIP) (regUDP)\n", arg);
}


int main(int argc, char *argv[])
{

    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;

    if(sigaction(SIGPIPE, &act, NULL) == -1) exit(1); // Ignoring SIGPIPE signal

    char ns_ip[INET_ADDRSTRLEN];
    char ns_port[6];

    //Input validation

    if(argc  <3 || argc > 5){
        usage(argv[0]);
        exit(1);
    }


    //Check if ip is valid

    if(isip(argv[1]) != 1){
        printf("Invalid Ip address!\n");
        exit(1);
    }

    //Check if port is valid

    if(isport(argv[2]) != 1) {
        printf("Invalid TCP port!\n");
        exit(1);
    }

    if(argv[3] == NULL) {
        strcpy(ns_ip, DEFFAULT_NS_IP);
        strcpy(ns_port, DEFFAULT_NS_PORT);

    } 
    else if(isip(argv[3]) == 1) {

        strcpy(ns_ip, argv[3]);
        strcpy(ns_port, DEFFAULT_NS_PORT);

        if(argv[4] == NULL) {
            strcpy(ns_port, DEFFAULT_NS_PORT);
        
        } else if(isport(argv[4]) == 1) {
            strcpy(ns_port, argv[4]);
        } else {
            printf("Invalid node server port\n");
            exit(1);
        }

    } else {
        printf("Invalid node server IP address!\n");
        exit(1);
    }

    if(system("clear") != 0) exit(EXIT_FAILURE);

    node_information * app_node = init_node(argv[1], argv[2], 1, ns_ip, ns_port);

    printf("\x1b[32m> Starting app @%s:%s\x1b[0m\n",app_node->ipaddr, app_node->port);

    if (app_node == NULL) exit(EXIT_FAILURE);

    if(start_server_TCP(app_node) != 1){
        printf("\x1b[33m Error starting TCP server @ %s:%s\x1b[0m\n", app_node->ipaddr, app_node->port);
        free_app_node(app_node);
        exit(1);
    };
    

    printf("> ");
    fflush(stdout);

    int counter = 0;

    while (1)
    {

        cor_interrupt i = wait_for_interrupt(app_node); //Will block until something happens, timeout after 1ms


        status success = SUCCESS;

        switch (i)
        {
        case I_USER_COMMAND:
            success = execute_user_command(app_node);
            break;
        case I_MESSAGE_PREDECESSOR:
            success = process_message_frompred(app_node);
            break;
        case I_MESSAGE_SUCCESSOR:
            success = process_message_fromsucc(app_node);
            break;
        case I_NEW_CONNECTION:
            if(DEBUG) printf("New connection inbound\n");
            fflush(stdout);
            success = accept_inbound_connection(app_node);
            break;
        case I_MESSAGE_TEMP:
            if(DEBUG) printf("New message from temporary node\n> ");
            fflush(stdout);
            success = process_message_fromtemp(app_node);
            break;
        case I_MESSAGE_CHORD_IN:
            //printf("> New message from chord\n");
            //fflush(stdout);
            success = process_message_fromchord_in(app_node);
            break;
        case I_MESSAGE_CHORD_OUT:
            //printf("> New message from chord\n");
            //fflush(stdout);
            success = process_message_fromchord_out(app_node);
            break;
        default:
            break;
        }


        if(success == UNKNOWN_COMMAND) {
            printf("\x1b[33m> Unknown Command\x1b[0m\n");
            printf("> ");
            fflush(stdout);
            continue;
        }

        if(success == 10) {
            printf("\x1b[33m> Exiting now...\x1b[0m\n");
            fflush(stdout);
            free_app_node(app_node);
            break;
        }

        if(success == E_NON_FATAL) {
        }


        if(success == E_FATAL) {
            printf("\x1b[31mFatal Error Ocurred: %s\x1b[0m\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        

        if(i != I_TIMEOUT) {
            if(success == SUCCESS_HIDDEN) {
                fflush(stdout);
                continue;
            }
            printf("> ");
            fflush(stdout); 
        }

        if(i == I_TIMEOUT) counter++;




        //Quick fix for invalid chords after someone leaves
        if(counter > 500){
            counter = 0;
            if((app_node->chord_id != -1) && ((app_node->chord_id == app_node->succ_id) || (app_node->chord_id == app_node->pred_id))){

                memset(app_node->chord_ip, 0, INET_ADDRSTRLEN);
                memset(app_node->chord_port, 0, 6);
                app_node->chord_id = -1;
                close(app_node->chord_fd);
                app_node->chord_fd = -1;
            
            }
        }

    }

    


}

