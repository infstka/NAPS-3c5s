#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include "time.h"
#include <Windows.h>
//#include <WinSock2.h>

#pragma comment(lib, "WS2_32.lib")

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

bool GetServerByName(char* call, char* hostname, short port, sockaddr* from, int* flen)
{
	SOCKET sock;
	hostent* server;
	int optval = 1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
	{
		throw SetErrorMsgText("socket: ", WSAGetLastError());
	}

	if ((server = gethostbyname(hostname)) == NULL)
		throw SetErrorMsgText("Error: server not found.", 0);

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(port);
	all.sin_addr.s_addr = ((in_addr*)server->h_addr_list[0])->s_addr;
	char buf[] = "hello";
	if (sendto(sock, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}
	char ibuf[50];
	if (recvfrom(sock, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
			return false;
		else
			throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
	}
	cout << ibuf << endl;
	if (closesocket(sock) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
	}
	return true;
}


void main(int argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	WSADATA wsaData;
	char* hostname = NULL;
	char call[] = "hello";
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
			memset(&from, 0, sizeof(from));
			int lfrom = sizeof(from);
			GetServerByName(call, hostname, 2000, (sockaddr*)&from, &lfrom);
			cout << "Server socket -> ";
			cout << inet_ntoa(from.sin_addr) << ": " << htons(from.sin_port) << endl;
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
