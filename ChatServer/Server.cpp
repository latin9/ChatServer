// �ʿ��� ��� ���ϵ�
#include <stdio.h>          // ǥ�� �Է�/��� ���
#include <stdlib.h>         // ǥ�� ���̺귯�� ���
#include <string.h>         // ���ڿ� ó�� ���̺귯��
#include <WinSock2.h>       // ������ ���� 2 ���
#include <process.h>        // ������ ���� �� ������ ���� ���
#include <WS2tcpip.h>       // WinSock2�� �߰� ����� �����ϴ� ���
#pragma comment(lib, "ws2_32.lib")  // WinSock2�� ���� ��Ŀ ���ù�

#define BUFSIZE 100  // ���� ũ�⸦ ����
#define PORT 4578    // ��Ʈ ��ȣ�� ����

// �Լ� ����
unsigned  WINAPI ClientConn(void* arg);  // Ŭ���̾�Ʈ ������ ���� �Լ�
void SendMSG(char* message, int len);  // �޽��� ������ ���� �Լ�
void ErrorHandling(const char* message);  // ���� ó���� ���� �Լ�

// ���� ����
int clntNumber = 0;  // ����� Ŭ���̾�Ʈ ��
SOCKET clntSocks[10];  // ����� Ŭ���̾�Ʈ ������ ������ �迭
HANDLE hMutex;  // ������ �� ������ ����ȭ�� ���� ���ؽ� �ڵ�

int main()  // ���� �Լ�
{
	// ���� ����
	WSADATA wsaData;  // WinSock ������ ���� ����ü
	SOCKET servSock;  // ���� ����
	SOCKET clntSock;  // Ŭ���̾�Ʈ ����

	SOCKADDR_IN servAddr;  // ���� �ּ� ����ü
	SOCKADDR_IN clntAddr;  // Ŭ���̾�Ʈ �ּ� ����ü
	int clntAddrSize;  // Ŭ���̾�Ʈ �ּ� ����ü�� ũ��

	HANDLE hThread;  // ������ �ڵ�
	DWORD dwThreadID;  // ������ ID

	// WinSock �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// ���ؽ� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
		ErrorHandling("CreateMutex() error");

	// ���� ����
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)  // ���� ���� ���н� ���� ó��
		ErrorHandling("socket() error");

	// ���� �ּ� ����ü �ʱ�ȭ
	// memset�� �ϴ� ������ ��� ����� 0���� �ʱ�ȭ ���־� ������ ���� ���� ���� �ʵ��� �ϱ� �����̴�.
	// �̷��� �ϸ�, ������ �ʴ� �����ͷ� ���� �߻��� �� �ִ� ����ġ ���� ������ ���� �� �ִ�.
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;  // �ּ�ü�� IPv4
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // ��� Ŭ���̾�Ʈ�� ������ ���
	servAddr.sin_port = htons(PORT);  // ��Ʈ ��ȣ ����

	// ���Ͽ� �ּ� ���ε�
	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	// ���� ��û �����·� ��
	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	while (1)  // ���ѷ����� ���� ���� Ŭ���̾�Ʈ�� ���� ��û�� ��� ������
	{
		clntAddrSize = sizeof(clntAddr);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &clntAddrSize);  // Ŭ���̾�Ʈ�� ���� ��û ����

		if (clntSock == INVALID_SOCKET)  // ���� ���� ���н� ���� ó��
			ErrorHandling("accept() error");

		// ���ؽ��� ���� ����ȭ ����
		WaitForSingleObject(hMutex, INFINITE);
		clntSocks[clntNumber++] = clntSock;  // ����� Ŭ���̾�Ʈ�� ������ ����
		ReleaseMutex(hMutex);  // ���ؽ��� ���� ����ȭ ����

		// ���ο� Ŭ���̾�Ʈ�� IP �ּ� ���
		char clientIP[20] = { 0, };
		printf_s("���ο� ����, Ŭ���̾�Ʈ IP : %s \n", inet_ntop(AF_INET, &clntAddr.sin_addr, clientIP, sizeof(clientIP)));

		// ���ο� Ŭ���̾�Ʈ�� ���� ������ ����
		hThread = (HANDLE)_beginthreadex(NULL, 0, ClientConn, (void*)clntSock, 0, (unsigned*)&dwThreadID);
		if (hThread == 0)  // ������ ���� ���н� ���� ó��
			ErrorHandling("������ ���� ����");
	}

	// WinSock ����
	WSACleanup();

	return 0;
}

unsigned  WINAPI ClientConn(void* arg)  // Ŭ���̾�Ʈ���� ���� ó���� ���� �Լ�
{
	SOCKET clntSock = (SOCKET)arg;
	int strLen = 0;
	char message[BUFSIZE];
	int i;

	// Ŭ���̾�Ʈ�κ��� �޽����� �޾� �ٸ� Ŭ���̾�Ʈ���� ����
	while ((strLen = recv(clntSock, message, BUFSIZE, 0)) != 0)
		SendMSG(message, strLen);

	// ���ؽ��� ���� ����ȭ ����
	WaitForSingleObject(hMutex, INFINITE);

	// ������ ����� Ŭ���̾�Ʈ ����
	for (i = 0; i < clntNumber; i++)
	{
		if (clntSock == clntSocks[i])
		{
			for (; i < clntNumber - 1; i++)
				clntSocks[i] = clntSocks[i + 1];  // Ŭ���̾�Ʈ ���� �迭 ������
			break;
		}
	}

	clntNumber--;  // ����� Ŭ���̾�Ʈ �� ����
	ReleaseMutex(hMutex);  // ���ؽ��� ���� ����ȭ ����

	closesocket(clntSock);  // Ŭ���̾�Ʈ ���� ����
	return 0;
}

void SendMSG(char* message, int len)  // ��� Ŭ���̾�Ʈ���� �޽����� �����ϴ� �Լ�
{
	int i;

	// ���ؽ��� ���� ����ȭ ����
	WaitForSingleObject(hMutex, INFINITE);

	// ��� Ŭ���̾�Ʈ���� �޽��� ����
	for (i = 0; i < clntNumber; i++)
		send(clntSocks[i], message, len, 0);

	ReleaseMutex(hMutex);  // ���ؽ��� ���� ����ȭ ����
}

void ErrorHandling(const char* message)  // ���� ó�� �Լ�
{
	fputs(message, stderr);  // ���� �޽��� ���
	fputc('\n', stderr);  // ���� ���� ���
	exit(1);  // ���α׷� ����
}