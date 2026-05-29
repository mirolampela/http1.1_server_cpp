#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;

int start_server() {
    WSADATA wsaData;
    int sResult;

    sResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (sResult != 0) {
        printf("WSAStartup failed: %d\n", sResult);
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

    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(8080);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    int bResult;

    bResult = bind(tcp_socket, (sockaddr*)&my_addr, sizeof my_addr);
    if (bResult != 0) {
        cout<<"Error at bind():"<<WSAGetLastError()<<endl;
        return 1;
    }

    if (listen(tcp_socket, SOMAXCONN) == SOCKET_ERROR) {
        cout<<"Error at listen():"<<WSAGetLastError()<<endl;
        return 1;
    }

    sockaddr_in client_addr;
    int client_size = sizeof(client_addr);

    while (true) {
        SOCKET client_socket = accept(tcp_socket, (sockaddr*)&client_addr, &client_size);

        if (client_socket == INVALID_SOCKET) {
            cout<<"Error at accept():"<<WSAGetLastError()<<endl;
            continue;
        }
        cout<<"Client connected!"<<endl;
    }
}
