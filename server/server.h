#pragma once
#include "login.h"
#include <cstdio>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <ws2tcpip.h>
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
#define USERNUM 10

class server {
public:
	server(char* addr, int portnum);
	void running();
private:
	int user;
	char password[PASSWORDLEN+1];
	bool ticket[USERNUM];
	clock_t last_conn_time[USERNUM];
	sockaddr_in serv_addr;
	sockaddr_in last_client_addr;
#ifdef _WIN32
	SOCKET sock;
#endif
#ifdef __linux__
	int sock;
#endif
	bool setup();
	void handle_request(char * msg);
	bool do_hello(char * msg);
	bool do_goodbye(char* msg);
	bool do_connect(char* msg);
	bool LogToFile();
	char * randomPASSWORD();
	bool ReadLogFile();
};