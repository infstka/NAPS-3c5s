#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Winsock2.h" //заголовок WS2_32.dll
#pragma comment(lib, "WS2_32.lib") //экспорт WS2_32.dll
#include "Error.h"
#include <list>
#include <time.h>

#define AS_SQUIRT 10
using namespace std;

SOCKET sS;
SOCKET sSUDP;
int serverPort;
char dll[50];
char namedPipe[10];
volatile long accepted = 0, failed = 0, finished = 0, active = 0;

HANDLE hAcceptServer, hConsolePipe, hGarbageCleaner, hDispatchServer, hResponseServer;
//атриб безоп, тип, нач сост, имя
HANDLE hClientConnectedEvent = CreateEvent(NULL, FALSE, FALSE, L"ClientConnected");;

DWORD WINAPI AcceptServer(LPVOID pPrm);
DWORD WINAPI ConsolePipe(LPVOID pPrm);
DWORD WINAPI GarbageCleaner(LPVOID pPrm);
DWORD WINAPI DispatchServer(LPVOID pPrm);
DWORD WINAPI ResponseServer(LPVOID pPrm);
bool  GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen);
bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto);

CRITICAL_SECTION scListContact;

enum TalkersCommand {
	START, STOP, EXIT, STATISTICS, WAIT, SHUTDOWN, GETCOMMAND
};
volatile TalkersCommand  previousCommand = GETCOMMAND;      // предыдущая команда клиента 

struct Contact         // элемент списка подключений       
{
	enum TE {               // состояние  сервера подключения  
		EMPTY,              // пустой элемент списка подключений 
		ACCEPT,             // подключен (accept), но не обслуживается
		CONTACT             // передан обслуживающему серверу  
	}    type;     // тип элемента списка подключений 
	enum ST {               // состояние обслуживающего сервера  
		WORK,               // идет обмен данными с клиентом
		ABORT,              // обслуживающий сервер завершился не нормально 
		TIMEOUT,            // обслуживающий сервер завершился по времени 
		FINISH              // обслуживающий сервер завершился  нормально 
	}      sthread; // состояние  обслуживающего сервера (потока)

	SOCKET      s;         // сокет для обмена данными с клиентом
	SOCKADDR_IN prms;      // параметры  сокета 
	int         lprms;     // длина prms 
	HANDLE      hthread;   // handle потока (или процесса) 
	HANDLE      htimer;    // handle таймера
	HANDLE		serverHThtead;// handle обслуживающего сервера который в последствие может зависнуть

	char msg[50];           // сообщение 
	char srvname[15];       //  наименование обслуживающего сервера 

	Contact(TE t = EMPTY, const char* namesrv = "") // конструктор 
	{
		ZeroMemory(&prms, sizeof(SOCKADDR_IN));
		lprms = sizeof(SOCKADDR_IN);
		type = t;
		strcpy(srvname, namesrv);
		msg[0] = 0x00;
	};

	void SetST(ST sth, const char* m = "")
	{
		sthread = sth;
		strcpy(msg, m);
	}
};
typedef list<Contact> ListContact;

ListContact contacts;

TalkersCommand set_param(char* param) {
	if (!strcmp(param, "start")) return START;
	if (!strcmp(param, "stop")) return STOP;
	if (!strcmp(param, "exit")) return EXIT;
	if (!strcmp(param, "wait")) return WAIT;
	if (!strcmp(param, "shutdown")) return SHUTDOWN;
	if (!strcmp(param, "statistics")) return STATISTICS;
	if (!strcmp(param, "getcommand")) return GETCOMMAND;
}

HANDLE(*ts)(char*, LPVOID);
HMODULE st;

void CALLBACK ASWTimer(LPVOID Prm, DWORD, DWORD) {
	Contact* contact = (Contact*)(Prm);
	cout << "ASWTimer is calling " << contact->hthread << endl;
	//(дескр потока, код заверш)
	TerminateThread(contact->serverHThtead, NULL);
	send(contact->s, "TimeOUT", strlen("TimeOUT") + 1, NULL);
	EnterCriticalSection(&scListContact);
	//д о т
	CancelWaitableTimer(contact->htimer); 
	contact->type = contact->EMPTY;
	contact->sthread = contact->TIMEOUT;
	LeaveCriticalSection(&scListContact);
}

