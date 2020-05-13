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
	//serv_addr.sin_addr.s_addr = inet_addr(addr);
	serv_addr.sin_addr.S_un.S_addr = inet_addr(addr);
	serv_addr.sin_port = htons(portnum);

	int timeout = 1200;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout ,sizeof(timeout));
}

bool client::running()
{
	if (get_ticket()) {
		printf("APP start running...\n");
		printf("Input Q or q to quit\n");
		//int disconn_times = 0;//if connect failed,disconn_times++,if conn_time>=3,APP exit;if connect valid,disconn_times=0
		clock_t last_conn_time=clock();
		while (true) {
			if ((clock()-last_conn_time)/ CLOCKS_PER_SEC >20) {
				last_conn_time = clock();
				if (!confirm_connect())
					printf("connect with server failed.Maybe server crashed.Waiting for server restart...\n");
				
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
	char msg[MSGLEN];
	memset(msg, 0, MSGLEN);
	int addr_len=sizeof(sockaddr);
	while (true) {
	sprintf(msg, "HELO %s", password);
	sendto(sock, msg, MSGLEN, 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, MSGLEN);
		for (int i = 0; i < 5; i++) {
	int ret = recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &addr_len);
			if ( ret!= -1) {
				if (strncmp(msg, "TICK", 4) == 0) {
					ticket_NO = atoi(msg+5);
					printf("get ticket %d\n",ticket_NO);
					return true;
				}
				else if (strncmp(msg, "FAIL", 4) == 0) {
					printf("%s\n",msg+5);
					return false;
				}
				else perror("received message wrong\n");
				return false;
			}
		}
		printf("connect failed!trying to connect again...\n");
			
	}
}

bool client::free_ticket()
{
	char msg[MSGLEN];
	memset(msg, 0, MSGLEN);
	sprintf(msg, "GBYE %d", ticket_NO);
	int addr_len=sizeof(sockaddr);
	sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, MSGLEN);
	for (int i = 0; i <3; i++)
		if (recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &addr_len) != -1) {
			printf("\nreleased ticket OK.APP exit.\n");
			ticket_NO = -1;
			return true;
		}
	printf("\nreleased ticket failed,APP exit.\n");
}

bool client::confirm_connect()
{
	char msg[MSGLEN];
	memset(msg, 0, MSGLEN);
	sprintf(msg, "CONN %d", ticket_NO);
	int addr_len=sizeof(sockaddr);
	sendto(sock, msg, MSGLEN, 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
	memset(msg, 0, MSGLEN);

		for (int i = 0; i < 5; i++) {
			if (recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) & serv_addr, &addr_len) != -1) {
				if (strncmp(msg, "CONN", 4) == 0) {
					printf("connect with server succeed\n");
					return true;
				}
				else if (strncmp(msg, "FAIL", 4) == 0) {
					printf("%s\n", msg + 5);
				}
				else perror("received message wrong\n");
				return false;
			}
		}
		
	
	return false;
	
}