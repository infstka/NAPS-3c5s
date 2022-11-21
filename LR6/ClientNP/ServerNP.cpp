#include "windows.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
	setlocale(LC_CTYPE, "Russian");

	cout << "ServerNP" << endl << endl;

	try
	{
		HANDLE h_NP;

		char rbuf[50];
		char wbuf[50] = "Hello from server";
		DWORD rb = sizeof(rbuf);
		DWORD wb = sizeof(wbuf);
		//1
		//������� ����������� �����
		//(���� ��� ������, �������� ������, ������ �������� ������, ���� ���-�� ����������� ������, ������ ��� ���, ������ ���� ���, ����� ���� ����� � ��������)
		// . - ��� ����, pipe - ������ �����
		if ((h_NP = CreateNamedPipe(L"\\\\.\\pipe\\Tube", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
		{
			throw "Error: CreateNamedPipe";
		}
		while (true)
		{
			//(���������� ������������ ������, ���������� �����)
			if (!ConnectNamedPipe(h_NP, NULL)) //��������� ������ � ������� 
			{ 
				throw "Error: ConnectNamedPipe";
			}
			//2
			//(����� ������, ���� �� ����� �����, ���-�� �������� ����, ���-�� ������ ����, �� ��� ����� �����)
			if (!ReadFile(h_NP, rbuf, sizeof(rbuf), &rb, NULL))  //������ ������ �� ������
			{
				throw "Error: ReadFile";
			}

			cout << rbuf << endl;
			//3
			//(����� ������, ���� �� ����� �����, ���-�� ������������ ����, ���-�� ���������� ����, �� ��� ����� �����)
			if (!WriteFile(h_NP, wbuf, sizeof(wbuf), &wb, NULL))  //������ ������ � �����
			{
				throw "Error: WriteFile";
			}
			//4
			if (!DisconnectNamedPipe(h_NP))  //��������� ����� �������
			{
				throw "Error: DisconnectNamedPipe";
			}
		}
		if (!CloseHandle(h_NP))  //������� �����(����������)
		{
			throw "Eroor: CloseHandle"; 
		}
	}
	catch (string e)
	{
		cout << e << endl;
	}
	catch (...)
	{
		cout << "Error" << endl;
	}

	system("pause");
	return 0;
}