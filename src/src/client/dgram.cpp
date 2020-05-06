/********************************************
*	dgram.cpp
*	support fuctions for datagram based programs
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include <arpa/inet.h>
#include <netdb.h>
#endif

#define HOSTLEN 256

int make_internet_address();

int make_dgram_server_socket(int portnum)
{
	struct sockaddr_in saddr;	/*bulid our address here*/
	char hostname[HOSTLEN];		/*address*/
	int sock_id;				/*socket*/

	sock_id = socket(PF_INET, SOCK_DGRAM, 0);	/*get a socket*/
	if (sock_id == -1) return -1;

	/*build address and bind it to socket*/
	gethostname(hostname, HOSTLEN);
	make_internet_address(hostname, portnum, &saddr);
	if (bind(sock_id, (struct sockaddr*) & saddr, sizeof(saddr)) != 0)
		return -1;
	return sock_id;
}

int make_dgram_client_socket()
{
	return socket(PF_INET, SOCK_DGRAM, 0);
}

int make_internet_address(char* hostname, int port, struct sockaddr_in* addrp)
{
	/*
	*	constructor for an internet socket address,use hostname and port
	*	(host,port)->*addrp
	*/

	struct hostent* hp;
#ifdef __linux__
	bzero((void*)addrp, sizeof(struct sockaddr_in));
	hp = gethostbyname(hostname);
	if (hp == NULL) return -1;
	bcopy((void*)hp->h_addr, (void*)&addrp->sin_addr, hp->h_length);
	addrp->sin_port = htons(port);
	addrp->sin_family = AF_INET;
#endif

#ifdef _WIN32
	memset((void*)addrp, sizeof(struct sockaddr_in), 0);
	hp = gethostbyname(hostname);
	if (hp == NULL) return -1;
	/*maybe something wrong,maybe not*/
	strncpy((char *)hp->h_addr, (char *)&addrp->sin_addr, hp->h_length);
	addrp->sin_port = htons(port);
	addrp->sin_family = AF_INET;
#endif
	return 0;
}

int get_internet_address(char* host, int len, int* portp, struct sockaddr_in* addrp)
{
	/*
	*	extracts host and port form an internet socket address
	*	*addrp->(host,port)
	*/

	strncpy(host, inet_ntoa(addrp->sin_addr), len);
	*portp = ntohs(addrp->sin_port);
	return 0;
}