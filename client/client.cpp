#include "client.h"

client::client(char * addr,int portnum,char *Password)
{
	password = Password;
	ticket_NO = -1;
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
#ifdef __linux__
	cmd_sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(addr);
	//serv_addr_cmd.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnum);
}

bool client::running()
{
	if (get_ticket()) {
		int conn_time = 0;//if connect failed,conn_time++,if conn_time>=3,APP exit;if connect valid,conn_time=0
		int second = 0;
		while (true) {
			if (second > 20) {
				second -= 20;
				if (confirm_connect())
					conn_time = 0;
				else conn_time++;
			}
			if (conn_time >= 3) {
				printf("connection with server lost.APP close.");
			}

#ifdef _WIN32
			if (_kbhit())//非阻塞获取用户输入
			{
				char temp = _getche();
				if (temp == 'Q' || temp == 'q')
					break;
			}
#endif

		}

		if (free_ticket())
			return true;

	}
	return false;
}

bool client::get_ticket()
{
	char msg[20];
	memset(msg, 0, 20);
	sprintf(msg, "HELO %s",password);
	int recv_len;
	sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, 20);
	while (true) {
		for (int i = 0; i < 10; i++) {
			if (recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &recv_len) != -1) {
				if (strncmp(msg, "TICK", 4) == 0) {
					ticket_NO = atoi(msg+5);
					printf("get ticket\n");
					return true;
				}
				else if (strncmp(msg, "FAIL", 4) == 0) {
					printf("%s\n",msg+5);
					return false;
				}
				else perror("received message wrong\n");
				return false;
			}
			Sleep(500);
		}
		printf("connect failed!trying to connect again...\n");
	}
}

bool client::free_ticket()
{
	char msg[20];
	memset(msg, 0, 20);
	sprintf(msg, "GBYE %d", ticket_NO);
	int recv_len;
	sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, 20);
	for (int i = 0; i < 10; i++)
		if (recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &recv_len) != -1) {
			printf("released ticket OK\n");
			ticket_NO = -1;
			return true;
		}
		else Sleep(200);
	printf("released ticket failed,APP exit.\n");
}

bool client::confirm_connect()
{
	char msg[20];
	memset(msg, 0, 20);
	sprintf(msg, "CONN %d", ticket_NO);
	int recv_len;
	sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, 20);

		for (int i = 0; i < 20; i++) {
			if (recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &recv_len) != -1) {
				if (strncmp(msg, "CONN", 4) == 0) {
					return true;
				}
				else if (strncmp(msg, "FAIL", 4) == 0) {
					printf("%s\n", msg + 5);
				}
				else perror("received message wrong");
				return false;
			}
			Sleep(200);
		}
		
	
	return false;
	
}