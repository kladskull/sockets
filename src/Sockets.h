/* 
 * File:   Sockets.h
 * Author: Mike Curry
 *
 * Created on March 2, 2015, 9:01 AM
 */

#ifndef SOCKETS_H
#define	SOCKETS_H

#include <arpa/inet.h>
#include <netinet/in.h> 

class Sockets {
private:
    int _socket;
    std::string _hostName;
    int _port;
    int _backlog;
    int _bufferSize;
    struct sockaddr_in _server;
    bool _blocking;
    
    // client attributes
    int _clientSocket;
    struct sockaddr_storage _clientAddress;
    socklen_t _sinSize;
protected:
public:
    Sockets();
    Sockets(const Sockets& orig);
    Sockets(const std::string& address, const int& port);
    ~Sockets();

    // getters
    std::string getAddress() { return _hostName; }
    int getPort() { return _port; }
    int getBacklog() { return _backlog; }
    int getBufferSize() { return _bufferSize; }
    bool getBlocking() { return _blocking; }
    
    // setters
    void setAddress(const std::string& address) { _hostName = address; }
    void setPort(const int& port) { if (port > 0 && port < 65536) { _port = port; } }
    void setBacklog(const int& backlog) { if (backlog >= 0 && backlog <= 128) { _backlog = backlog; } }
    void setBufferSize(const int& bufferSize) { if (bufferSize > 0 && bufferSize <= 1048576) { _bufferSize = bufferSize; } }
    void setBlocking(const bool& blocking) { _blocking = blocking; }    
    
    // member functions
    void openSocket();
    void closeSocket();
    int sendData(int &clientSocket, const std::string &buffer);
    int readDataTimeout(const int &clientSocket, std::string& buffer, const int& timeout);
    int readData(const int &clientSocket, std::string &buffer);
    int waitForConnection();
    std::string getClientIp();
    
private:
    void *get_in_addr(struct sockaddr *sa);
};

#endif	/* SOCKETS_H */
