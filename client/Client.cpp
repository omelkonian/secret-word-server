/*
 * Client.cpp
 *
 *  Created on: May 9, 2015
 *      Author: orestis
 */

#include "Client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "../shortcuts.h"

#include <iostream>
using namespace std;

extern pthread_mutex_t printMutex;

Client::Client(char *address, CopyArguments *args) {
	this->address = address;

	int port, sock;
	struct sockaddr_in server;
	struct sockaddr *serverPtr = (struct sockaddr *) &server;
	struct hostent *rem;

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " socket created" << endl;
	UNLOCK(printMutex);

	/* Find server's address */
	if ((rem = gethostbyname(address)) == NULL) {
		herror("gethostbyname");
		exit(1);
	}
	port = args->port;
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);

	// Set SO_REUSEADDR for server restarts
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	/* Initiate connection */
	if (connect(sock, serverPtr, sizeof(server)) < 0)
		perror_exit("connect");
	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " Connected to " << address << ":" << port << endl;
	UNLOCK(printMutex);

	// Receive size, threadNo, iterationNo, hashNo, port
	uint32_t receive[5];
	for (int i = 0; i < 5; i++) {
		read(sock, &(receive[i]), sizeof(uint32_t));
		receive[i] = ntohl(receive[i]);
	}
	// Receive logfile length
	uint32_t len;
	read(sock, &len, sizeof(uint32_t));
	len = ntohs(len);
	// Receive logfile
	char *logfile = (char*) malloc(len + 1);
	read(sock, logfile, len + 1);
	// Receive seed
	uint32_t seed;
	read(sock, &seed, sizeof(uint32_t));
	seed = ntohs(seed);
	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " Got parameters" << endl;
	UNLOCK(printMutex);

	// Check parameters
	TransferParameters *parameters = new TransferParameters(receive[0], receive[1], receive[2], receive[3], receive[4], logfile, seed);
	if (!(args->equal(parameters))) {
		cout << RED << "ERROR" << RESET << "Parameters do not match" << endl;
		close(sock);
		exit(1);
	}

	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " Cross-checked" << endl;
	UNLOCK(printMutex);

	// Receive bloom filter bytes
	cout << "SIZE: " << args->size << endl;
	this->bloomFilter = new BloomFilter(args->size, args->hashNo);

	int bytesRead = 0;
	cout << "Bytes read" << endl;
	while (bytesRead < args->size) {
		int bytes;
		if ((bytes = read(sock, this->bloomFilter->bitArray + bytesRead, args->size)) == -1)
			perror_exit("read");
		bytesRead += bytes;
		cout << "\t" << bytesRead << endl;
	}

	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " Got bloom filter" << endl;
	UNLOCK(printMutex);

	// Free resources
	free(logfile);
	delete parameters;
	delete args;
	close(sock);

	LOCK(printMutex);
	cout << YEL << "Client" << RESET << " Finished" << endl;
	UNLOCK(printMutex);
}

Client::~Client() {}

BloomFilter* Client::getBloomFilter() {
	return this->bloomFilter;
}
