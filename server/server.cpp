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

	int timeout = 1200;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

void server::running()
{
	setup();
	char msg[MSGLEN];
	memset(msg,0,MSGLEN);
	int nSize = sizeof(sockaddr);
	while (true) {
		int res = -1;
		while (res == -1)res = recvfrom(sock, msg, MSGLEN, 0, (struct sockaddr*) &last_client_addr, &nSize);
		if (res!=-1)
		{
			printf("get message:%s\n", msg);
			handle_request(msg);
			memset(msg, 0, MSGLEN);
		}

#ifdef _WIN32
		if (_kbhit())//非阻塞获取用户输入
		{
			char temp = _getche();
			if (temp == 'Q' || temp == 'q')
				break;
		}
#endif

		for (int i = 0; i < USERNUM; i++) {
			if ((clock() - last_conn_time[i])/CLOCKS_PER_SEC>60);
			{
				ticket[i] = false;
				printf("user %d lost connection,ticket take back\n", i);
				user--;
			}
		}
	}
}


bool server::setup()
{
	memset(password,0,PASSWORDLEN+1);
	if (!ReadLogFile()) {
		strncpy(password, randomPASSWORD(), PASSWORDLEN);
		for (int i = 0; i < USERNUM; i++)
			ticket[i] = false;
		printf("server start\n");
	}
	else {
		printf("server restart\n");
		for (int i = 0; i < USERNUM; i++)
			last_conn_time[i] = clock();
	}
	return true;
}

void server::handle_request(char* msg)
{
	if (strncmp(msg, "HELO", 4) == 0)
		do_hello(msg + 5);
	else if (strncmp(msg, "GBYE", 4) == 0)
		do_goodbye(msg + 5);
	else if (strncmp(msg, "CONN", 4) == 0)
		do_connect(msg + 5);

}

bool server::do_goodbye(char* msg)
{
	char back_msg[MSGLEN];
	memset(back_msg, 0, MSGLEN);
	int free_NO = atoi(msg);
	ticket[free_NO] = false;
	user--;
	sprintf(back_msg, "THNX thanks for using!");
	sendto(sock, back_msg, MSGLEN, 0, (struct sockaddr*) & last_client_addr, sizeof(last_client_addr));
	return true;
}

bool server::do_connect(char* msg)
{
	char back_msg[MSGLEN];
	memset(back_msg, 0, MSGLEN);
	int conn_NO = atoi(msg);
	last_conn_time[conn_NO] = clock();
	sprintf(back_msg, "CONN connecting with server");
	sendto(sock, back_msg, MSGLEN, 0, (struct sockaddr*) & last_client_addr, sizeof(last_client_addr));
	return true;
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
			int valid_ticket = 0;
			while (ticket[valid_ticket]) valid_ticket++;
			sprintf(back_msg, "TICK %d", valid_ticket);
			last_conn_time[user] = clock();
			user++;
		}
	}
	else sprintf(back_msg, "FAIL password wrong");
	sendto(sock,back_msg,MSGLEN,0,(struct sockaddr*)&last_client_addr,sizeof(last_client_addr));
	return true;
}

bool server::LogToFile()
{
	FILE* fp;
	if ((fp = fopen("Log.FLOG", "w")) != NULL)
	{
		for (unsigned int i = 0; i < PASSWORDLEN; i++)
			fprintf(fp, "%c", password[i]);
		fprintf(fp, "\n");
		for (int i = 0; i < USERNUM; i++)
			fprintf(fp, "%c", ticket[i] ? '1' : '0');
		fclose(fp);
		return true;
	}
	else
	{
		printf("open file error");
	}
	return false;
}
bool server::ReadLogFile()
{
	FILE* fp;
	if ((fp = fopen("Log.FLOG", "r")) != NULL)
	{
		if (!feof(fp))
		{
			fgets(password, PASSWORDLEN + 1, fp);
		}
		int i = 0;
		while (!feof(fp))
		{
			char tmp = fgetc(fp);
			if (tmp == '\n')
			{
				continue;
			}
			ticket[i] = tmp - '0';
			i++;
			if (i >= USERNUM)
			{
				break;
			}
		}
		return true;
		fclose(fp);
	}
	return false;
}

char* server::randomPASSWORD() {
	char p[PASSWORDLEN];
	srand((int)time(0));
		for (int i = 0; i < PASSWORDLEN; i++) {
			p[i] = rand() % 36;
			if (p[i] < 10) p[i] += 48;
			else p[i] += 55;
		}
	return p;
}