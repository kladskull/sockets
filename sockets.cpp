
/* 
 * File:    sockets.cpp
 * Author:  Mike Curry
 * License: The MIT License (MIT)
 *
 * Created on February 27, 2015, 12:48 PM
 */

#include "sockets.h"

#define BUFFER_LENGTH 256

using namespace std;

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int send(int &clientSocket, string &buffer) {
    int totalSent = 0;
    int bufferLength = buffer.length();
    int bytesLeft = bufferLength;
    int bytesSent, retries, success = 0;

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

        // did we get an error? retry 3 times
        if (retries++ > 3) {
            break;
        }

        // sleep for 1/4 of a second
        usleep(250000);
    }

    if (totalSent == bufferLength) {
        success = totalSent;
    }

    // success will either be 0 or the length of the data sent
    return success;
}

int recvtimeout(int s, char *buf, int len, int timeout) {
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s + 1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv(s, buf, len, 0);
}

int receive(int &clientSocket, string &buffer) {
    int result; //, n;

    // setup receive buffers
    stringstream receivedBuffer;
    char buf[BUFFER_LENGTH]; // buffer for client data

    while (true) {
        memset(buf, 0, sizeof buf); //clear the variable
        result = recvtimeout(clientSocket, buf, BUFFER_LENGTH, 1); // 1 second timeout
        if (result == 0) {
            // client disconnected...
            break;
        } else if (result == -1) {
            // error occurred
            break;
        } else if (result == -2) {
            // timeout occurred
            cerr << "timeout..." << endl;
            break;
        } else {
            receivedBuffer << buf;
            cout << "received:  " << result << " bytes..." << endl;

            // did we get it all? if not loop around...
            if (result < BUFFER_LENGTH) {
                // likely all out of data
                break;
            }
        }
    }

    // transfer the data we received to the buffer
    buffer = receivedBuffer.str();

    // send back our status
    return result;
}

int createserver(int backLog, const char* port, int &sockfd) {
    int status = 0;
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    // resolve the host name
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) == 0) {

        // loop through all the results and bind to the first we can
        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                cerr << "server socket error" << endl;
                continue;
            }

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
                cerr << "server socket options error" << endl;
                p = NULL;
                break;
            }

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                cerr << "server bind error" << endl;
                continue;
            }

            // break out..
            break;
        }

        // free the structure
        freeaddrinfo(servinfo); // all done with this structure

    } else {
        // cannot resolve name
        cerr << "server: failed to get server address " << endl;
        return -2;
    }

    // nope..
    if (p == NULL) {
        cerr << "server: failed to bind" << endl;
        return -3;
    }

    // listen for conntections on our socket
    if (listen(sockfd, backLog) == -1) {
        cerr << "server: unable to listen" << endl;
        return -4;
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        cerr << "server: sigaction... " << endl;
        return -5;
    }

    cout << "Waiting for connections..." << endl;

    return status;
}

