//1
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <string>
#include "Winsock2.h" //заголовок WS2_32.dll
#pragma comment(lib, "WS2_32.lib") //экспорт WS2_32.dll
#include <ctime>


using namespace std;

string GetErrorMsgText(int code) //сформировать текст ошибки
{
	string msgText;
	switch (code) //проверка кода возврата
	{
	case WSAEINTR: msgText = "–абота функции прервана";
		break;
	case WSAEACCES: msgText = "–азрешение отвергнуто";
		break;
	case WSAEFAULT: msgText = "ќшибочный адрес";
		break;
	case WSASYSCALLFAILURE: msgText = "јварийное завершение системного вызова";
		break;
	default: msgText = "***ERROR***";
		break;
	}

	return msgText;
}

string SetErrorMsgText(string msgText, int code) //получение ошибки
{
	return msgText + GetErrorMsgText(code);
}


int main()
{
	setlocale(LC_ALL, "rus");
	cout << "Server is started" << endl;
	cout << "Server is waiting connections" << endl;
	int i = 0;	//кол-во клиентов
	WSADATA ws;
	SOCKET s; //дескриптор сокета сервера
	SOCKET c; //дескриптор сокета клиента
	char ibuf[50];	//дл€ записи сообщений от клиентов
	char obuf[50] = "ECHO: Hello from Client";	//отклик дл€ клиента
	int t;

	//1
	try {
		if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws))) //инициализировать библиотеку WS2_32.DLL (верси€ виндов сокет, указатель на wsadata)
		{
			cout << "Socket: " << WSAGetLastError() << endl; //ѕолучить диагностирующий код ошибки
		}
		//2 soket(формат адреса, тип сокета, протокол)
		if (INVALID_SOCKET == (s = socket(AF_INET, SOCK_STREAM, 0))) //создание сокета, SOCK_STREAM указывает, что сокет ориентированный на поток
		{
			cout << "Socket: " << WSAGetLastError() << endl; //ѕолучить диагностирующий код ошибки
		}
		sockaddr_in c_adr;	//порт клиента
		sockaddr_in s_adr;	//порт сервера
		{
			s_adr.sin_port = htons(3000); //преобразовать u_short в формат TCP/IP 3000 - номер порта
			s_adr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			s_adr.sin_family = AF_INET; //дл€ IPv4
		}
		//(сокет, указатель на sockaddr_in, длина в байтах)
		if (SOCKET_ERROR == (bind(s, (LPSOCKADDR)&s_adr, sizeof(s_adr)))) //св€зать сокет с параметрами (SOCKADDR хранит параметры)
		{
			cout << "Bind: " << WSAGetLastError() << endl;
		}

		//3 (дескриптор сокета, макс длина очереди)
		if (SOCKET_ERROR == listen(s, 2)) //переключить сокет в режим прослушивани€ 
		{
			cout << "Listen: " << WSAGetLastError << endl;
		}

		while (true)
		{
			int lcInt = sizeof(c_adr);
			//разрешить подключение к сокету (переводит процесс сервера в состо€ние ожидани€, пока программа не выполнит коннект)
			if (INVALID_SOCKET == (c = accept(s, (sockaddr*)&c_adr, &lcInt))) //(дескр скоета, указатель на сокаддр, указатель на длину сокаддр)
			{
				cout << "Accept: " << WSAGetLastError() << endl;
			}

			cout << "Client connect" << endl;
			cout << "ADDRES client : " << inet_ntoa(c_adr.sin_addr) << " : " << htons(c_adr.sin_port) << endl << endl << endl << endl << endl;
			//4, 5 
			while (true)
			{
				//(дескр сокета, указатель на буфер данных, кол-во байт в буфере, индикатор)
				if (SOCKET_ERROR == recv(c, ibuf, sizeof(ibuf), NULL)) //прин€ть данные по установленному каналу
				{
					cout << "Recv: " << WSAGetLastError() << endl;
					break;
				}

				cout << i << " Client : " << ibuf << endl;
				i++;

				if (!strcmp(ibuf, "CLOSE")) { break; }
				//(дескр сокета, указатель на буфер данных, кол-во байт данных в буфере, индикатор особого режима маршрутиз)
				if (SOCKET_ERROR == send(c, obuf, strlen(obuf) + 1, NULL)) //отправить данные по установленному каналу
				{
					cout << "Send: " << WSAGetLastError() << endl;
					break;
				}
			}
			i = 0;
			cout << "\t\tClient Diskonect" << endl;
		}
		//(сокет)
		if (closesocket(c) == SOCKET_ERROR)  //закрыть существующий сокет
		{
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		}

		if (WSACleanup() == SOCKET_ERROR) //завершить работу с библиотекой WS2_32.DLL
		{
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
		}
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}

	return 0;
}