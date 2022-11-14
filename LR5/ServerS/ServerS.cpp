// ServerS.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <time.h>
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll
#pragma warning (disable : 4996)
#define CASE(i) case i:msgText=#i; break
#define MAX_SIZE_OF_MSG 50

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "WSAEINTR"; break;
	case WSAEACCES: msgText = "WSAEACCES"; break;
	case WSAEFAULT: msgText = "WSAEFAULT"; break;
	case WSAEINVAL: msgText = "WSAEINVAL"; break;
	case WSAEMFILE: msgText = "WSAEMFILE"; break;
	case WSAEWOULDBLOCK:msgText = "WSAEWOULDBLOCK"; break;
	case WSAEINPROGRESS:msgText = "WSAEINPROGRESS"; break;
	case WSAENOTSOCK:msgText = "WSAENOCTSOCK"; break;
	case WSAEDESTADDRREQ:msgText = "WSAEDESTADDRREQ"; break;
	case WSAEMSGSIZE:msgText = "WSAEMSGSIZE"; break;
	case WSAEPROTOTYPE:msgText = "WSAEPROTOTYPE"; break;
	case WSAENOPROTOOPT:msgText = "WSAENOPROTOOPT"; break;
	case WSAEPROTONOSUPPORT:msgText = "WSAEPROTONOSUPPORT"; break;
	case WSAESOCKTNOSUPPORT:msgText = "WSAESOCKTNOSUPPORT"; break;
	case WSAEOPNOTSUPP:msgText = "WSAEOPNOTSUPP"; break;
	case WSAEPFNOSUPPORT:msgText = "WSAEPFNOSUPPORT"; break;
	case WSAEAFNOSUPPORT:msgText = "WSAEAFNOSUPPORT"; break;
	case WSAEADDRINUSE:msgText = "WSAEADDRINUSE"; break;
	case WSAEADDRNOTAVAIL:msgText = "WSAEADDRNOTAVAIL"; break;
	case WSAENETDOWN:msgText = "WSAENETDOWN"; break;
	case WSAENETUNREACH:msgText = "WSAENETUNREACH"; break;
	case WSAENETRESET:msgText = "WSAENETRESET"; break;
	case WSAECONNABORTED:msgText = "WSAECONNABORTED"; break;
	case WSAECONNRESET:msgText = "WSAECONNRESET"; break;
	case WSAENOBUFS:msgText = "WSAENOBUFS"; break;
	case WSAEISCONN:msgText = "WSAEISCONN"; break;
	case WSAENOTCONN:msgText = "WSAENOTCONN"; break;
	case WSAESHUTDOWN:msgText = "WSAESHUTDOWN"; break;
	case WSAETIMEDOUT:msgText = "WSAETIMEDOUT"; break;
	case WSAECONNREFUSED:msgText = "WSAECONNREFUSED"; break;
	case WSAEHOSTDOWN:msgText = "WSAEHOSTDOWN"; break;
	case WSAEHOSTUNREACH:msgText = "WSAEHOSTUNREACH"; break;
	case WSAEPROCLIM:msgText = "WSAEPROCLIM"; break;
	case WSASYSNOTREADY:msgText = "WSASYSNOTREADY"; break;
	case WSAVERNOTSUPPORTED:msgText = "WSAVERNOTSUPPORTED"; break;
	case WSANOTINITIALISED:msgText = "WSANOTINITIALISED"; break;
	case WSAEDISCON:msgText = "WSAEDISCON"; break;
	case WSATYPE_NOT_FOUND:msgText = "WSATYPE_NOT_FOUND"; break;
	case WSAHOST_NOT_FOUND:msgText = "WSAHOST_NOT_FOUND"; break;
	case WSATRY_AGAIN:msgText = "WSATRY_AGAIN"; break;
	case WSANO_RECOVERY:msgText = "WSANO_RECOVERY"; break;
	case WSANO_DATA:msgText = "WSANO_DATA"; break;
	case WSAEINVALIDPROCTABLE:msgText = "WSAEINVALIDPROCTABLE"; break;
	case WSAEINVALIDPROVIDER:msgText = "WSAEINVALIDPROVIDER"; break;
	case WSAEPROVIDERFAILEDINIT:msgText = "WSAEPROVIDERFAILEDINIT"; break;
	case WSASYSCALLFAILURE:msgText = "WSASYSCALLFAILURE"; break;
	default: msgText = "***ERROR***"; break;
	}
	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
}

