#ifndef UTILITY_H
#define UTILITY_H

#define _POSIX_C_SOURCE 200112L

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>


typedef enum interrupt {

    I_USER_COMMAND,
    I_NODE_SERVER_MSG,
    I_MESSAGE_PREDECESSOR,
    I_MESSAGE_SUCCESSOR,
    I_NEW_CONNECTION,
    I_TIMEOUT,
    I_ERROR

} cor_interrupt;



/*
*
*   Checks if string is a valid port
*
*   @returns 1 if valid 
*/
int isport(const char * str);


/*
*
*   Checks if string is a valid IPV4 address
*   
*   @returns 1 if valid
*/
int isip(const char * str);

/*
*
*
*   Waits for an event and returns it when it occurs
*
*/
cor_interrupt wait_for_interrupt(node_information *node_info);

#endif