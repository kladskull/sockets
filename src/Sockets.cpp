/* 
 * File:   Sockets.cpp
 * Author: Mike Curry
 *
 * Created on March 2, 2015, 9:01 AM
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>

#include "SocketsException.h"

#include "Sockets.h"


using namespace std;

Sockets::Sockets() {
    _socket = -1;
    _hostName = "";
    _port = 8080;
    _backlog = 128;
    _bufferSize = 4096;
    _sinSize = sizeof _clientAddress;
    _blocking = true;
}

Sockets::~Sockets() {
    if (_socket != -1) {
        closeSocket();
    }
}

// get sockaddr, IPv4 or IPv6:
void *Sockets::get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
        

    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

string Sockets::getClientIp() {
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(_clientAddress.ss_family, get_in_addr(reinterpret_cast<sockaddr *>(&_clientAddress)), ip, sizeof ip);
    return ip;
}

void Sockets::openSocket() {

    struct addrinfo hints, *servinfo, *p;
    int optionValue = 1;

    // setup the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    // resolve the host name
    if (getaddrinfo(NULL, to_string(_port).c_str(), &hints, &servinfo) == 0) {

        // loop through all the results and bind to the first we can
        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                // error, try another interface
                continue;
            }

            if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof (int)) == -1) {
                // server socket options error
                p = NULL;
                break;
            }

            // git a bind error..
            if (bind(_socket, p->ai_addr, p->ai_addrlen) == -1) {
                close(_socket);
                throw new SocketsException("Couldn't bind to the port.");
            }

            // break out..
            break;
        }

        // free the structure
        freeaddrinfo(servinfo); // all done with this structure

    } else {
        // cannot resolve name
        throw new SocketsException("Couldn't get the server address.");
    }

    // nope..
    if (p == NULL) {
        throw new SocketsException("Couldn't bind to the port.");
    }

    // listen for connections on our socket
    if (listen(_socket, _backlog) == -1) {
        throw new SocketsException("Couldn't listen for connections.");
    }

    // set blocking..i
    if (!_blocking) {
        fcntl(_socket, F_SETFL, O_NONBLOCK);
    }
}

void Sockets::closeSocket() {
    if (_socket != -1) {
        close(_socket);
    }
}

int Sockets::sendData(int& clientSocket, const string& buffer) {
    int totalSent = 0;
    int bufferLength = buffer.length();
    int bytesLeft = bufferLength;
    int bytesSent, retries = 0, success = 0;
    
    // try to send 3 times max
    while (true) {
        while (totalSent < bufferLength) {
            bytesSent = send(clientSocket, buffer.c_str() + totalSent, bytesLeft, 0);
            if (bytesSent == -1) {
                break;
            }
            totalSent += bytesSent;
            bytesLeft -= bytesSent;
        }

        // anymore to send?
        if (bytesLeft <= 0) {
            break;
        }

        // did we get an error? retry 5 times
        if (retries++ > 5) {
            break;
        }

        // sleep for 1 second
        usleep(1000000);
    }

    if (totalSent == bufferLength) {
        success = totalSent;
    }

    // success will either be 0 or the length of the data sent
    return success;
}

// read data until we get data, or until a timeout
int Sockets::readDataTimeout(const int &clientSocket, string& buffer, const int& timeout) {
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(_socket + 1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return readData(clientSocket, buffer);
}

// read data until there is nothing else to read
int Sockets::readData(const int &clientSocket, string &buffer) {
    int result;
    //char recvBuffer[16386]; // buffer for client data
    char* recvBuffer = (char*)malloc(_bufferSize);
    //char* recvBuffer = static_cast<char*>(malloc(_bufferSize, sizeof(char)));
    
    stringstream receivedBuffer(_bufferSize);
    
    while (true) {
        // clear the buffer
        memset(recvBuffer, 0, strlen(recvBuffer));
        
        // read the data
       result = recv(clientSocket, recvBuffer, strlen(recvBuffer), 0);
       
       // append the data, if any... 
        receivedBuffer << buffer;
        
       // keep reading?
       if (result <= 0) {
           break;
       }
    }
    
    // assign the buffer to the string
    buffer = receivedBuffer.str();
    
    // free our buffer
    free(recvBuffer);
    
    // send back our status
    return result;
}

int Sockets::waitForConnection() { 
    // any waiting connections?
    _clientSocket = accept(_socket, reinterpret_cast<sockaddr *> (&_clientAddress), &_sinSize);
    
    // return accept status..
    return _clientSocket;
}
