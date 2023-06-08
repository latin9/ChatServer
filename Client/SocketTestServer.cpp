
// �ʿ��� ��� ���ϵ�
#include <stdio.h>          // ǥ�� �Է�/��� ���
#include <stdlib.h>         // ǥ�� ���̺귯�� ���
#include <string.h>         // ���ڿ� ó�� ���̺귯��
#include <WinSock2.h>       // ������ ���� 2 ���
#include <WS2tcpip.h>       // WinSock2�� �߰� ����� �����ϴ� ���
//#include <unistd.h>
#pragma comment(lib, "ws2_32.lib")  // WinSock2�� ���� ��Ŀ ���ù�

#define PORT 4578

void ErrorHandling(const char* message);

int main()
{
	WSADATA		wsaData;
	SOCKET		hServSock;
	SOCKET		hClntSock;
	SOCKADDR_IN	servAddr;
	SOCKADDR_IN	clntAddr;
	int szClntAddr;
	const char message[] = "Hello World!\n";

	// ������ ���� ���α׷����� �� ���� �ݵ�� WSAStartup�Լ��� ȣ���� ��� �Ѵٰ���.
	// �ش� �Լ��� ȣ���ϴ� ������ ���α׷����� �䱸�ϴ� ������ ������ ������ �˷��༭,
	// �ش� ������ ������ ���� ����� ���� ���̺귯�� �ʱ�ȭ �۾��� �����Ѵ�.
	// �̶� 1�� ���ڴ� ���α׷����� �䱸�ϴ� ������ �ֻ��� ������ �˷��ֱ� ���� ���Ǵµ�
	// WORD�� 16��Ʈ unsigned int�� �ǹ��Ѵ�.
	// ���� 8��Ʈ���ٰ��� �� ������, ���� 8��Ʈ���� �� ������ ǥ�����ִµ�
	// �̸� ��ũ��ȭ �Ѱ� MAKEWORD�̴�
	// MAKEWORD(2, 2)�� ȣ���ϸ� 0x0202�� ���ϵǾ� ����.
	// Load Winsock 2.2 dll
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	// ���� ���� ����
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	// ���Ͽ� �ּ� �Ҵ� (��ȭ�⿡ ��ȭ ��ȣ�� �Ҵ��ϴ� �������� ��������)
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	// ���� ��û ��� ����
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	// ���� ��û ����
	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error");

	// ������ ����
	send(hClntSock, message, sizeof(message), 0);

	// ���� ����
	closesocket(hClntSock);

	// �ش� �Լ� ȣ���� ���ؼ� �Ҵ� ���� ���ҽ��� �����ϴ� �۾��� �ǹ��Ѵ�.
	WSACleanup();

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}