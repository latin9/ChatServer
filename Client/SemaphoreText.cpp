
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <WS2tcpip.h>

unsigned WINAPI ThreadSend(void* arg);
unsigned WINAPI ThreadRecv(void* arg);
void ErrorHandling(const char* message);

HANDLE hSem, hSem2;
int number = 0;

char thread1[] = "A Thread";
char thread2[] = "B Thread";
char thread3[] = "C Thread";

int main(int argc, char** argv)
{
	HANDLE hThread1, hThread2, hThread3;
	DWORD dwThreadID1, dwThreadID2, dwThreadID3;

	hSem = CreateSemaphore(NULL, 0, 1, NULL);	// 바이너리 세마포어 생성
	hSem2 = CreateSemaphore(NULL, 0, 1, NULL);

	if (hSem == NULL || hSem2 == NULL)
	{
		puts("세마포어 오브젝트 생성 실패");
		exit(1);
	}

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, ThreadSend, (void*)thread1, 0, (unsigned*)&dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, (void*)thread2, 0, (unsigned*)&dwThreadID2);
	hThread3 = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, (void*)thread3, 0, (unsigned*)&dwThreadID3);

	if (hThread1 == 0 || hThread2 == 0 || hThread3 == 0)
	{
		puts("쓰레드 생성 오류");
		exit(1);
	}

	if (WaitForSingleObject(hThread1, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 wait 오류");

	if (WaitForSingleObject(hThread2, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 wait 오류");

	if (WaitForSingleObject(hThread3, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 wait 오류");

	printf_s("최종 number : %d \n", number);

	CloseHandle(hSem);	// hSem세마포어 오브젝트 소멸
	CloseHandle(hSem2);	// hSem2세마포어 오브젝트 소멸

	return 0;
}

unsigned WINAPI ThreadSend(void* arg)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		number++;
		printf_s("실행 : %s, number : %d \n", (char*)arg, number);
		ReleaseSemaphore(hSem, 1, NULL);	// hSem 세마포어 1 증가
		WaitForSingleObject(hSem2, INFINITE); // hSem2 세마포어 1 감소
	}

	return 0;
}

unsigned WINAPI ThreadRecv(void* arg)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		WaitForSingleObject(hSem, INFINITE); // hSem 세마포어 1 감소
		number--;
		printf_s("실행 : %s, number : %d \n", (char*)arg, number);
		ReleaseSemaphore(hSem2, 1, NULL);	// hSem2 세마포어 1 증가
	}

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}