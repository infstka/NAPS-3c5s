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
	case WSAEINTR:				 msgText = "Работа функции прервана\n";						  break;
	case WSAEACCES:				 msgText = "Разрешение отвергнуто\n";						  break;
	case WSAEFAULT:				 msgText = "Ошибочный адрес\n";								  break;
	case WSAEINVAL:				 msgText = "Ошибка в аргументе\n";							  break;
	case WSAEMFILE:				 msgText = "Слишком много файлов открыто\n";				  break;
	case WSAEWOULDBLOCK:		 msgText = "Ресурс временно недоступен\n";					  break;
	case WSAEINPROGRESS:		 msgText = "Операция в процессе развития\n";				  break;
	case WSAEALREADY: 			 msgText = "Операция уже выполняется\n";					  break;
	case WSAENOTSOCK:   		 msgText = "Сокет задан неправильно\n";						  break;
	case WSAEDESTADDRREQ:		 msgText = "Требуется адрес расположения\n";				  break;
	case WSAEMSGSIZE:  			 msgText = "Сообщение слишком длинное\n";				      break;
	case WSAEPROTOTYPE:			 msgText = "Неправильный тип протокола для сокета\n";		  break;
	case WSAENOPROTOOPT:		 msgText = "Ошибка в опции протокола\n";					  break;
	case WSAEPROTONOSUPPORT:	 msgText = "Протокол не поддерживается\n";					  break;
	case WSAESOCKTNOSUPPORT:	 msgText = "Тип сокета не поддерживается\n";				  break;
	case WSAEOPNOTSUPP:			 msgText = "Операция не поддерживается\n";					  break;
	case WSAEPFNOSUPPORT:		 msgText = "Тип протоколов не поддерживается\n";			  break;
	case WSAEAFNOSUPPORT:		 msgText = "Тип адресов не поддерживается протоколом\n";	  break;
	case WSAEADDRINUSE:			 msgText = "Адрес уже используется\n";						  break;
	case WSAEADDRNOTAVAIL:		 msgText = "Запрошенный адрес не может быть использован\n";	  break;
	case WSAENETDOWN:			 msgText = "Сеть отключена\n";								  break;
	case WSAENETUNREACH:		 msgText = "Сеть не достижима\n";							  break;
	case WSAENETRESET:			 msgText = "Сеть разорвала соединение\n";					  break;
	case WSAECONNABORTED:		 msgText = "Программный отказ связи\n";						  break;
	case WSAECONNRESET:			 msgText = "Связь восстановлена\n";							  break;
	case WSAENOBUFS:			 msgText = "Не хватает памяти для буферов\n";				  break;
	case WSAEISCONN:			 msgText = "Сокет уже подключен\n";							  break;
	case WSAENOTCONN:			 msgText = "Сокет не подключен\n";							  break;
	case WSAESHUTDOWN:			 msgText = "Нельзя выполнить send: сокет завершил работу\n";  break;
	case WSAETIMEDOUT:			 msgText = "Закончился отведенный интервал  времени\n";		  break;
	case WSAECONNREFUSED:		 msgText = "Соединение отклонено\n";						  break;
	case WSAEHOSTDOWN:			 msgText = "Хост в неработоспособном состоянии\n";			  break;
	case WSAEHOSTUNREACH:		 msgText = "Нет маршрута для хоста\n";						  break;
	case WSAEPROCLIM:			 msgText = "Слишком много процессов\n";						  break;
	case WSASYSNOTREADY:		 msgText = "Сеть не доступна\n";							  break;
	case WSAVERNOTSUPPORTED:	 msgText = "Данная версия недоступна\n";					  break;
	case WSANOTINITIALISED:		 msgText = "Не выполнена инициализация WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 msgText = "Выполняется отключение\n";						  break;
	case WSATYPE_NOT_FOUND:		 msgText = "Класс не найден\n";								  break;
	case WSAHOST_NOT_FOUND:		 msgText = "Хост не найден\n";								  break;
	case WSATRY_AGAIN:			 msgText = "Неавторизированный хост не найден\n";			  break;
	case WSANO_RECOVERY:		 msgText = "Неопределенная ошибка\n";						  break;
	case WSANO_DATA:			 msgText = "Нет записи запрошенного типа\n";				  break;
	case WSAEINVALIDPROCTABLE:	 msgText = "Ошибочный сервис\n";							  break;
	case WSAEINVALIDPROVIDER:	 msgText = "Ошибка в версии сервиса\n";						  break;
	case WSAEPROVIDERFAILEDINIT: msgText = "Невозможно инициализировать сервис\n";			  break;
	case WSASYSCALLFAILURE:		 msgText = "Аварийное завершение системного вызова\n";		  break;
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
			//открыть канал (подключ клиента к именованному каналу)
			// //(симв имя канала , чтение/запись в канал, режим совм исп, атрибуты безопасности, флаг открытия канала, флаги и атрибуты, доп атрибуты)
			// . - лок комп
			if ((hNP = CreateFile(L"\\\\.\\pipe\\Tube", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
			{
				throw SetErrorPipe("CeateFile: ", GetLastError());
			}

			//изменение характеристик канала
			//(дескр канала, указ на новое состояние, указ на макс кол-во байтов, указ на инт задержки)
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

			//(декср канала, указ на буфер для записи, размер буфера для записи, указ для чтения, размер для чтения, кол-во прочит байт, без асинхр доступа)
			//объединяет операции чтения и записи в одну операцию
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

		//изменение характеристик канала
		//(дескр канала, указ на новое состояние, указ на макс кол-во байтов, указ на инт задержки)
		if (!SetNamedPipeHandleState(hNP, &dwMode, NULL, NULL))
		{
			throw SetErrorPipe("SetNamedPipeHandleState: ", GetLastError());
		}

		char chr[] = " ";
		DWORD lchr = sizeof(chr);

		//(декср канала, указ на буфер для записи, размер буфера для записи, указ для чтения, размер для чтения, кол-во прочит байт, без асинхр доступа)
		//объединяет операции чтения и записи в одну операцию
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