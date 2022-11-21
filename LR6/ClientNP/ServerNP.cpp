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
		//создать именованный канал
		//(симв имя канала, атрибуты канала, режимы передачи данных, макс кол-во экземпляров канала, размер вых буф, размер вход буф, время ожид связи с клиентом)
		// . - лок комп, pipe - фиксир слово
		if ((h_NP = CreateNamedPipe(L"\\\\.\\pipe\\Tube", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
		{
			throw "Error: CreateNamedPipe";
		}
		while (true)
		{
			//(дескриптор именованного канала, синхронная связь)
			if (!ConnectNamedPipe(h_NP, NULL)) //Соединить сервер с каналом 
			{ 
				throw "Error: ConnectNamedPipe";
			}
			//2
			//(дескр канала, указ на буфер ввода, кол-во читаемых байт, кол-во прочит байт, не исп синхр обраб)
			if (!ReadFile(h_NP, rbuf, sizeof(rbuf), &rb, NULL))  //Читать данные из канала
			{
				throw "Error: ReadFile";
			}

			cout << rbuf << endl;
			//3
			//(дескр канала, указ на буфер ввода, кол-во записываемых байт, кол-во записанных байт, не исп синхр обраб)
			if (!WriteFile(h_NP, wbuf, sizeof(wbuf), &wb, NULL))  //Писать данные в канал
			{
				throw "Error: WriteFile";
			}
			//4
			if (!DisconnectNamedPipe(h_NP))  //Закончить обмен данными
			{
				throw "Error: DisconnectNamedPipe";
			}
		}
		if (!CloseHandle(h_NP))  //закрыть канал(дескриптор)
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