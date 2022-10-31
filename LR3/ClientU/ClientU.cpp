#include <iostream>
#include <charconv>
#include <string>
#include "Winsock2.h"                // заголовок  WS2_32.dll
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll
#include "WS2tcpip.h"
#pragma warning(disable : 4996)


std::string  SetErrorMsgText(std::string msgText, int code);
std::string GetErrorMsgText(int code);

WSADATA wsaData;	//структура для хранения данных WSA
SOCKET cC;			//дескриптор сокета клиента
SOCKET cS;			//дескриптор сокета сервера
SOCKADDR_IN clnt;	//параметры сокета клиента
SOCKADDR_IN serv;	//параметры сокета сервера

char ibuf[50];
int libuf = 0, lobuf = 0;


int main()
{
	setlocale(LC_ALL, "Russian");
	std::cout << "Client Started!\n";

	serv.sin_family = AF_INET;						//использовать IP-адресацию
	serv.sin_port = htons(2000);					//порт 2000
	inet_pton(AF_INET, "127.0.0.1", &(serv.sin_addr)); //Server adress	

	memset(&clnt, 0, sizeof(clnt));
	int lenserv = sizeof(serv);
	//2 блок не содержит бинд

	try {

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw SetErrorMsgText("Sturtup:", WSAGetLastError());

		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)	//создание сокета, SOCK_DGRAM - ориентир на сообщения ЮДП
			throw SetErrorMsgText("socket:", WSAGetLastError());
		int j;
		std::cout << "Введите количество сообщений:";
		std::cin >> j;
		int intFromSrever = 0;
		for (int i = 0; i < j; i++)
		{
			char obuf[50] = "Hello from ClientU";
			char integer_char[5];
			sprintf(integer_char, "%d", intFromSrever);
			strcat(obuf, integer_char);
			
			if ((lobuf = sendto(cC, obuf, sizeof(obuf), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
				throw SetErrorMsgText("sendto:", WSAGetLastError());

			if (libuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)&serv, &lenserv) == SOCKET_ERROR)
				throw SetErrorMsgText("recvfrom:", WSAGetLastError());
			
			char charArrayNumber[5]="";
			strcat(charArrayNumber, &ibuf[18]);
			intFromSrever = atoi(charArrayNumber) + 1;

			Sleep(20);

			std::cout << "Serv: " << ibuf << std::endl;
		}

		if ((lobuf = sendto(cC, "", sizeof(""), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)	//Stop server message
        throw SetErrorMsgText("sendto:", WSAGetLastError());

		if (closesocket(cC) == SOCKET_ERROR)			//закрываем сокет
			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup", WSAGetLastError());
	}
	catch (std::string errorMsgText) { std::cout << std::endl << errorMsgText << "\n"; }
	system("pause");
	return 0;
}

std::string GetErrorMsgText(int code)    // cформировать текст ошибки 
{
	std::string msgText;
	switch (code)                      // проверка кода возврата  
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
	};
	return msgText;
};
std::string  SetErrorMsgText(std::string msgText, int code)
{
	return  msgText + "	 " + GetErrorMsgText(code);
};
