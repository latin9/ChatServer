
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <WS2tcpip.h>


unsigned WINAPI ThreadFunction(void* arg);

int main(int argc, char** argv)
{
	HANDLE hThread;
	DWORD dwThreadID;
	DWORD dw;

	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, NULL, 0, (unsigned*)&dwThreadID);

	if (hThread == 0)
	{
		puts("_beginthreadex() error");
		exit(1);
	}

	printf_s("������ �������� �ڵ� : %d \n", hThread);
	printf_s("������ �������� ID : %d \n", dwThreadID);
	Sleep(3000);

	// ������ ���� �ñ��� MAIN �Լ��� ������ ����
	dw = WaitForSingleObject(hThread, INFINITE);

	if (dw == WAIT_FAILED)
	{
		puts("������ wait ����");
		exit(1);
	}

	printf_s("main �Լ� ����, %s ����\n", dw == WAIT_OBJECT_0 ? "����" : "������");

	return 0;
}

unsigned WINAPI ThreadFunction(void* arg)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		Sleep(2000);
		puts("������ ���� ��");
	}

	return 0;
}