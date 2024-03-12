#ifndef UTILITY_H
#define UTILITY_H

#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 700

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "node.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define BUFFER_SIZE 300
#define CHAT_BUFFER_SIZE 300


typedef enum interrupt {

    I_USER_COMMAND,
    I_MESSAGE_TEMP,
    I_NODE_SERVER_MSG,
    I_MESSAGE_PREDECESSOR,
    I_MESSAGE_SUCCESSOR,
    I_NEW_CONNECTION,
    I_TIMEOUT,
    I_ERROR,
    I_MESSAGE_CHORD_IN,
    I_MESSAGE_CHORD_OUT

} cor_interrupt;


typedef enum status {
    E_FATAL = -1,
    E_NON_FATAL = -2,
    SUCCESS = 1,
    UNKNOWN_COMMAND = 2,
    SUCCESS_HIDDEN = 3
} status;


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


/*
*
* Somehow strnlen does not work so we have to make our own
*
*/
size_t strnlen_2(const char * s, size_t maxlen);



void idtostr(int id, char str[2]);

void strtoid(int id, char str[2]);


#endif