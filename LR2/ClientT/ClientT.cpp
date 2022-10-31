#include "pch.h"
#include <iostream>
#include "Winsock2.h"
#include <string>
#include "WS2tcpip.h"
#pragma comment(lib, "WS2_32.lib")

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

int main()
{
	setlocale(LC_ALL, "rus");
	SOCKET cC; //серверный сокет
	WSADATA wsaData;
	//const int k = 1000;
	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((cC = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;	//параметры сокета сервера
		serv.sin_family = AF_INET;	//используется IP-адресация
		serv.sin_port = htons(3000);	//TCP-порт 3000
		inet_pton(AF_INET, "127.0.0.1", &(serv.sin_addr)); //адрес сервера инт.внут. петли
		if ((connect(cC, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) //установить соединение с сокетом (дескр. сокета, указатель на сокаддр, длина в байтах)
			throw  SetErrorMsgText("connect:", WSAGetLastError());

		char ibuf[50], //буфер ввода
			obuf[50] = "Hello from client";  //буфер вывода
		int  libuf = 0, //кол-во принятых байт
			lobuf = 0;  //кол-во отправленных байт
		char b[] = "Hello from client ";

		int count;
		cout << "Enter count of messages" << endl;
		cin >> count;


		for (int i = 0; i < count; i++)
		{

			if (SOCKET_ERROR == send(cC, obuf, sizeof(obuf), NULL)) //отправить данные по установленному каналу
			{
				cout << "Send : " << GetLastError() << endl;;
			}

			if (SOCKET_ERROR == recv(cC, ibuf, sizeof(ibuf), NULL)) //получить данные по установленному каналу
			{
				cout << "Recv : " << GetLastError() << endl;
			}

			cout << (i + 1) << " Serv: " << ibuf << endl;
		}
		if (SOCKET_ERROR == send(cC, "CLOSE", sizeof("CLOSE"), NULL)) //отправить данные по установленному каналу
		{
			cout << "send exit : " << GetLastError() << endl;
		}

		if (closesocket(cC) == SOCKET_ERROR) //закрыть существующий сокет
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR) //завершить работу с библиотекой WS2_32.DLL
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;

	}
	system("pause");
	return 0;
}

string GetErrorMsgText(int code) // сформировать текст ошибки
{
	string msgText;
	switch (code) //проверка кода возврата
	{
	case WSAEINTR: msgText = "Работа функции прервана";
		break;
	case WSAEACCES: msgText = "Разрешение отвергнуто";
		break;
	case WSAEFAULT: msgText = "Ошибочный адрес";
		break;
	case WSASYSCALLFAILURE: msgText = "Аварийное завершение системного вызова";
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