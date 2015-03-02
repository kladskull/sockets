## Sockets Server

Sockets is built on some earlier work by Brian "Beej" Hall (beej@beej.us) mostly
from his book, which can be found here: http://beej.us/guide/

The goal of Sockets is to create a fast and easy to follow source base that can
be used for a multitude of things like a Web Server, Telnet Server, MMO, Proxy
or basically anything sockets.

The current goal is to make improvements specifically around readability, 
performance, add unit testing and make sure its simple to use. I also plan
on adding a UDP implementation in the near future.

Example usage:

```
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
```

