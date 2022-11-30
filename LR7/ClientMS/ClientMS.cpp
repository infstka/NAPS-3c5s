#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include "iostream"
#include <Windows.h>
#include "time.h"

using namespace std;

string  GetErrorMail(int code)    // cформировать текст ошибки 
{
    string msgText;
    switch (code)                      // проверка кода возврата  
    {
    case WSAEINTR: msgText = "Работа функции прервана"; break;
    case WSAEACCES: msgText = "Разрешение отвергнуто"; break;
    case WSAEFAULT: msgText = "Ошибочный адрес"; break;
    case WSAEINVAL: msgText = "Ошибка в аргументе"; break;
    case WSAEMFILE: msgText = "Слишком много файлов открыто"; break;
    case WSAEWOULDBLOCK: msgText = "Ресурс временно недоступен"; break;
    case WSAEINPROGRESS: msgText = "Операция в процессе развития"; break;
    case WSAEALREADY: msgText = "Операция уже выполняется"; break;
    case WSAENOTSOCK: msgText = "Сокет задан неправильно"; break;
    case WSAEDESTADDRREQ: msgText = "Требуется адрес расположения"; break;
    case WSAEMSGSIZE: msgText = "Сообщение слишком длинное"; break;
    case WSAEPROTOTYPE: msgText = "Неправильный тип протокола для сокета"; break;
    case WSAENOPROTOOPT: msgText = "Ошибка в опции протокола"; break;
    case WSAEPROTONOSUPPORT: msgText = "Протокол не поддерживается"; break;
    case WSAESOCKTNOSUPPORT: msgText = "Тип сокета не поддерживается"; break;
    case WSAEOPNOTSUPP: msgText = "Операция не поддерживается"; break;
    case WSAEPFNOSUPPORT: msgText = "Тип протоколов не поддерживается"; break;
    case WSAEAFNOSUPPORT: msgText = "Тип адресов не поддерживается протоколом"; break;
    case WSAEADDRINUSE: msgText = "Адрес уже используется"; break;
    case WSAEADDRNOTAVAIL: msgText = "Запрошенный адрес не может быть использован"; break;
    case WSAENETDOWN: msgText = "Сеть отключена"; break;
    case WSAENETUNREACH: msgText = "Сеть не достижима"; break;
    case WSAENETRESET: msgText = "Сеть разорвала соединение"; break;
    case WSAECONNABORTED: msgText = "Программный отказ связи"; break;
    case WSAECONNRESET: msgText = "Связь восстановлена"; break;
    case WSAENOBUFS: msgText = "Не хватает памяти для буферов"; break;
    case WSAEISCONN: msgText = "Сокет уже подключен"; break;
    case WSAENOTCONN: msgText = "Сокет не подключен"; break;
    case WSAESHUTDOWN: msgText = "Нельзя выполнить send : сокет завершил работу"; break;
    case WSAETIMEDOUT: msgText = "Закончился отведенный интервал  времени"; break;
    case WSAECONNREFUSED: msgText = "Соединение отклонено"; break;
    case WSAEHOSTDOWN: msgText = "Хост в неработоспособном состоянии"; break;
    case WSAEHOSTUNREACH: msgText = "Нет маршрута для хоста"; break;
    case WSAEPROCLIM: msgText = "Слишком много процессов"; break;
    case WSASYSNOTREADY: msgText = "Сеть не доступна"; break;
    case WSAVERNOTSUPPORTED: msgText = "Данная версия недоступна"; break;
    case WSANOTINITIALISED: msgText = "Не выполнена инициализация WS2_32.DLL"; break;
    case WSAEDISCON: msgText = "Выполняется отключение"; break;
    case WSATYPE_NOT_FOUND: msgText = "Класс не найден"; break;
    case WSAHOST_NOT_FOUND: msgText = "Хост не найден"; break;
    case WSATRY_AGAIN: msgText = "Неавторизированный хост не найден"; break;
    case WSANO_RECOVERY: msgText = "Неопределенная  ошибка"; break;
    case WSANO_DATA: msgText = "Нет записи запрошенного типа"; break;
    case WSASYSCALLFAILURE: msgText = "Аварийное завершение системного вызова"; break;
    default: msgText = "***ERROR***"; break;
    };
    return msgText;
};

string SetErrorMail(string msgText, int code)
{
    return msgText + ": " + GetErrorMail(code);
}

//. - лок комп
//mailslot - фиксированное слово
//box - имя почтового ящика 
#define MAIL1 TEXT("\\\\WIN-U2TD77CA49G\\mailslot\\Box")
#define MAIL2 TEXT("\\\\.\\mailslot\\Box")

int main()
{
    setlocale(LC_CTYPE, "rus");

    HANDLE hMS; //дескриптор почт ящика
    double t1, t2;
    char messageL[50] = "Hello from Mailslots-Client Local "; //буфер ввода
    DWORD bytes; //длина прочит/запис сообщения

    int countMessage = 0;
    cout << endl << "Count of messages: ";
    cin >> countMessage;

    try
    {
        //подключение клиента к почтовому ящику
        //(симв. имя почтового ящика, чтение/запись, режим совместного исп., атрибуты безопасности, флаг открытия ПЯ, флаги и атрибуты, доп атрибуты)
        if ((hMS = CreateFile(MAIL2, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
            throw SetErrorMail("CreateFile: ", GetLastError());

        cout << "MS with local name has started\n";

        t1 = clock();

        for (int i = 0; i < countMessage; i++)
        {
            char newMessage[50] = "";
            char number[5] = "";
            _itoa(i, number, 10);
            strcat(newMessage, messageL);
            strcat(newMessage, number);

            //запись данных в почтовый ящик
            if (!WriteFile(hMS, newMessage, sizeof(newMessage), &bytes, NULL))
                throw SetErrorMail("WriteFile: ", GetLastError());
        }

        t2 = clock();

        cout << "\ntime:" << t2 - t1 << "ms." << endl << endl;

        //закрытие дескриптора почтового ящика
        CloseHandle(hMS);
    }
    catch (string e)
    {
        cout << e << endl;
    }

    system("pause");
}
