#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include<ws2tcpip.h>
#include <winsock.h>
#include <windows.h>
#include <conio.h>//_kbhit()ะ่าช
#pragma comment (lib, "ws2_32.lib")
#endif

//LINUX INCLUDE
#ifdef __linux__
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#define MSGLEN 20
#define PASSWORDLEN 10
#define USERNUM 10

class server {
public:
	server(char* addr, int portnum);
	void running();
private:
	int user;
	bool ticket[USERNUM];
	int last_conn_time[USERNUM];
#ifdef _WIN32
	SOCKET sock;
#endif
#ifdef __linux__
	int sock;
#endif
	bool setup();
	bool restart();
	void handle_request(char * msg);
};