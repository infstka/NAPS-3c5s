#include "windows.h"
#include <string>
#include "iostream"
#include <ctime>

#pragma warning(disable : 4996)
using namespace std;

string GetErrorPipe(int code)
{
	string msgText;

	switch (code)
	{
	case WSAEINTR:				 msgText = "������ ������� ��������\n";						  break;
	case WSAEACCES:				 msgText = "���������� ����������\n";						  break;
	case WSAEFAULT:				 msgText = "��������� �����\n";								  break;
	case WSAEINVAL:				 msgText = "������ � ���������\n";							  break;
	case WSAEMFILE:				 msgText = "������� ����� ������ �������\n";				  break;
	case WSAEWOULDBLOCK:		 msgText = "������ �������� ����������\n";					  break;
	case WSAEINPROGRESS:		 msgText = "�������� � �������� ��������\n";				  break;
	case WSAEALREADY: 			 msgText = "�������� ��� �����������\n";					  break;
	case WSAENOTSOCK:   		 msgText = "����� ����� �����������\n";						  break;
	case WSAEDESTADDRREQ:		 msgText = "��������� ����� ������������\n";				  break;
	case WSAEMSGSIZE:  			 msgText = "��������� ������� �������\n";				      break;
	case WSAEPROTOTYPE:			 msgText = "������������ ��� ��������� ��� ������\n";		  break;
	case WSAENOPROTOOPT:		 msgText = "������ � ����� ���������\n";					  break;
	case WSAEPROTONOSUPPORT:	 msgText = "�������� �� ��������������\n";					  break;
	case WSAESOCKTNOSUPPORT:	 msgText = "��� ������ �� ��������������\n";				  break;
	case WSAEOPNOTSUPP:			 msgText = "�������� �� ��������������\n";					  break;
	case WSAEPFNOSUPPORT:		 msgText = "��� ���������� �� ��������������\n";			  break;
	case WSAEAFNOSUPPORT:		 msgText = "��� ������� �� �������������� ����������\n";	  break;
	case WSAEADDRINUSE:			 msgText = "����� ��� ������������\n";						  break;
	case WSAEADDRNOTAVAIL:		 msgText = "����������� ����� �� ����� ���� �����������\n";	  break;
	case WSAENETDOWN:			 msgText = "���� ���������\n";								  break;
	case WSAENETUNREACH:		 msgText = "���� �� ���������\n";							  break;
	case WSAENETRESET:			 msgText = "���� ��������� ����������\n";					  break;
	case WSAECONNABORTED:		 msgText = "����������� ����� �����\n";						  break;
	case WSAECONNRESET:			 msgText = "����� �������������\n";							  break;
	case WSAENOBUFS:			 msgText = "�� ������� ������ ��� �������\n";				  break;
	case WSAEISCONN:			 msgText = "����� ��� ���������\n";							  break;
	case WSAENOTCONN:			 msgText = "����� �� ���������\n";							  break;
	case WSAESHUTDOWN:			 msgText = "������ ��������� send: ����� �������� ������\n";  break;
	case WSAETIMEDOUT:			 msgText = "���������� ���������� ��������  �������\n";		  break;
	case WSAECONNREFUSED:		 msgText = "���������� ���������\n";						  break;
	case WSAEHOSTDOWN:			 msgText = "���� � ����������������� ���������\n";			  break;
	case WSAEHOSTUNREACH:		 msgText = "��� �������� ��� �����\n";						  break;
	case WSAEPROCLIM:			 msgText = "������� ����� ���������\n";						  break;
	case WSASYSNOTREADY:		 msgText = "���� �� ��������\n";							  break;
	case WSAVERNOTSUPPORTED:	 msgText = "������ ������ ����������\n";					  break;
	case WSANOTINITIALISED:		 msgText = "�� ��������� ������������� WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 msgText = "����������� ����������\n";						  break;
	case WSATYPE_NOT_FOUND:		 msgText = "����� �� ������\n";								  break;
	case WSAHOST_NOT_FOUND:		 msgText = "���� �� ������\n";								  break;
	case WSATRY_AGAIN:			 msgText = "������������������ ���� �� ������\n";			  break;
	case WSANO_RECOVERY:		 msgText = "�������������� ������\n";						  break;
	case WSANO_DATA:			 msgText = "��� ������ ������������ ����\n";				  break;
	case WSAEINVALIDPROCTABLE:	 msgText = "��������� ������\n";							  break;
	case WSAEINVALIDPROVIDER:	 msgText = "������ � ������ �������\n";						  break;
	case WSAEPROVIDERFAILEDINIT: msgText = "���������� ���������������� ������\n";			  break;
	case WSASYSCALLFAILURE:		 msgText = "��������� ���������� ���������� ������\n";		  break;
	default:					 msgText = "Have no connection with Named pipe\n";			  break;
	};

	return msgText;
};

