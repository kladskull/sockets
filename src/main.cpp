
/* 
 * File:    main.cpp
 * Author:  Mike Curry
 * License: The MIT License (MIT)
 *
 * Created on February 27, 2015, 12:48 PM
 */

#include <iostream>
#include <istream>
#include <ostream>
#include <unistd.h>
#include "signal.h"
#include "Sockets.h"

using namespace std;

struct sigaction sigAction;

void sigchld_handler(int s) {
    if (s) {
        s = 1;
    }
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {

    // create a socket server
    Sockets socketServer;
    socketServer.setAddress("localhost");
    socketServer.setPort(8080);
    socketServer.setBufferSize(256);
    socketServer.setBlocking(false);
    
    // open and start listening on the server
    socketServer.openSocket();

    // client Socket
    int clientSocket;
    
    while (true) {
        // await a connection
        clientSocket = socketServer.waitForConnection();
        
        // did we get a connection?
        if (clientSocket > 0) {
            // we got something...
            cout << "Client connection: " << socketServer.getClientIp() << endl;
        } else {
            // nothing happened/or error on accept
            if (socketServer.getBlocking() == false) {
                // don't cook the cpu!
                usleep(100000); // pause 10th of a second
            }
        }
    }
    
    // close the server
    socketServer.closeSocket();
    
/*
    char s[INET6_ADDRSTRLEN];

    while (true) {


        // get the ip of the new connection
        inet_ntop(their_addr.ss_family, get_in_addr(reinterpret_cast<sockaddr *>(&their_addr)), s, sizeof s);
        cout << "server: got connection from " << s << endl;

        // fork off
        if (!fork()) {
            // this is now the child process, close the server socket as its no longer required
            close(serverSocket);

            int status;
            int timeouts = 0;

            while (true) {

                // send some response...
                string message = "Here is the data you sent: ";
                string receiveBuffer;

                // get some data
                status = receive(clientSocket, receiveBuffer);
                if (status == 0) {
                    // disconnected?
                    timeouts = 0;
                    break;
                } else if (status == -1) {
                    // error
                    timeouts = 0;
                    break;
                } else if (status == -2) {
                    ++timeouts;
                    if (timeouts > 10) {
                        break;
                    }
                } else {
                    // process the data we just received!
                    string sBuffer = message + receiveBuffer;
                    timeouts = 0;
                    status = send(clientSocket, sBuffer);
                    if (status == -1) {
                        cerr << "There was an error!" << endl;
                        break;
                    }
                }
            }

            // kill the connection off & exit
            close(clientSocket);
            exit(0);
        }
        close(clientSocket); // parent doesn't need this
    }
*/
    return 0;
}