//ответить на запрос клиента
//пересылка позывного сервера программе клиента (позывной сервера, указатель на sockaddr_in, указатель на размер from)
bool PutAnswerToClient(char* name, SOCKET sock, sockaddr * to, int* lto)
{
	//для отправки сообщения без уст. соединения
	//(дексриптор сокета, буфер для пересыл. данных)
	if (sendto(sock, name, strlen(name) + 1, NULL, to, *lto) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}
	return true;
}

//Ожидание запроса клиентской программы
//(позывной сервера (набор символов, получаемый сервером от клиента и интепретируемый, как запрос на установку соединения), 
//номер прослушиваемого порта, указатель на SOCKADDR_IN, указатель на размер from)
bool GetRequestFromClient(char* name, SOCKET sock, sockaddr * from, int* flen)
{
	char ibuf[50];
	cout << "Waiting for message..." << endl;
	while (true)
	{
		//получение сообщения без установления соединения
		//(дескриптор сокета, буфер для получаемых данных, размер буфера, индикатор режима маршрутизации, указатель на сокадр, указатель на размер ту)
		if (recvfrom(sock, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
				return false;
			else
				throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
		}
		if (strcmp(ibuf, name) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}



void main()
{
	setlocale(LC_CTYPE, "rus");
	WSADATA wsaData;
	SOCKET sockServer;
	hostent* server;
	hostent* client;
	char name[] = "Hello";
	char ans[] = "Access";
	char hostname[32];
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		}
		if ((sockServer = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		{
			throw SetErrorMsgText("socket: ", WSAGetLastError());
		}

		SOCKADDR_IN serv; //параметры сокета sS
		serv.sin_family = AF_INET; //исп. IP-адресация
		serv.sin_port = htons(2000);  //порт
		serv.sin_addr.s_addr = INADDR_ANY; //любой собств адрес

		//связывание сокета с параметрами (сокет, указ. на сокадр, длина сокадр в байтах)
		if (bind(sockServer, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("bind: ", WSAGetLastError());
		}

		SOCKADDR_IN from;
		memset(&from, 0, sizeof(from)); //обнуление памяти
		int lfrom = sizeof(from);

		//для получения информации о хосте по его симв. имени; получение адреса хоста по его имени
		//(символич. имя хоста)
		gethostname(hostname, sizeof(hostname));
		cout << "Hostname: " << hostname << endl;
		while (true)
		{

//Ожидание запроса клиентской программы
//(позывной сервера (набор символов, получаемый сервером от клиента и интепретируемый, как запрос на установку соединения), 
//номер прослушиваемого порта, указатель на SOCKADDR_IN, указатель на размер from)
			if (GetRequestFromClient(name, sockServer, (sockaddr*)&from, &lfrom))
			{
				cout << "\n=======Connection=======" << endl;
				cout << "Client socket: ";

				//преобразование символьного представления айпив4 адреса в формат тсп / преобразовать ю_шорт в тсп
				cout << inet_ntoa(((SOCKADDR_IN*)&from)->sin_addr) << ": " << htons(((SOCKADDR_IN*)&from)->sin_port) << endl;
				//получение имени хоста по адресу
				//(адрес в сетевом формате, длина адреса в байтах, тип адреса(афинет для тсп))
				cout << "Client: " << gethostbyaddr((char*)&from.sin_addr, sizeof(from.sin_addr), AF_INET)->h_name << endl << endl;
//ответить на запрос клиента
//пересылка позывного сервера программе клиента (позывной сервера, указатель на sockaddr_in, указатель на размер from)
				PutAnswerToClient(ans, sockServer, (sockaddr*)&from, &lfrom);
			}
			else
				cout << "Error" << endl;
		}
		//закрытие
		if (closesocket(sockServer) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		}
		//завершение работы
		if (WSACleanup() == SOCKET_ERROR)
		{
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
		}
	}
	catch (string errorMsgText)
	{
		cout << "WSAGetLastError: " << errorMsgText << endl;
	}
	system("pause");
}
