
// �ʿ��� ��� ���ϵ�
#include <stdio.h>          // ǥ�� �Է�/��� ���
#include <stdlib.h>         // ǥ�� ���̺귯�� ���
#include <string.h>         // ���ڿ� ó�� ���̺귯��
#include <WinSock2.h>       // ������ ���� 2 ���
#include <WS2tcpip.h>       // WinSock2�� �߰� ����� �����ϴ� ���
//#include <unistd.h>
#pragma comment(lib, "ws2_32.lib")  // WinSock2�� ���� ��Ŀ ���ù�

#define PORT 4578
#define IP "172.20.10.2"

void ErrorHandling(const char* message);

int main()
{
	WSADATA wsaData;
	SOCKET hSocket;
	char message[30];
	int strLen = 0;
	SOCKADDR_IN servAddr;

	// ������ ���� ���α׷����� �� ���� �ݵ�� WSAStartup�Լ��� ȣ���� ��� �Ѵٰ���.
	// �ش� �Լ��� ȣ���ϴ� ������ ���α׷����� �䱸�ϴ� ������ ������ ������ �˷��༭,
	// �ش� ������ ������ ���� ����� ���� ���̺귯�� �ʱ�ȭ �۾��� �����Ѵ�.
	// �̶� 1�� ���ڴ� ���α׷����� �䱸�ϴ� ������ �ֻ��� ������ �˷��ֱ� ���� ���Ǵµ�
	// WORD�� 16��Ʈ unsigned int�� �ǹ��Ѵ�.
	// ���� 8��Ʈ���ٰ��� �� ������, ���� 8��Ʈ���� �� ������ ǥ�����ִµ�
	// �̸� ��ũ��ȭ �Ѱ� MAKEWORD�̴�
	// MAKEWORD(2, 2)�� ȣ���ϸ� 0x0202�� ���ϵǾ� ����.
	// Load WinSocket 2.2 DLL
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// ���� ������ ���� ���� ����
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("hSocket() error");

	// ���� �ּ� ����ü �ʱ�ȭ
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_pton(AF_INET, IP, &(servAddr.sin_addr.s_addr));
	servAddr.sin_port = htons(PORT);

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");


	// ������ ����
	strLen = recv(hSocket, message, sizeof(message) - 1, 0);
	if (strLen == -1)
		ErrorHandling("read() error");

	message[strLen] = 0;
	printf_s("Message from server : %s \n", message);

	closesocket(hSocket);	// ���� ����

	// �ش� �Լ� ȣ���� ���ؼ� �Ҵ� ���� ���ҽ��� �����ϴ� �۾��� �ǹ��Ѵ�.
	WSACleanup();

	return 0;
}