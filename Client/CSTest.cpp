

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <WS2tcpip.h>

unsigned WINAPI ThreadSummation(void* arg);
void ErrorHandling(const char* message);

int sum = 0;
int sum1[] = { 1, 5 };
int sum2[] = { 6, 10 };



CRITICAL_SECTION cs;

int main(int argc, char** argv)
{
	HANDLE hThread1, hThread2;
	DWORD dwThreadID1, dwThreadID2;

	// cs 초기화
	InitializeCriticalSection(&cs);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, ThreadSummation, (void*)sum1, 0, (unsigned*)&dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, ThreadSummation, (void*)sum2, 0, (unsigned*)&dwThreadID2);

	if (hThread1 == 0 || hThread2 == 0)
		ErrorHandling("쓰레드 생성 오류");

	if (WaitForSingleObject(hThread1, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 Wait 오류");

	if (WaitForSingleObject(hThread2, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 Wait 오류");

	// sum = 55;
	printf_s("main 함수 종료, sum = %d \n", sum);

	// cs 초멸
	DeleteCriticalSection(&cs);

	return 0;
}

unsigned WINAPI ThreadSummation(void* arg)
{
	int start = ((int*)arg)[0];
	int end = ((int*)arg)[1];

	// 전역 변수는 쓰레드들에 의해서 공유되는 변수이기 때문에
	// 해당 변수에 접근하는 코드는 임계 영역이 되게된다.
	for (; start <= end; start++)
	{
		// 임계 영역에 접근하기 전에는 CS를 얻어야 하고 임계 영역을 빠져나올 때 CS를 반환해야 한다.
		// 임계 영역으로의 접근은 한 번에 하나의 쓰레드에 의해서만 가능하도록 동기화 되어서
		// sum은 늘 55가 출력된다는 것을 보장 받게 된다.
		EnterCriticalSection(&cs);
		sum += start;
		LeaveCriticalSection(&cs);
	}

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}