bool AcceptCycle(int sq)// подключение клиентов
{
	bool rc = false;
	Contact c(Contact::ACCEPT, "EchoServer");
	while (sq-- > 0 && !rc)
	{
		if ((c.s = accept(sS, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				throw  SetErrorMsgText("accept:", WSAGetLastError());
		}
		else
		{
			rc = true;
			//вход в кс
			EnterCriticalSection(&scListContact);
			contacts.push_front(c);
			//выход из кс
			LeaveCriticalSection(&scListContact);
			puts("contact connected");
			InterlockedIncrement(&accepted);
			InterlockedIncrement(&active);
		}
	}
	return rc;
};

//Задание 3
void CommandsCycle(TalkersCommand& cmd) // цикл обработки команд
{
	int sq = 0;
	while (cmd != EXIT)
	{
		switch (cmd)
		{
		case START: cmd = GETCOMMAND;
			if (previousCommand != START) {
				sq = AS_SQUIRT;
				cout << "Start command\n";
				SOCKADDR_IN serv, clnt;
				u_long nonblk = 1;

				if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
					throw  SetErrorMsgText("socket:", WSAGetLastError());

				int lclnt = sizeof(clnt);
				serv.sin_family = AF_INET;
				serv.sin_port = htons(serverPort);
				serv.sin_addr.s_addr = INADDR_ANY;

				if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
					throw  SetErrorMsgText("bind:", WSAGetLastError());
				if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
					throw  SetErrorMsgText("listen:", WSAGetLastError());
				//дескр, команда прим к сокету, указатель на парам
				if (ioctlsocket(sS, FIONBIO, &nonblk) == SOCKET_ERROR)
					throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
				previousCommand = START;
			}
			else cout << "--Start already in use--\n";
			break;
		case STOP:  cmd = GETCOMMAND;
			if (previousCommand != STOP) {
				sq = 0;
				cout << "Stop command\n";
				if (closesocket(sS) == SOCKET_ERROR)
					throw  SetErrorMsgText("closesocket:", WSAGetLastError());
				previousCommand = STOP;
			}
			else cout << "--Stop already in use--\n";

			break;
		case WAIT:  cmd = GETCOMMAND;
			sq = 0;
			cout << "Wait command\n";
			cout << "--Socket closed for clients--\n";
			if (closesocket(sS) == SOCKET_ERROR)
				throw  SetErrorMsgText("closesocket:", WSAGetLastError());
			while (contacts.size() != 0);
			cout << "--Size of contacts " << contacts.size() << "--\n";
			cmd = START;
			previousCommand = WAIT;
			cout << "--Socket closed--\n";
			break;
		case SHUTDOWN:
			sq = 0;
			cout << "SHUTDOWN command\n";
			cout << "----------SHUTDOWN----------\n";
			if (closesocket(sS) == SOCKET_ERROR)
				throw  SetErrorMsgText("closesocket:", WSAGetLastError());
			while (contacts.size() != 0);
			cout << "--Size of contacts " << contacts.size() << "--\n";
			cmd = EXIT;
			break;
		case GETCOMMAND:  cmd = GETCOMMAND;
			break;
		};
		if (cmd != STOP) {
			if (AcceptCycle(sq))
			{
				cmd = GETCOMMAND;
				SetEvent(hClientConnectedEvent);
			}
			else SleepEx(0, TRUE);
		}
	};
};

//Задание 
//подключение клиентов к серверу
DWORD WINAPI AcceptServer(LPVOID pPrm)\
{
	cout << "AcceptServer started;\n";
	DWORD rc = 0;
	WSADATA wsaData;
	 
	//Задание 9 
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		CommandsCycle(*((TalkersCommand*)pPrm));
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
	cout << "AcceptServer stop;\n";
	ExitThread(*(DWORD*)pPrm);
}

//Задание 4
//диспетчеризация запросов клиента
DWORD WINAPI DispatchServer(LPVOID pPrm)
//принимает от клиента запрос (команду)  на обслуживание и после этого уже запускается соответствующий поток
//ServiceServer, который исполняет команду и  в случае необходимости обменивается данными с клиентом
{
	cout << "DispathServer started;\n";

	DWORD rc = 0;
	TalkersCommand& command = *(TalkersCommand*)pPrm;
	while (command != EXIT)
	{
		if (command != STOP) {
			//перевод потока в состояние ожидания до получения сигнала
			WaitForSingleObject(hClientConnectedEvent, 5000);
			//cброс события
			ResetEvent(hClientConnectedEvent);
			EnterCriticalSection(&scListContact);
			for (auto i = contacts.begin(); i != contacts.end(); i++) {
				if (i->type == i->ACCEPT) {
					u_long nonblk = 0;
					//дескр, команда прим к сокету, указатель на парам
					if (ioctlsocket(i->s, FIONBIO, &nonblk) == SOCKET_ERROR)//переключение режимов сокета
						throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());

					char serviceType[5];
					clock_t start = clock();
					recv(i->s, serviceType, sizeof(serviceType), NULL);//читает сообщение клиента
					strcpy(i->msg, serviceType);

					clock_t delta = clock() - start;
					if (delta > 5000) {
						cout << "It's so long\n";
						i->sthread = i->TIMEOUT;
						if ((send(i->s, "TimeOUT", strlen("TimeOUT") + 1, NULL)) == SOCKET_ERROR)
							throw  SetErrorMsgText("send:", WSAGetLastError());
						if (closesocket(i->s) == SOCKET_ERROR)
							throw  SetErrorMsgText("closesocket:", WSAGetLastError());
						i->type = i->EMPTY;
					}
					else if (delta <= 5000) {
						if (strcmp(i->msg, "Echo") && strcmp(i->msg, "Time") && strcmp(i->msg, "0001")) {
							if ((send(i->s, "ErrorInquiry", strlen("ErrorInquiry") + 1, NULL)) == SOCKET_ERROR)
								throw  SetErrorMsgText("send:", WSAGetLastError());
							i->sthread = i->ABORT;
							i->type = i->EMPTY;
							if (closesocket(i->s) == SOCKET_ERROR)
								throw  SetErrorMsgText("closesocket:", WSAGetLastError());
						}
						else {
							i->type = i->CONTACT;
							i->hthread = hAcceptServer;
							i->serverHThtead = ts(serviceType, (LPVOID) & (*i));
							//(атриб безоп, тип сброса, имя)
							i->htimer = CreateWaitableTimer(0, FALSE, 0);
							LARGE_INTEGER Li;
							int seconds = 60;
							Li.QuadPart = -(10000000 * seconds);
							//перевод в активное сост
							//(декс таймера время сраб, период, заверш, параметр процед, упр прит)
							SetWaitableTimer(i->htimer, &Li, 0, ASWTimer, (LPVOID) & (*i), FALSE);
							SleepEx(0, TRUE);	// приостанавлиевает поток для асинхронных процедур
						}
					}
				}
			}
			LeaveCriticalSection(&scListContact);
		}
	}
	cout << "DispathServer stop;\n";
	ExitThread(rc);
}

//сборщик мусора (удаление элемента из списка подключений)
DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
	cout << "GarbageCleaner started;\n";

	DWORD rc = 0;
	while (*((TalkersCommand*)pPrm) != EXIT) {
		int listSize = 0;
		int howMuchClean = 0;
		if (contacts.size() != 0) {
			for (auto i = contacts.begin(); i != contacts.end();) {
				if (i->type == i->EMPTY) {
					EnterCriticalSection(&scListContact);
					if (i->sthread == i->FINISH)
						//адрес
						InterlockedIncrement(&finished);
					if (i->sthread == i->ABORT || i->sthread == i->TIMEOUT)
						InterlockedIncrement(&failed);
					i = contacts.erase(i);
					howMuchClean++;
					listSize = contacts.size();
					LeaveCriticalSection(&scListContact);
					Sleep(2000);
				}
				else ++i;
			}
		}
	}
	cout << "GarbageCleaner stop;\n";
	ExitThread(rc);
}

