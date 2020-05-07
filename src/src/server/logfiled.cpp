/*
*logfiled.c-a simple logfile server using Unix Domain Data Sockets
*usage:logfiled>>logfilename
*/

#ifndef __linux__
#define _WIN32
#endif   //!LINUX


#include <stdio.h>
#include <sys/types.h>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#endif  //LINUX
#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif //  _WIN32

#define MSGLEN 512
#define oops(m,x){perror(m);exit(x);}
#define SOCKNAME"/tmp/logfilesock"
int main(int ac, char *av[])
{
	int sock;/*read message here*/
	struct sockaddr_un addr;/*this is its address*/
#ifdef __linux__
	socklen_t addrlen = sizeof(client_addr);
	int sock;
#endif // __linux__
#ifdef _WIN32
	int addrlen = sizeof(client_addr);
	SOCKET sock;
#endif
	char msg[MSGLEN];
	int l;
	char sockname[] = SOCKNAME;
	time_t now;
	int msgnum = 0;
	char *timestr;
	/*bulid an address*/
	addr.sun_family = AF_UNIX;/*note AF_UNIX*/
	strcpy(addr.sun_path, sockname);/*filename is address*/
	addrlen = strlen(sockname) + sizeof(addr.sun_family);
	sock = socket(PF_UNIX, SOCK_DGRAM, 0);/*note PF_UNIX*/
	if (sock == -1)
		oops("socket", 2);
	/*bind the address*/
	if (bind(sock, (struct sockaddr*)&addr, addrlen) == -1);
	oops("bind", 3);
	/*read and write*/
	while (1)
	{
		l = read(sock, msg, MSGLEN);/*read works for DGRAM*/
		msg[1] = '\0';/*make it a string*/
		time(&now);
		timestr = ctime(&now);
		timestr[strlen(timestr) - 1] = '\0';/*chop newline*/
		printf("[%5d]%s %s\n", msgnum++, timestr, msg);
		fflush(stdout);
	}

}