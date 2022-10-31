//1
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <string>
#include "Winsock2.h" //��������� WS2_32.dll
#pragma comment(lib, "WS2_32.lib") //������� WS2_32.dll
#include <ctime>


using namespace std;

string GetErrorMsgText(int code) //������������ ����� ������
{
	string msgText;
	switch (code) //�������� ���� ��������
	{
	case WSAEINTR: msgText = "������ ������� ��������";
		break;
	case WSAEACCES: msgText = "���������� ����������";
		break;
	case WSAEFAULT: msgText = "��������� �����";
		break;
	case WSASYSCALLFAILURE: msgText = "��������� ���������� ���������� ������";
		break;
	default: msgText = "***ERROR***";
		break;
	}

	return msgText;
}

string SetErrorMsgText(string msgText, int code) //��������� ������
{
	return msgText + GetErrorMsgText(code);
}


int main()
{
	setlocale(LC_ALL, "rus");
	cout << "Server is started" << endl;
	cout << "Server is waiting connections" << endl;
	int i = 0;	//���-�� ��������
	WSADATA ws;
	SOCKET s; //���������� ������ �������
	SOCKET c; //���������� ������ �������
	char ibuf[50];	//��� ������ ��������� �� ��������
	char obuf[50] = "ECHO: Hello from Client";	//������ ��� �������
	int t;

	//1
	try {
		if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws))) //���������������� ���������� WS2_32.DLL (������ ������ �����, ��������� �� wsadata)
		{
			cout << "Socket: " << WSAGetLastError() << endl; //�������� ��������������� ��� ������
		}
		//2 soket(������ ������, ��� ������, ��������)
		if (INVALID_SOCKET == (s = socket(AF_INET, SOCK_STREAM, 0))) //�������� ������, SOCK_STREAM ���������, ��� ����� ��������������� �� �����
		{
			cout << "Socket: " << WSAGetLastError() << endl; //�������� ��������������� ��� ������
		}
		sockaddr_in c_adr;	//���� �������
		sockaddr_in s_adr;	//���� �������
		{
			s_adr.sin_port = htons(3000); //������������� u_short � ������ TCP/IP 3000 - ����� �����
			s_adr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			s_adr.sin_family = AF_INET; //��� IPv4
		}
		//(�����, ��������� �� sockaddr_in, ����� � ������)
		if (SOCKET_ERROR == (bind(s, (LPSOCKADDR)&s_adr, sizeof(s_adr)))) //������� ����� � ����������� (SOCKADDR ������ ���������)
		{
			cout << "Bind: " << WSAGetLastError() << endl;
		}

		//3 (���������� ������, ���� ����� �������)
		if (SOCKET_ERROR == listen(s, 2)) //����������� ����� � ����� ������������� 
		{
			cout << "Listen: " << WSAGetLastError << endl;
		}

		while (true)
		{
			int lcInt = sizeof(c_adr);
			//��������� ����������� � ������ (��������� ������� ������� � ��������� ��������, ���� ��������� �� �������� �������)
			if (INVALID_SOCKET == (c = accept(s, (sockaddr*)&c_adr, &lcInt))) //(����� ������, ��������� �� �������, ��������� �� ����� �������)
			{
				cout << "Accept: " << WSAGetLastError() << endl;
			}

			cout << "Client connect" << endl;
			cout << "ADDRES client : " << inet_ntoa(c_adr.sin_addr) << " : " << htons(c_adr.sin_port) << endl << endl << endl << endl << endl;
			//4, 5 
			while (true)
			{
				//(����� ������, ��������� �� ����� ������, ���-�� ���� � ������, ���������)
				if (SOCKET_ERROR == recv(c, ibuf, sizeof(ibuf), NULL)) //������� ������ �� �������������� ������
				{
					cout << "Recv: " << WSAGetLastError() << endl;
					break;
				}

				cout << i << " Client : " << ibuf << endl;
				i++;

				if (!strcmp(ibuf, "CLOSE")) { break; }
				//(����� ������, ��������� �� ����� ������, ���-�� ���� ������ � ������, ��������� ������� ������ ���������)
				if (SOCKET_ERROR == send(c, obuf, strlen(obuf) + 1, NULL)) //��������� ������ �� �������������� ������
				{
					cout << "Send: " << WSAGetLastError() << endl;
					break;
				}
			}
			i = 0;
			cout << "\t\tClient Diskonect" << endl;
		}
		//(�����)
		if (closesocket(c) == SOCKET_ERROR)  //������� ������������ �����
		{
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		}

		if (WSACleanup() == SOCKET_ERROR) //��������� ������ � ����������� WS2_32.DLL
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