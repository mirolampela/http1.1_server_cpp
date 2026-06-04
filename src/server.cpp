#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
using namespace std;

int start_server() {
    WSADATA wsaData;
    int sResult;

// Initializes Winsock (v2.2) - required for any socket functions on Windows
    sResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (sResult != 0) {
        cout << "WSAStartup failed:" << WSAGetLastError() << endl;
        return 1;
    }

// Create TCP socket (IPv4, stream-based)
    SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket == INVALID_SOCKET){
            cout << "Error at socket():"  << WSAGetLastError() << endl;
            WSACleanup();
            return 0;
        } else {
            cout << "Socket is OK" << endl;
        }

    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(8080);
    my_addr.sin_addr.s_addr = INADDR_ANY;

// Bind the socket to port 8080 on all available network interfaces
    int bResult;
    bResult = bind(tcp_socket, (sockaddr*)&my_addr, sizeof my_addr);
    if (bResult != 0) {
        cout<<"Error at bind():" << WSAGetLastError() << endl;
        return 1;
    }

// Puts the socket into listening mode (SOMAXCONN = max queued connections)    
    if (listen(tcp_socket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Error at listen():" << WSAGetLastError() << endl;
        return 1;
    }

    sockaddr_in client_addr;
    int client_size = sizeof(client_addr);

// Accept -> receive -> parse -> respond - everything happens inside while-loop
    while (true) {
    // Accepts the connection request and returns new socket for that client.
        SOCKET client_socket = accept(tcp_socket, (sockaddr*)&client_addr, &client_size);

        if (client_socket == INVALID_SOCKET) {
            cout << "Error at accept():" << WSAGetLastError() << endl;
            continue;
        }
        cout << "Client connected!" << endl;

    // Receive data from the client into the buffer. Returns the number of bytes read
        char buffer[4096];
        int bytes = recv(client_socket, buffer, 4096, 0);

    // Creates an object that takes bytes amount of data from buffer
        string request(buffer, bytes);

    // Wraps the request in a stream and splits it into method, path and version by spaces.
        istringstream stream(request);
        string method, path, version;
        stream >> method >> path >> version;
        cout << "Method: " << method << ", Path: " << path << ", Version: " << version << endl;

        map<string, string> headers;
        string line;
        while (getline(stream, line)) {
            // Strip trailing carriage return left by getline
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            size_t colon = line.find(":");
            if (colon == string::npos) continue;
            string key = line.substr(0, colon);
            string value = line.substr(colon + 2);
            headers[key] = value;
        }

    // Build a fixed HTTP response (status line + Content-Length + body) and send it to the client
        string body = "<html><body><h1>Simple test page for HTTP 1.1 server.</h1></body></html>";
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(body.size()) + "\r\n\r\n" + body;
        int sent = send(client_socket, response.c_str(), response.size(), 0);

        for (auto& h : headers) {
            cout << h.first << " = " << h.second << endl;
        }
    }



}
