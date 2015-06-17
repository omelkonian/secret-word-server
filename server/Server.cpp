/*
 * Server.cpp
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#include "Server.h"
#include "CopyArguments.h"
#include "CopyThread.h"
#include "../shortcuts.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
using namespace std;

extern pthread_mutex_t printMutex;

void* serve(void *args) {
	CopyThread *copyThread = new CopyThread((CopyArguments *) args);
	return NULL;
}

Server::Server(CopyArguments *args) {
	this->size = args->size;
	this->threadNo = args->threadNo;
	this->iterationNo = args->iterationNo;
	this->hashNo = args->hashNo;
	this->port = args->port;
	this->logfile = args->logfile;
	this->seed = args->seed;
	this->bloomFilter = args->bloomFilter;

	int sock, newSocket;
	struct sockaddr_in server, client;
	socklen_t clientLen;
	struct sockaddr *serverPtr = (struct sockaddr *) &server;
	struct sockaddr *clientPtr = (struct sockaddr *) &client;
;
	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " created socket" << endl;
	UNLOCK(printMutex);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	/* Bind socket to address */
	if (bind(sock, serverPtr, sizeof(server)) < 0)
		perror_exit("bind");
	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " binded socket" << endl;
	UNLOCK(printMutex);

	/* Listen for connections */
	if (listen(sock, 10) < 0)
		perror_exit("listen");

	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " Listening for connections to port " << port << endl;
	UNLOCK(printMutex);
	while (true) {
		// Accept connection
		if ((newSocket = accept(sock, clientPtr, &clientLen)) < 0)
			perror_exit("accept");
		// Find client's address
		LOCK(printMutex);
		cout << YEL << "Server" << RESET << " Accepted connection" << endl;
		UNLOCK(printMutex);

		// Create thread for serving
		pthread_t threadID;
		CopyArguments *args = new CopyArguments(size, threadNo, iterationNo, hashNo, port, logfile, seed, bloomFilter, newSocket);
		CREATE(threadID, serve, (void *)args);
		DETACH(threadID);
	}
}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

