#include "pch.h"
#include <iostream>
#include <string>
#include <tchar.h>
#include "Winsock2.h"
#pragma warning (disable : 4996)
#pragma comment(lib, "WS2_32.lib")

SOCKET cC;

std::string  GetErrorMsgText(int code)
{
	switch (code)
	{
	case WSAEINTR: return "Работа функции прервана ";
	case WSAEACCES: return "Разрешение отвергнуто";
	case WSAEFAULT:	return "Ошибочный адрес";
	case WSAEINVAL:	return "Ошибка в аргументе";
	case WSAEMFILE: return "Слишком много файлов открыто";
	case WSAEWOULDBLOCK: return "Ресурс временно недоступен";
	case WSAEINPROGRESS: return "Операция в процессе развития";
	case WSAEALREADY: return "Операция уже выполняется";
	case WSAENOTSOCK: return "Сокет задан неправильно";
	case WSAEDESTADDRREQ: return "Требуется адрес расположения";
	case WSAEMSGSIZE: return "Сообщение слишком длинное";
	case WSAEPROTOTYPE: return "Неправильный тип протокола для сокета";
	case WSAENOPROTOOPT: return "Ошибка в опции протокола";
	case WSAEPROTONOSUPPORT: return "Протокол не поддерживается";
	case WSAESOCKTNOSUPPORT: return "Тип сокета не поддерживается";
	case WSAEOPNOTSUPP:	return "Операция не поддерживается";
	case WSAEPFNOSUPPORT: return "Тип протоколов не поддерживается";
	case WSAEAFNOSUPPORT: return "Тип адресов не поддерживается протоколом";
	case WSAEADDRINUSE:	return "Адрес уже используется"; /*для серверов с одинаковым позывным*/
	case WSAEADDRNOTAVAIL: return "Запрошенный адрес не может быть использован";
	case WSAENETDOWN: return "Сеть отключена";
	case WSAENETUNREACH: return "Сеть не достижима";
	case WSAENETRESET: return "Сеть разорвала соединение";
	case WSAECONNABORTED: return "Программный отказ связи";
	case WSAECONNRESET:	return "Связь восстановлена";
	case WSAENOBUFS: return "Не хватает памяти для буферов";
	case WSAEISCONN: return "Сокет уже подключен";
	case WSAENOTCONN: return "Сокет не подключен";
	case WSAESHUTDOWN: return "Нельзя выполнить send : сокет завершил работу";
	case WSAETIMEDOUT: return "Закончился отведенный интервал  времени";
	case WSAECONNREFUSED: return "Соединение отклонено";
	case WSAEHOSTDOWN: return "Хост в неработоспособном состоянии";
	case WSAEHOSTUNREACH: return "Нет маршрута для хоста";
	case WSAEPROCLIM: return "Слишком много процессов";
	case WSASYSNOTREADY: return "Сеть не доступна";
	case WSAVERNOTSUPPORTED: return "Данная версия недоступна";
	case WSANOTINITIALISED:	return "Не выполнена инициализация WS2_32.DLL";
	case WSAEDISCON: return "Выполняется отключение";
	case WSATYPE_NOT_FOUND: return "Класс не найден";
	case WSAHOST_NOT_FOUND:	return "Хост не найден";
	case WSATRY_AGAIN: return "Неавторизированный хост не найден";
	case WSANO_RECOVERY: return "Неопределенная  ошибка";
	case WSANO_DATA: return "Нет записи запрошенного типа";
	case WSA_INVALID_HANDLE: return "Указанный дескриптор события  с ошибкой";
	case WSA_INVALID_PARAMETER: return "Один или более параметров с ошибкой";
	case WSA_IO_INCOMPLETE:	return "Объект ввода - вывода не в сигнальном состоянии";
	case WSA_IO_PENDING: return "Операция завершится позже";
	case WSA_NOT_ENOUGH_MEMORY: return "Не достаточно памяти";
	case WSA_OPERATION_ABORTED: return "Операция отвергнута";
	case WSASYSCALLFAILURE: return "Аварийное завершение системного вызова";
	default: return "**ERROR**";
	};
};

std::string  SetErrorMsgText(std::string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

//4
//послать запрос серверу и получить ответ (позывной сервера, номер порта сервера, указатель на socaddr_in, указатель на размер from)
bool GetServer(char* call, short port, sockaddr* from, int* flen)
{
	memset(from, 0, sizeof(flen)); //обнулить память

	//для сетвеого протокола ipv4, датаграмнный сокет
	if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
	{
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	}

	int optval = 1;

	if (setsockopt(		//установить опции сокета
		cC,
		SOL_SOCKET,		///ур. действия режима
		SO_BROADCAST,	///режим сокета для исп широковещ. адрес.
		(char*)&optval,	///(значение режима сокета)в сост enabled (1)
		sizeof(int))	///длина буфера optval
		== SOCKET_ERROR)
	{
		throw  SetErrorMsgText("opt:", WSAGetLastError());
	}

	SOCKADDR_IN all; //параметры сокета
	all.sin_family = AF_INET; //исп. IP-адресация
	all.sin_port = htons(2000); //порт 2000
	all.sin_addr.s_addr = INADDR_BROADCAST; //всем

	//отправить сообщение
	//(дескриптор сокета, буфер для пересыл. данных, размер буфера, индикатор режима маршрутизации, указ на сокадр, длина структуры)
	if ((sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR)
	{
		throw  SetErrorMsgText("sendto:", WSAGetLastError());
	}

	//принять сообщение
	//(дескриптор сокета, буфер для получ данных, размер буфера, индикатор режима маршрутизации, указ на сокадр, указ на размер ту)
	char nameServer[50];
	if ((recvfrom(cC, nameServer, sizeof(nameServer), NULL, from, flen)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
			return false;

		else throw  SetErrorMsgText("recv:", WSAGetLastError());
	}

	SOCKADDR_IN* addr = (SOCKADDR_IN*)&from;
	std::cout << std::endl << "Порт сервера: " << addr->sin_port;
	std::cout << std::endl << "IP-адрес сервера: " << inet_ntoa(addr->sin_addr);

//сравнение
	if (!strcmp(nameServer, call))
	{
		std::cout << std::endl << "Сервер с таким именем найден";
		return true;
	}
	else
	{
		std::cout << std::endl << "Сервер с таким именем не найден";
		return false;
	}

	return true;
}

int main(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "rus");

	WSADATA wsaData;

	try
	{
		//инициализ библиотеки вс232длл
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		}

		char call[] = "Hello";

		SOCKADDR_IN clnt;
		int lc = sizeof(clnt);

		//послать запрос серверу и получить ответ (позывной сервера, номер порта, указ на сокадр, указ на размер фром)
		GetServer(call, 2000, (sockaddr*)&clnt, &lc);
		//закрываем сокет
		if (closesocket(cC) == SOCKET_ERROR)
		{
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		}
		//завершение работы с библиотекой вс232длл
		if (WSACleanup() == SOCKET_ERROR)
		{
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
		}
	}
	catch (std::string errorMsgText)
	{
		std::cout << std::endl << errorMsgText;
	}

	std::cout << std::endl;
	system("pause");
	return 0;
}