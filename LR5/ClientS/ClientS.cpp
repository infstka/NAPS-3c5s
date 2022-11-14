// ClientS.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <WinSock2.h> 
#include <cstring>
#include <time.h>
#pragma warning (disable : 4996)
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll
#define CASE(i) case i:msgText=#i;break
#define MAX_SIZE_OF_MSG 50

using namespace std;

string GetErrorMsgText(int code) // cформировать текст ошибки 
{
	string msgText;
	switch (code) // проверка кода возврата
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

//ф-я для пересылки позывного серверу, адрес которого задан в виде симв.имени компьютера
//(позывной, имя компьютера в сети, указатель на сокадр, указатель на размер фром)
bool GetServerByName(char* call, char* hostname, short port, sockaddr * from, int* flen)
{
	SOCKET sock;
	hostent* server;
	int optval = 1;

	//для сетвеого протокола ipv4, датаграмнный сокет
	if ((sock = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
	{
		throw SetErrorMsgText("socket: ", WSAGetLastError());
	}

//для получения информации о хосте по его симв. имени
//(символич. имя хоста)
	if ((server = gethostbyname(hostname)) == NULL)		
		throw SetErrorMsgText("Can't find server", 0);

	SOCKADDR_IN all; //параметры сокета
	all.sin_family = AF_INET; //исп. IP-адресация
	all.sin_port = htons(port); //порт 2000
	all.sin_addr.s_addr = ((in_addr*)server->h_addr_list[0])->s_addr; //адрес сервера
	char buf[] = "Hello";

//отправить сообщение
//(дескриптор сокета, буфер для пересыл. данных, размер буфера, индикатор режима маршрутизации, указ на сокадр, длина структуры)
	if (sendto(sock, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}
	char ibuf[50];

//принять сообщение
//(дескриптор сокета, буфер для получ данных, размер буфера, индикатор режима маршрутизации, указ на сокадр, указ на размер ту)
	if (recvfrom(sock, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
			return false;
		else
			throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
	}
	cout << ibuf << endl;
	//закрыть
	if (closesocket(sock) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
	}
	return true;
}


void main(int argc, char** argv)
{
	WSADATA wsaData;
	setlocale(LC_CTYPE, "rus");
	char* hostname = NULL;
	char call[] = "Hello";
	if (argc >= 1)
	{
		hostname = argv[1];
		try
		{
			if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)

			{
				throw SetErrorMsgText("Startup: ", WSAGetLastError());
			}
			SOCKADDR_IN from;
			memset(&from, 0, sizeof(from)); //обнулить память
			int lfrom = sizeof(from);
//ф-я для пересылки позывного серверу, адрес которого задан в виде симв.имени компьютера
//(позывной, имя компьютера в сети, указатель на сокадр, указатель на размер фром)
			GetServerByName(call, hostname, 2000, (sockaddr*)&from, &lfrom); //
			cout << "Server socket: ";
			//преобразование символьного представления айпив4 адреса в формат тсп / преобразовать ю_шорт в тсп
			cout << inet_ntoa(from.sin_addr) << ": " << htons(from.sin_port) << endl;

			//завершить работу 
			if (WSACleanup() == SOCKET_ERROR)
			{
				throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
			}
		}
		catch (string errorMsgText)
		{
			cout << "WSAGetLastError: " << errorMsgText << endl;
		}
	}
	else
	{
		cout << "type hostname as parameter\n";
	}
	system("pause");
}