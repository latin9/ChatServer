
// 필요한 헤더 파일들
#include <stdio.h>          // 표준 입력/출력 헤더
#include <stdlib.h>         // 표준 라이브러리 헤더
#include <string.h>         // 문자열 처리 라이브러리
#include <WinSock2.h>       // 윈도우 소켓 2 헤더
#include <WS2tcpip.h>       // WinSock2에 추가 기능을 제공하는 헤더
//#include <unistd.h>
#pragma comment(lib, "ws2_32.lib")  // WinSock2를 위한 링커 지시문

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

	// 윈도우 소켓 프로그래밍을 할 때는 반드시 WSAStartup함수를 호출해 줘야 한다고함.
	// 해당 함수를 호출하는 목적은 프로그램에서 요구하는 윈도우 소켓의 버전을 알려줘서,
	// 해당 버전의 윈도우 소켓 사용을 위한 라이브러리 초기화 작업을 진행한다.
	// 이때 1번 인자는 프로그램에서 요구하는 윈속의 최상위 버전을 알려주기 위해 사용되는데
	// WORD는 16비트 unsigned int를 의미한다.
	// 상위 8비트에다가는 부 버전을, 하위 8비트에는 주 버전을 표시해주는데
	// 이를 매크로화 한게 MAKEWORD이다
	// MAKEWORD(2, 2)를 호출하면 0x0202가 리턴되어 들어간다.
	// Load Winsock 2.2 dll
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	// 서버 소켓 생성
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	// 소켓에 주소 할당 (전화기에 전화 번호를 할당하는 개념으로 생각하자)
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	// 연결 요청 대기 상태
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	// 연결 요청 수락
	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error");

	// 데이터 전송
	send(hClntSock, message, sizeof(message), 0);

	// 연결 종료
	closesocket(hClntSock);

	// 해당 함수 호출을 통해서 할당 받은 리소스를 해제하는 작업을 의미한다.
	WSACleanup();

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}