string SetErrorPipe(string msgText, int code)
{
	return msgText + GetErrorPipe(code);
};

int main()
{
	cout << "ClientNPt" << endl << endl;
	int start_time = clock();
	try
	{
		HANDLE hNP;

		char rbuf[50];
		char wbuf[50] = "Hello from ClientNPt ";
		DWORD rb = sizeof(rbuf);
		DWORD wb = sizeof(wbuf);

		DWORD dwMode = PIPE_READMODE_MESSAGE;

		for (int i = 0; i < 10; i++)
		{
			//������� ����� (������� ������� � ������������ ������)
			// //(���� ��� ������ , ������/������ � �����, ����� ���� ���, �������� ������������, ���� �������� ������, ����� � ��������, ��� ��������)
			// . - ��� ����
			if ((hNP = CreateFile(L"\\\\.\\pipe\\Tube", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
			{
				throw SetErrorPipe("CeateFile: ", GetLastError());
			}

			//��������� ������������� ������
			//(����� ������, ���� �� ����� ���������, ���� �� ���� ���-�� ������, ���� �� ��� ��������)
			if (!SetNamedPipeHandleState(hNP, &dwMode, NULL, NULL))
			{
				throw SetErrorPipe("SetNamedPipeHandleState: ", GetLastError());
			}

			char inum[6];
			sprintf(inum, "%d", i + 1);
			char buf[sizeof(wbuf) + sizeof(inum)];
			strcpy(buf, wbuf);
			strcat(buf, inum);
			wb = sizeof(wbuf);

			//(����� ������, ���� �� ����� ��� ������, ������ ������ ��� ������, ���� ��� ������, ������ ��� ������, ���-�� ������ ����, ��� ������ �������)
			//���������� �������� ������ � ������ � ���� ��������
			if (!TransactNamedPipe(hNP, buf, wb, rbuf, rb, &rb, NULL))
			{
				throw SetErrorPipe("TransactNamedPipe: ", GetLastError());
			}

			cout << rbuf << endl;

			CloseHandle(hNP);
		}

		if ((hNP = CreateFile(L"\\\\.\\pipe\\Tube", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
		{
			throw SetErrorPipe("CreateFile: ", GetLastError());
		}

		//��������� ������������� ������
		//(����� ������, ���� �� ����� ���������, ���� �� ���� ���-�� ������, ���� �� ��� ��������)
		if (!SetNamedPipeHandleState(hNP, &dwMode, NULL, NULL))
		{
			throw SetErrorPipe("SetNamedPipeHandleState: ", GetLastError());
		}

		char chr[] = " ";
		DWORD lchr = sizeof(chr);

		//(����� ������, ���� �� ����� ��� ������, ������ ������ ��� ������, ���� ��� ������, ������ ��� ������, ���-�� ������ ����, ��� ������ �������)
		//���������� �������� ������ � ������ � ���� ��������
		if (!TransactNamedPipe(hNP, chr, lchr, rbuf, rb, &rb, NULL))
		{
			throw SetErrorPipe("TransactNamedPipe: ", GetLastError());
		}

		CloseHandle(hNP);
	}
	catch (string e)
	{
		cout << e << endl;
	}

	int end_time = clock();
	int abs_time = end_time - start_time;
	cout << "Time: " << abs_time << " ms \n";

	system("pause");
	return 0;
}