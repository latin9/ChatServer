
// 필요한 헤더 파일들
#include <stdio.h>          // 표준 입력/출력 헤더
#include <stdlib.h>         // 표준 라이브러리 헤더
#include <string.h>         // 문자열 처리 라이브러리
#include <WinSock2.h>       // 윈도우 소켓 2 헤더
#include <WS2tcpip.h>       // WinSock2에 추가 기능을 제공하는 헤더
//#include <unistd.h>
#pragma comment(lib, "ws2_32.lib")  // WinSock2를 위한 링커 지시문

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

	// 윈도우 소켓 프로그래밍을 할 때는 반드시 WSAStartup함수를 호출해 줘야 한다고함.
	// 해당 함수를 호출하는 목적은 프로그램에서 요구하는 윈도우 소켓의 버전을 알려줘서,
	// 해당 버전의 윈도우 소켓 사용을 위한 라이브러리 초기화 작업을 진행한다.
	// 이때 1번 인자는 프로그램에서 요구하는 윈속의 최상위 버전을 알려주기 위해 사용되는데
	// WORD는 16비트 unsigned int를 의미한다.
	// 상위 8비트에다가는 부 버전을, 하위 8비트에는 주 버전을 표시해주는데
	// 이를 매크로화 한게 MAKEWORD이다
	// MAKEWORD(2, 2)를 호출하면 0x0202가 리턴되어 들어간다.
	// Load WinSocket 2.2 DLL
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 서버 접속을 위한 소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("hSocket() error");

	// 서버 주소 구조체 초기화
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_pton(AF_INET, IP, &(servAddr.sin_addr.s_addr));
	servAddr.sin_port = htons(PORT);

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");


	// 데이터 수신
	strLen = recv(hSocket, message, sizeof(message) - 1, 0);
	if (strLen == -1)
		ErrorHandling("read() error");

	message[strLen] = 0;
	printf_s("Message from server : %s \n", message);

	closesocket(hSocket);	// 연결 종료

	// 해당 함수 호출을 통해서 할당 받은 리소스를 해제하는 작업을 의미한다.
	WSACleanup();

	return 0;
}