//Задание 22
//серверная часть консоли управления
DWORD WINAPI ConsolePipe(LPVOID pPrm) //осуществляет связь между параллельным сервером и RConcole
{
	cout << "ConsolePipe started;\n";

	DWORD rc = 0;
	char rbuf[100];
	DWORD dwRead, dwWrite;
	HANDLE hPipe;
	try
	{
		char NPname[50];
		sprintf(NPname, "\\\\.\\pipe\\%s", namedPipe);
		if ((hPipe = CreateNamedPipeA(NPname, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
			throw SetErrorMsgText("create:", GetLastError());
		if (!ConnectNamedPipe(hPipe, NULL))
			throw SetErrorMsgText("connect:", GetLastError());
		TalkersCommand& param = *((TalkersCommand*)pPrm);
		while (param != EXIT) {
			cout << "--Connecting to Named Pipe Client--\n";
			ConnectNamedPipe(hPipe, NULL);
			while (ReadFile(hPipe, rbuf, sizeof(rbuf), &dwRead, NULL))
			{
				cout << "Client message:  " << rbuf << endl;
				param = set_param(rbuf);
				if (param == STATISTICS)
				{
					char stat[200];
					sprintf(stat, "\n----STATISTICS----\n"\
						"Connections count:    %d\n" \
						"Breaked:        %d\n" \
						"Complete successfully:             %d\n" \
						"Active connections: %d\n" \
						"-------------------------------------", accepted, failed, finished, contacts.size());
					WriteFile(hPipe, stat, sizeof(stat), &dwWrite, NULL);
				}
				if (param != STATISTICS)
					WriteFile(hPipe, rbuf, strlen(rbuf) + 1, &dwWrite, NULL);
			}
			cout << "--------------Pipe Closed-----------------\n";
			DisconnectNamedPipe(hPipe);
		}
		CloseHandle(hPipe);
		cout << "ConsolePipe breaked;\n" << endl;
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText << endl;
	}
	ExitThread(rc);
}

bool  GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen)
{
	SOCKADDR_IN clnt;
	int lc = sizeof(clnt);
	ZeroMemory(&clnt, lc);
	char ibuf[500];
	int  lb = 0;
	int optval = 1;
	int TimeOut = 1000;
	setsockopt(sSUDP, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int));
	setsockopt(sSUDP, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut));
	while (true) {
		if ((lb = recvfrom(sSUDP, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &lc)) == SOCKET_ERROR) return false;
		if (!strcmp(name, ibuf)) {
			*from = clnt;
			*flen = lc;
			return true;
		}
		cout << "\nBad name\n";
	}
	return false;
}

