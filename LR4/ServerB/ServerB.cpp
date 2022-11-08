#include "pch.h"
#include <iostream>
#include <string>
#include <time.h>
#include <tchar.h>
#include "Winsock2.h"

#pragma comment(lib, "WS2_32.lib")

SOCKET sS;

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
	case WSAEADDRINUSE:	return "Адрес уже используется"; //6
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
	case WSAETIMEDOUT: return "Закончился отведенный интервал времени";
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


//Ожидание запроса клиентской программы
bool GetRequestFromClient(
	char *name,		///позывной сервера (набор символов, получаемый сервером от клиента и интепретируемый, как запрос на установку соединения)
	short port,		///номер прослушиваемого порта
	sockaddr* from,	///указатель на SOCKADDR_IN
	int* flen)		///указатель на размер from
{
	char nameServer[50];
	memset(from, 0, sizeof(flen)); //обнуление памяти

	//создание сокета (формат адреса, тип сокета, протокол) 
	if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) //для сетвеого протокола ipv4, датаграмнный сокет (поток)
	{
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	}

	SOCKADDR_IN serv; //параметры сокета sS
	serv.sin_family = AF_INET; //исп. IP-адресация
	serv.sin_port = htons(port); //порт
	serv.sin_addr.s_addr = INADDR_ANY; //любой собств адрес

	//связывание сокета с параметрами (сокет, указ. на сокадр, длина сокадр в байтах)
	if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
	{
		throw  SetErrorMsgText("bind:", WSAGetLastError());
	}

	//ожидание запроса (дескриптор сокета, имя сервера, )
	if ((recvfrom(sS, nameServer, sizeof(nameServer), NULL, from, flen)) == SOCKET_ERROR)
	{
		//получ ошибки
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			return false;
		}
		else
		{
			throw  SetErrorMsgText("recv:", WSAGetLastError());
		}
	}

	if (!strcmp(nameServer, name))
	{
		std::cout << std::endl << "Позывной сервера совпадает";
		return true;
	}
	else
	{
		std::cout << std::endl << "Позывной сервера не совпадает";
		return false;
	}
}

//ответить на запрос клиента
//пересылка позывного сервера программе клиента (позывной сервера, указатель на sockaddr_in, указатель на размер from)
bool PutAnswerToClient(const char* name, sockaddr* to, int* lto)
{

	//отправка сообщения без уст соед
	if ((sendto(sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR)
	{
		throw  SetErrorMsgText("send:", WSAGetLastError());
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

		char name[] = "Hello"; //позывной (набор символов, получаемый сервером от клиента и интепретируемый, как запрос на установку соединения)

		for (;;)
		{
			SOCKADDR_IN clnt;
			int lc = sizeof(clnt);
			//обработка запроса клиента (позывной сервера, номер прослуш порта, указ на сокадр, указ на фром)
			if (GetRequestFromClient(name, 2000, (sockaddr*)&clnt, &lc))
			{
				//пересылка позывного сервера программе клиента (позывной сервера, указатель на sockaddr_in, указатель на размер from)
				std::cout << std::endl << PutAnswerToClient(name, (sockaddr*)&clnt, &lc);
			}
			else
			{
				std::cout << std::endl << PutAnswerToClient("**Eror name**", (sockaddr*)&clnt, &lc);
			}

			SOCKADDR_IN* addr = (SOCKADDR_IN*)&clnt;
			std::cout << std::endl << "Порт клиента: " << addr->sin_port;
			//std::cout << std::endl << "IP-адрес клиента: " << inet_ntoa(addr->sin_addr);

			//закрытие сущ. сокета (дескриптор сокета)
			if (closesocket(sS) == SOCKET_ERROR)
			{
				throw  SetErrorMsgText("closesocket:", WSAGetLastError());
			}
		}
		//заверш работы с библиотекой вс232длл
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