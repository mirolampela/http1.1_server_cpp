#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;

int start_server() {
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket == INVALID_SOCKET){
            cout<<"Error at socket():"<<WSAGetLastError()<<endl;
            WSACleanup();
            return 0;
        } else {
            cout<<"Socket is OK"<<endl;
        }
}
