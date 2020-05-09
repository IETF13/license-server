#include "server.h"

server::server(char* addr, int portnum)
{
	user = 0;
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
#endif
#ifdef __linux__
	cmd_sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	//memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//serv_addr.sin_addr.s_addr = inet_addr(addr);
	serv_addr.sin_addr.S_un.S_addr = inet_addr(addr);
	serv_addr.sin_port = htons(portnum);
	printf("server start\n");

	/*
	int on = 1;

#ifdef WIN32
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) == -1)
#endif
#ifdef __linux__
		if (setsockopt(cmd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
#endif
		{
			perror("setsockopt reuse failed!");
		}

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1)
	{
		perror("setsockopt timeout failed:");
	}

	char nochecksum = 0;
	setsockopt(sock,IPPROTO_UDP,UDP_NOCHECKSUM,& nochecksum, sizeof(nochecksum));
	*/
	if (bind(sock, (sockaddr*) & serv_addr, sizeof(sockaddr)) == -1)
	{
		perror("bind failed!");
	}
}

void server::running()
{
	setup();
	char msg[MSGLEN];
	memset(msg,0,MSGLEN);
	while (true) {
		int res = -1;
		int nSize = sizeof(sockaddr);
		while (res == -1)res = recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) &last_client_addr, &nSize);
		if (res!=-1)
		{
			printf("get message:%s\n", msg);
			handle_request(msg);

			memset(msg, 0, MSGLEN);
		}
	}
}

bool server::setup()
{
	strncpy(password, "12345ABCDE", 10);
	return true;
}

void server::handle_request(char* msg)
{
	if (strncmp(msg, "HELO", 4) == 0)
		do_hello(msg+5);
	else if (strncmp(msg, "GBYE", 4) == 0)
		;
	else if (strncmp(msg, "CONN", 4) == 0)
		;
}

bool server::do_hello(char * msg)
{
	char back_msg[MSGLEN];
	memset(back_msg, 0, MSGLEN);
	if (strncmp(password, msg, 10) == 0)
	{
		if (user == 10)
			sprintf(back_msg, "FAIL no available ticket");
		else {
			sprintf(back_msg, "TICK %d", user);
			last_conn_time[user] = clock();
			user++;
		}
	}
	else sprintf(back_msg, "FAIL password wrong");
	sendto(sock,back_msg,MSGLEN,0,(struct sockaddr*)&last_client_addr,sizeof(last_client_addr));
	return true;
}