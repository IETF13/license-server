/*
*logfilec.c-logfile client-send message to the logfile server
*usage:logfilec"a message here"
*
*/
#ifndef __linux__
#define _WIN32
#endif   //!LINUX

#include <stdio.h>
#include <sys/types.h>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>
#endif  //LINUX
#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif //  _WIN32

#define SOCKET "/tmp/logfilesock"
#define oops(m,x) {perror(m);exit(x);}
int main(int ac, char*av[])
{
	int sock;
	struct sockaddr_un addr;
#ifdef __linux__
	socklen_t addrlen = sizeof(client_addr);
	int sock;
#endif // __linux__
#ifdef _WIN32
	int addrlen = sizeof(client_addr);
	SOCKET sock;
#endif
	char sockname[] = SOCKET;
	char *msg = av[1];
	if (ac != 2)
	{
		fprintf(stderr, "usage:logfilec 'message'\n");
		exit(1);
	}
	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock == -1)
		oops("socket", 2);
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockname);
	addrlen = strlen(sockname) + sizeof(addr.sun_family);
	if (sendto(sock, msq, strlen(msq), 0, &addr, addrlen) == -1)
		oops("sendto", 3);
}