bool PutAnswerToClient(char* name, sockaddr* to, int* lto) {

	char msg[] = "You can connect to server ";
	if ((sendto(sSUDP, msg, sizeof(msg) + 1, NULL, to, *lto)) == SOCKET_ERROR)
		throw  SetErrorMsgText("sendto:", WSAGetLastError());
	return false;
}

DWORD WINAPI ResponseServer(LPVOID pPrm)//предназначен для  приема  позывного  на 
//широковещательные запросы клиента, предназначенные для поиска сервера в локальной сети
{
	cout << "ResponseServer started;\n";

	DWORD rc = 0;
	WSADATA wsaData;
	SOCKADDR_IN serv;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		throw  SetErrorMsgText("Startup:", WSAGetLastError());
	if ((sSUDP = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	serv.sin_family = AF_INET;
	serv.sin_port = htons(serverPort);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(sSUDP, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());

	SOCKADDR_IN someServer;
	int serverSize = sizeof(someServer);

	SOCKADDR_IN from;
	int lc = sizeof(from);
	ZeroMemory(&from, lc);
	int numberOfClients = 0;
	while (*(TalkersCommand*)pPrm != EXIT)
	{
		try
		{
			if (GetRequestFromClient((char*)"Hello", serverPort, &from, &lc))
			{
				printf("\nConnected Client: №%d, port: %d, ip: %s", ++numberOfClients, htons(from.sin_port), inet_ntoa(from.sin_addr));
				PutAnswerToClient((char*)"Hello", (sockaddr*)&from, &lc);
			}
		}
		catch (string errorMsgText)
		{
			cout << endl << errorMsgText;
		}
	}
	if (closesocket(sSUDP) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());
	if (WSACleanup() == SOCKET_ERROR)
		throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	cout << "ResponseServer stop;\n";
	ExitThread(rc);
}

//главный поток сервера
int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	if (argc == 2) {
		serverPort = atoi(argv[1]);
	}
	else if (argc == 3) {
		serverPort = atoi(argv[1]);
		strcpy(dll, argv[2]);
	}
	else if (argc == 4) {
		serverPort = atoi(argv[1]);
		strcpy(dll, argv[2]);
		strcpy(namedPipe, argv[3]);
	}
	else {
		serverPort = 2000;
		strcpy(dll, "Win32Project1.dll");
		strcpy(namedPipe, "BOX");
	}

	cout << "Server started:" << endl;
	cout << "Port - " << serverPort << endl;
	cout << "NamedPipe - " << namedPipe << endl;

	st = LoadLibraryA(dll);
	ts = (HANDLE(*)(char*, LPVOID))GetProcAddress(st, "SSS");
	if (st == NULL)
		cout << "Load DLL ERROR" << endl;
	else
		cout << "DLL - " << dll << endl << endl;

	volatile TalkersCommand cmd = START;

	//(указ на кс)
	InitializeCriticalSection(&scListContact);

	//(атриб защиты, размер стека потока, ф-я потока, указ на парам, индик запуска, идентиф потока)
	hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
	//задание 11
	hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
	hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
	hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
	hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

	SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(hDispatchServer, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);

	WaitForSingleObject(hAcceptServer, INFINITE);
	WaitForSingleObject(hDispatchServer, INFINITE);
	WaitForSingleObject(hGarbageCleaner, INFINITE);
	WaitForSingleObject(hConsolePipe, INFINITE);
	WaitForSingleObject(hResponseServer, INFINITE);

	CloseHandle(hAcceptServer);
	CloseHandle(hDispatchServer);
	CloseHandle(hGarbageCleaner);
	CloseHandle(hConsolePipe);
	CloseHandle(hResponseServer);

	DeleteCriticalSection(&scListContact);

	//отключ библиотеки от процесса
	FreeLibrary(st);
	return 0;
};