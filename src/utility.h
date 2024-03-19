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

/*
 * Enumeration to represent various types of interrupts
 */
typedef enum interrupt {
    I_USER_COMMAND,           // Interrupt triggered by a user command
    I_MESSAGE_TEMP,           // Interrupt triggered by a message from a temporary node
    I_NODE_SERVER_MSG,        // Interrupt triggered by a message from a node server
    I_MESSAGE_PREDECESSOR,    // Interrupt triggered by a message from a predecessor node
    I_MESSAGE_SUCCESSOR,      // Interrupt triggered by a message from a successor node
    I_NEW_CONNECTION,         // Interrupt triggered by a new inbound connection
    I_TIMEOUT,                // Interrupt triggered by a timeout
    I_ERROR,                  // Interrupt triggered by an error condition
    I_MESSAGE_CHORD_IN,       // Interrupt triggered by a message from a chord connected to me
    I_MESSAGE_CHORD_OUT       // Interrupt triggered by a message from a chord im connected to
} cor_interrupt;


typedef enum status {
    E_FATAL = -1,         // Fatal error 
    E_NON_FATAL = -2,     // Non-fatal error 
    SUCCESS = 1,          // Success 
    UNKNOWN_COMMAND = 2,  // Unknown command 
    SUCCESS_HIDDEN = 3    // Success but don't print anything after 
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
 * Waits for an event and returns it when it occurs
 */
cor_interrupt wait_for_interrupt(node_information *node_info);


/*
 * Somehow strnlen does not work so we have to make our own
 */
size_t strnlen_2(const char * s, size_t maxlen);



/*
 * Converts an integer ID to a string representation
 */
void idtostr(int id, char str[2]);

/*
 * Converts a string representation to an integer ID
 */
void strtoid(int id, char str[2]);


#endif
