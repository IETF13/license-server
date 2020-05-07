/*******************************
 *lserv1.c
 *License server server program version1
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
#include <sys/errno.h>
#endif  //LINUX
#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif //  _WIN32
#define MSGLEN 128  /* Size of our datagrams */

int main(int ac, char *av[])
{
	struct sockaddr_in client_addr;
#ifdef __linux__
	socklen_t addrlen = sizeof(client_addr);
	int sock;
#endif // __linux__
#ifdef _WIN32
	int addrlen = sizeof(client_addr);
	SOCKET sock;
#endif
	char buf[MSGLEN];
	int ret;
	int sock;
	void ticket_reclaim();
	unsigned time_left;
	sock = setup();
	signal(SIGALRM, ticket_reclaim);
	alarm(RECLAIM_INTERVAL);
	while (1) {
		addrlen = sizeof(client_addr);
		ret = recvfrom(sock, buf, MSGLEN, 0, &client_addr, &addrlen);
		if (ret != -1) {
			buf[ret] = '\0';
			narrate("GOT:", buf, &client_addr);
			time_left = alarm(0);
			handle_request(buf, &client_addr, addrlen);
			alarm(time_left);
		}
#ifdef __linux__
		else if (errno != EINTR)
			perror("recvfrom");
#endif
	}

}