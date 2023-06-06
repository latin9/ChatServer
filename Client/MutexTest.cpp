
// Mutex의 동기화 원리를 설명한 것이 아니라 커널 모드에서의 쓰레드 동기화 원리를 설명한 것?

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <WS2tcpip.h>


unsigned WINAPI ThreadIncrement(void* arg);
void ErrorHandling(const char* message);

char thread1[] = "A Thread";
char thread2[] = "B Thread";

int number = 0;

// Mutex 생성 시 전달되는 핸들을 저장하기 위한 변수
// 쓰레드들에 의해서 공유되어야 하므로(쓰레드 동기화를 위해서) 전역 변수로 선언되어 있다.
// 스택메모리 이외의 메모리들은 공유이기 때문
HANDLE hMutex;

int main(int argc, char** argv)
{
	HANDLE hThread1, hThread2;
	DWORD dwThreadID1, dwThreadID2;

	// Mutex를 생성
	// 생성된 Mutex는 소유자가 없는 signaled 상태의 커널 오브젝트이다.
	hMutex = CreateMutex(NULL, FALSE, NULL);

	if (hMutex == NULL)
	{
		puts("뮤텍스 오브젝트 생성 실패");
		exit(1);
	}

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, ThreadIncrement, (void*)thread1, 0, (unsigned*)&dwThreadID1);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, ThreadIncrement, (void*)thread2, 0, (unsigned*)&dwThreadID2);

	if (hThread1 == 0 || hThread2 == 0)
	{
		puts("쓰레드 생성 오류");
		exit(1);
	}

	if (WaitForSingleObject(hThread1, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 wait 오류");

	if (WaitForSingleObject(hThread2, INFINITE) == WAIT_FAILED)
		ErrorHandling("쓰레드 wait 오류");

	printf_s("최종 number : %d \n", number);

	// Mutex 오브젝트 소멸
	CloseHandle(hMutex);

	return 0;
}

unsigned WINAPI ThreadIncrement(void* arg)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		// Mutex를 얻는다
		WaitForSingleObject(hMutex, INFINITE);	// Mutex를 얻는다.
		// 아래부터 ReleaseMutex 전까지가 임계 영역이다
		// 공유되고 있는 자원인 number에 대해 접근하고 있기 때문
		// Sleep의 임계 영역에 포함시킨 이유는 실행 순서를 두 개의 쓰레드가 번갈아 가면서 실행하게 하기 위해서 라고 함. 
		Sleep(100);
		number++;
		// 아래 줄도 임계영역에 포함시킨 이유는 데이터를 출력 하는 과정에서도 그 값은 변경 될 수 있기 때문이다.
		printf_s("실행 : %s, number : %d \n", (char*)arg, number);
		// Mutex를 반환
		ReleaseMutex(hMutex);
	}

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}