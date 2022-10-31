#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
//1
#include <iostream>
#include <string>
#include "Winsock2.h"                // заголовок  WS2_32.dll
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll

std::string  SetErrorMsgText(std::string msgText, int code);
std::string GetErrorMsgText(int code);


WSADATA wsaData;	//структура для хранения данных о WSA
SOCKET cS;			//дескриптор сокета клиента
SOCKET sS;			//дескриптор сокета сервера
SOCKADDR_IN serv;	//параметры сокета сервера
SOCKADDR_IN clnt;	//параметры сокета клиента

char ibuf[50], obuf[50];
int libuf = 0, lobuf = 0;

clock_t start, stop;


int main()
{
	setlocale(LC_ALL, "Russian");
	std::cout << "Server Started!\n";

	serv.sin_family = AF_INET;				//использовать IP-адресацию
	serv.sin_port = htons(2000);			//порт 2000
	serv.sin_addr.S_un.S_addr = INADDR_ANY;	//любой собственный IP-адрес

	memset(&clnt, 0, sizeof(clnt));
	int lenclnt = sizeof(clnt);

	try
	{
		//1
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //инициализировать библиотеку WS2_32.DLL (версия виндов сокет, указатель на wsadata)
			throw SetErrorMsgText("Sturtup:", WSAGetLastError());
		//2
		//soket(формат адреса, тип сокета, протокол)
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)	//создание UDP сокета, SOCK_DGRAM - ориентир на сообщения ЮДП
			throw SetErrorMsgText("socket:", WSAGetLastError());

		//(сокет, указатель на sockaddr_in, длина в байтах) связать сокет с параметрами (SOCKADDR хранит параметры)
		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)//бинд нового сокета на 2000 TCP
			throw SetErrorMsgText("bind:", WSAGetLastError());

		//netstat -s
		//netstat -ab
		start = clock();
		while (true)
		{
			if (ibuf == "")	break;
			//3
			//(дескриптор сокета, буфер полученных данных, размер буфера, индикатор режима маршрутизации, указатель на сокадр, указатель на размер то)
			if (libuf = recvfrom(sS, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &lenclnt) == SOCKET_ERROR) //получение сообщения без установления соединения
				throw SetErrorMsgText("recvfrom:", WSAGetLastError());
			//4
			//(дескриптор сокета, буфер для пересылки данных, размер буфера, индикатор режима маршрутизации, указ на сокадр, длина структуры )
			if ((lobuf = sendto(sS, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, sizeof(clnt))) == SOCKET_ERROR) //отправка сообщения без установления соединения
				throw SetErrorMsgText("sendto:", WSAGetLastError());


			std::cout << ibuf << "\n";
		}
		stop = clock();
		std::cout << stop - start;
	}
	catch (std::string errorMsgText) { std::cout << std::endl << errorMsgText; }

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

