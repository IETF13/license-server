/***********************
 *lsrv_funcs1.c
 *functions for the license server
 */

#ifndef __linux__
#define _WIN32
#endif   //!LINUX

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/errno.h>
#endif  //LINUX
#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif //  _WIN32



#define SERVER_PORTNUM 2020     /* Our server's port number */
#define MSGLEN         128      /* Size of ouer datagrams */
#define TICKET_AVAIL   0        /* Slot is available for use */
#define MAXUSERS       3        /* Only 3 users for us */
#define oops(x) {perror(x);exit(-1);}

 /***************************************
  *Important variables
  */
int ticket_array[MAXUSERS];
int sd = -1;
int num_tickets_out;
char* do_hello(char *msg_p);
char* do_goodbye(char *msg_p);

/**************************************
 *setup() - initialize license server
 */
setup()
{
    sd = make_dgram_server_socket();
    if (sd == -1)
        oops("make socket");
    free_all_tickets();
    return sd;
}
free_all_tickets()
{
    int i;
    for (i = 0; i < MAXUSERS; i++)
        ticket_array[i] = TICKET_AVAIL;
}
/*********************
 *shut_down() - close down license server
 */
shut_down()
{
    close(sd);
}

/**********************
 *handle_request(request,clientaddr,addrlen)
 *branch on code in request
 */
#ifdef __linux__
handle_request(char* req, struct sockaddr_in* client, socklen_t addlen)
#endif // __linux__
#ifdef _WIN32
handle_request(char* req, struct sockaddr_in* client, int addlen)
#endif // _WIN32
{
    char* response;
    int ret;

    /* act and compose a response */
    if (strncmp(req, "HELO", 4) == 0)
        response = do_hello(req);
    else if (strncmp(req, "GBYE", 4) == 0)
        response = do_goodbye(req);
    else
        response = "FAIL invalid request";
    /* send the response to the client */
    narrate("SAID:", response, client);
    ret = sendto(sd, response, strlen(response), 0, client, addlen);
    if (ret == -1)
        perror("SERVER sendto failed");
}

/******************************
 *do_hello
 *Give out a ticket if any are available
 *IN msg_p         message received from client
 *Results:ptr to response
 *Note:return is in static buffer overweitter by each call
 *NOTE:return is in static buffer overweitter by each call
 */
char* do_hello(char* msg_p)
{
    int x;
    static char replybuf[MSGLEN];

    if (num_tickets_out >= MAXUSERS)
        return("FAIL no tickets available");
    /* else find a free ticker and give it to client */
    for (x = 0; x < MAXUSERS && ticket_array[x] != TICKET_AVAIL; x++);

    /* A sanity check - should never happen */
    if (x == MAXUSERS) {
        narrate("database corrupt", "", NULL);
        return("FIIL database corrupt");
    }

    /* Found a free ticket.Record "name" of user(pid) in array.
     *generate ticket of form:pid.slot
     */
    ticket_array[x] = atoi(msg_p + 5); /* get pid in msg*/
    sprintf(replybuf, "TICK %d.%d", ticket_array[x], x);
    num_tickets_out++;
    return(replybuf);
}/* do_hello */

/******************
 *do_goodbye
 *Take back ticket client is returning
 *IN msg_p          message received from client
 *Results:ptr to response
 *Note:return is in static buff over written by each call
 */
char* do_goodbye(char* msg_p)
{
    int pid, slot;       /* components of ticket */

    /*The user's giving us back a ticket.First we need to get
     *the ticket out of message,which looks lisk
     *
     *GBYE pid.slot
     */
    if ((sscanf(msg_p + 5, "%d.%d", &pid, &slot) != 2) ||
        (ticket_array[slot] != pid)) {
        narrate("Bogus ticket", msg_p + 5, NULL);
        return("FAIL invalid ticket");
    }

    /* The ticket is valid.Release it. */
    ticket_array[slot] = TICKET_AVAIL;
    num_tickets_out--;

    /* Return response */
    return("THNX See ya!");
}/* do_goodbye */

/****************************
 *narrate() - chatty news for debugging and logging purposes
 */
narrate(char* msg1, char* msg2, struct sockaddr_in* clientp)
{
    fprintf(stderr, "\t\tSERVER: %s %s", msg1, msg2);
    if (clientp)
        fprintf(stderr, "(%s:%d)", inet_ntoa(clientp->sin_addr),
            ntohs(clientp->sin_port));
    putc('\n', stderr);
}
