#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <cerrno>
#include <cstring>
#include "ParseRequest.h"
using namespace std;

int start_server() {

// Create TCP socket (IPv4, stream-based)
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket == -1){
            cout << "Error at socket():"  << strerror(errno) << endl;
            return 1;
        } else {
            cout << "Socket is OK" << endl;
        }

    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(8080);
    my_addr.sin_addr.s_addr = INADDR_ANY;

// Allow reusing port right after restart
    int opt = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        cout << "Error at setsockopt():" << strerror(errno) << endl;
        return 1;
    }

// Bind the socket to port 8080 on all available network interfaces
    int bResult;
    bResult = bind(tcp_socket, (sockaddr*)&my_addr, sizeof my_addr);
    if (bResult == -1) {
        cout<<"Error at bind():" << strerror(errno) << endl;
        return 1;
    }

// Puts the socket into listening mode (SOMAXCONN = max queued connections)    
    if (listen(tcp_socket, SOMAXCONN) == -1) {
        cout << "Error at listen():" << strerror(errno) << endl;
        return 1;
    }

    sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

// Accept -> receive -> parse -> respond - everything happens inside while-loop
    while (true) {
    // Accepts the connection request and returns new socket for that client.
        int client_socket = accept(tcp_socket, (sockaddr*)&client_addr, &client_size);

        if (client_socket == -1) {
            cout << "Error at accept():" << strerror(errno) << endl;
            continue;
        }
        cout << "Client connected!" << endl;

    // Receive data from the client into the buffer. Returns the number of bytes read
        char buffer[4096];
        int bytes = recv(client_socket, buffer, 4096, 0);

    // Creates an object that takes bytes amount of data from buffer
        string request(buffer, bytes);

    // Wraps the request in a stream and splits it into method, path and version by spaces.
        HttpRequest req = parse_request(request);
        cout << "Method: " << req.method << ", Path: " << req.path << ", Version: " << req.version << endl;



    // Build a fixed HTTP response (status line + Content-Type + Content-Length + Connection + body) and send it to the client
        string body = "<html><body><h1>Simple test page for HTTP 1.1 server.</h1></body></html>";
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;
        int sent = send(client_socket, response.c_str(), response.size(), 0);

        for (auto& h : req.headers) {
            cout << h.first << " = " << h.second << endl;
        }

    // Close the connection
        close(client_socket);
        cout << "Connection closed!" << endl;

    }



}
