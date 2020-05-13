#pragma once
#include <cstdio>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include<ws2tcpip.h>
#include <winsock.h>
#include <windows.h>
#include <conio.h>//_kbhit()��Ҫ
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

#define MSGLEN 1024
#define PASSWORDLEN 10

class client {
public:
	client(char* addr, int portnum, char* pass);
	bool running();

private:

	int ticket_NO; //-1 means no ticket
	sockaddr_in serv_addr;
	char *password;
#ifdef _WIN32
	SOCKET sock;
#endif
#ifdef __linux__
	int sock;
#endif
	bool get_ticket();
	bool free_ticket();
	bool confirm_connect();
};