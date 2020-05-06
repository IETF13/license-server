/*
*	dgrecv2.cpp	datagram receiver
*				usage:dgrecv portnum
*				action: receives message,prints then,sends reply
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dgram.cpp"

#ifdef _WIN32
#include<ws2tcpip.h>
#include <winsock.h>
#pragma comment (lib, "ws2_32.lib")
#endif

//LINUX INCLUDE
#ifdef __linux__
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#define oops(m,x) {perror(m);exit(x);}
int make_dgram_server_socket(int);
int get_internet_address(char*, int, int*, struct sockaddr_in*);
void say_who_called(struct sockaddr_in*);
void reply_to_sender(int, char*, struct sockaddr_in *, socklen_t);

int mian(int ac, char* av[])
{
	int port;
	int sock;
	char buf[BUFSIZ];		/*to receive data here*/
	size_t msglen;			/*store its length here*/
	struct sockaddr_in saddr;	/*puts sender's address here*/
	socklen_t saddrlen;			/*and its length here*/

	if (ac == 1 || (port = atoi(av[1])) <= 0)
	{
		fprintf(stderr, "usage:dgrecv portnumber\n");
		exit(1);
	}

	/*get a socket and assign it a  port number*/
	if ((sock = make_dgram_server_socket(port)) == -1)
	{
		oops("cannot make socket", 2);
	}
	/*receive messages on that socket*/
	saddrlen = sizeof(saddr);
	while (msglen = recvfrom(sock, buf, BUFSIZ, 0, (sockaddr*)&saddr, &saddrlen) > 0)
	{
		buf[msglen] = '\0';
		printf("dgrecv:got a massage: %s\n", buf);
		say_who_called(&saddr);
		reply_to_sender(sock, buf, &saddr, saddrlen);
	}
	return 0;
}
void reply_to_sender(int sock, char* msg, struct sockaddr_in* addrp, socklen_t len)
{
	char reply[BUFSIZ + BUFSIZ];
	sprintf(reply, "Thanks for your %d char message\n", strlen(msg));
	sendto(sock, reply, strlen(reply), 0, (sockaddr*)addrp, len);
}

void say_who_called(struct sockaddr_in* addrp)
{
	char host[BUFSIZ];
	int port;
	get_internet_address(host, BUFSIZ, &port, addrp);
	printf("from: %s:%d\n", host, port);
}