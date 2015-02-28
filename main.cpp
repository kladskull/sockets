
/* 
 * File:    main.cpp
 * Author:  Mike Curry
 * License: The MIT License (MIT)
 *
 * Created on February 27, 2015, 12:48 PM
 */

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

#include "Logger.h"
#include "sockets.h"

#define PORT "8080"     // the port users will be connecting to
#define BACKLOG 256     // how many pending connections queue will hold

using namespace std;

int main() {

    int serverSocket, clientSocket; // listen on sock_fd, new connection on new_fd
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

    int status = createserver(BACKLOG, PORT, serverSocket);
    if (status < 0) {
        cerr << "Couldn't create the server.." << endl;
        exit(1);
    }

    while (true) {

        // wait for a connection
        sin_size = sizeof their_addr;
        
        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&their_addr), &sin_size);
        if (clientSocket == -1) {
            cerr << "client accept error" << endl;
            continue;
        }

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

    return 0;
}
