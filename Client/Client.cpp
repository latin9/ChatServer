
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <WinSock2.h>
#include <process.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 100
#define NAMESIZE 20
#define SERVER_IP "61.83.251.78"

unsigned WINAPI SendMSG(void* arg);
unsigned WINAPI RecvMSG(void* arg);
void ErrorHandling(const char* message);

char name[NAMESIZE] = "[Default]";
char message[BUFSIZE];

int main(int argc, char** argv)
{
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN servAddr;

	HANDLE hThread1, hThread2;
	DWORD dwThreadID1, dwThreadID2;

	if (argc != 4)
	{
		printf_s("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	sprintf_s(name, "[%s]", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, SendMSG, (void*)sock, 0, (unsigned*)&dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, RecvMSG, (void*)sock, 0, (unsigned*)&dwThreadID2);

	if (hThread1 == 0 || hThread2 == 0)
		ErrorHandling("쓰레드 생성 오류");


	WaitForSingleObject(hThread1, INFINITE);

	WaitForSingleObject(hThread2, INFINITE);

	closesocket(sock);

	return 0;
}

unsigned WINAPI SendMSG(void* arg)
{
	SOCKET sock = (SOCKET)arg;

	char nameMessage[NAMESIZE + BUFSIZE];

	while (1)
	{
		fgets(message, BUFSIZE, stdin);
		sprintf_s(nameMessage, "%s %s", name, message);

		if (!strcmp(message, "q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		send(sock, nameMessage, (int)strlen(nameMessage), 0);
	}
}

unsigned WINAPI RecvMSG(void* arg)
{
	SOCKET sock = (SOCKET)arg;
	char nameMessage[NAMESIZE + BUFSIZE];
	
	int strLen;
	
	while (1)
	{
		strLen = recv(sock, nameMessage, NAMESIZE + BUFSIZE - 1, 0);

		if (strLen == -1)
			return 1;

		nameMessage[strLen] = 0;

		fputs(nameMessage, stdout);
	}
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}