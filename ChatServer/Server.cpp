// 필요한 헤더 파일들
#include <stdio.h>          // 표준 입력/출력 헤더
#include <stdlib.h>         // 표준 라이브러리 헤더
#include <string.h>         // 문자열 처리 라이브러리
#include <WinSock2.h>       // 윈도우 소켓 2 헤더
#include <process.h>        // 스레드 생성 및 관리를 위한 헤더
#include <WS2tcpip.h>       // WinSock2에 추가 기능을 제공하는 헤더
#pragma comment(lib, "ws2_32.lib")  // WinSock2를 위한 링커 지시문

#define BUFSIZE 100  // 버퍼 크기를 정의
#define PORT 4578    // 포트 번호를 정의

// 함수 선언
unsigned  WINAPI ClientConn(void* arg);  // 클라이언트 연결을 위한 함수
void SendMSG(char* message, int len);  // 메시지 전송을 위한 함수
void ErrorHandling(const char* message);  // 에러 처리를 위한 함수

// 전역 변수
int clntNumber = 0;  // 연결된 클라이언트 수
SOCKET clntSocks[10];  // 연결된 클라이언트 소켓을 저장할 배열
HANDLE hMutex;  // 스레드 간 데이터 동기화를 위한 뮤텍스 핸들

int main()  // 메인 함수
{
	// 변수 선언
	WSADATA wsaData;  // WinSock 시작을 위한 구조체
	SOCKET servSock;  // 서버 소켓
	SOCKET clntSock;  // 클라이언트 소켓

	SOCKADDR_IN servAddr;  // 서버 주소 구조체
	SOCKADDR_IN clntAddr;  // 클라이언트 주소 구조체
	int clntAddrSize;  // 클라이언트 주소 구조체의 크기

	HANDLE hThread;  // 스레드 핸들
	DWORD dwThreadID;  // 스레드 ID

	// WinSock 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 뮤텍스 생성
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
		ErrorHandling("CreateMutex() error");

	// 소켓 생성
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)  // 소켓 생성 실패시 에러 처리
		ErrorHandling("socket() error");

	// 서버 주소 구조체 초기화
	// memset을 하는 이유는 모든 멤버를 0으로 초기화 해주어 쓰레기 값이 남아 있지 않도록 하기 위함이다.
	// 이렇게 하면, 원하지 않는 데이터로 인해 발생할 수 있는 예기치 않은 동작을 막을 수 있다.
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;  // 주소체계 IPv4
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 모든 클라이언트의 연결을 허용
	servAddr.sin_port = htons(PORT);  // 포트 번호 설정

	// 소켓에 주소 바인딩
	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	// 연결 요청 대기상태로 들어감
	if (listen(servSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	while (1)  // 무한루프를 통해 여러 클라이언트의 연결 요청을 계속 수락함
	{
		clntAddrSize = sizeof(clntAddr);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &clntAddrSize);  // 클라이언트의 연결 요청 수락

		if (clntSock == INVALID_SOCKET)  // 연결 수락 실패시 에러 처리
			ErrorHandling("accept() error");

		// 뮤텍스를 통한 동기화 시작
		WaitForSingleObject(hMutex, INFINITE);
		clntSocks[clntNumber++] = clntSock;  // 연결된 클라이언트의 소켓을 저장
		ReleaseMutex(hMutex);  // 뮤텍스를 통한 동기화 종료

		// 새로운 클라이언트의 IP 주소 출력
		char clientIP[20] = { 0, };
		printf_s("새로운 연결, 클라이언트 IP : %s \n", inet_ntop(AF_INET, &clntAddr.sin_addr, clientIP, sizeof(clientIP)));

		// 새로운 클라이언트를 위한 스레드 생성
		hThread = (HANDLE)_beginthreadex(NULL, 0, ClientConn, (void*)clntSock, 0, (unsigned*)&dwThreadID);
		if (hThread == 0)  // 스레드 생성 실패시 에러 처리
			ErrorHandling("쓰레드 생성 오류");
	}

	// WinSock 종료
	WSACleanup();

	return 0;
}

unsigned  WINAPI ClientConn(void* arg)  // 클라이언트와의 연결 처리를 위한 함수
{
	SOCKET clntSock = (SOCKET)arg;
	int strLen = 0;
	char message[BUFSIZE];
	int i;

	// 클라이언트로부터 메시지를 받아 다른 클라이언트에게 전송
	while ((strLen = recv(clntSock, message, BUFSIZE, 0)) != 0)
		SendMSG(message, strLen);

	// 뮤텍스를 통한 동기화 시작
	WaitForSingleObject(hMutex, INFINITE);

	// 연결이 종료된 클라이언트 제거
	for (i = 0; i < clntNumber; i++)
	{
		if (clntSock == clntSocks[i])
		{
			for (; i < clntNumber - 1; i++)
				clntSocks[i] = clntSocks[i + 1];  // 클라이언트 소켓 배열 재정렬
			break;
		}
	}

	clntNumber--;  // 연결된 클라이언트 수 감소
	ReleaseMutex(hMutex);  // 뮤텍스를 통한 동기화 종료

	closesocket(clntSock);  // 클라이언트 소켓 종료
	return 0;
}

void SendMSG(char* message, int len)  // 모든 클라이언트에게 메시지를 전송하는 함수
{
	int i;

	// 뮤텍스를 통한 동기화 시작
	WaitForSingleObject(hMutex, INFINITE);

	// 모든 클라이언트에게 메시지 전송
	for (i = 0; i < clntNumber; i++)
		send(clntSocks[i], message, len, 0);

	ReleaseMutex(hMutex);  // 뮤텍스를 통한 동기화 종료
}

void ErrorHandling(const char* message)  // 에러 처리 함수
{
	fputs(message, stderr);  // 에러 메시지 출력
	fputc('\n', stderr);  // 개행 문자 출력
	exit(1);  // 프로그램 종료
}