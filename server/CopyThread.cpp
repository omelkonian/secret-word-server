/*
 * CopyThread.cpp
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#include "CopyThread.h"

#include "TransferParameters.h"
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <math.h>
#include <arpa/inet.h>
#include "../shortcuts.h"

#include <iostream>
using namespace std;

extern pthread_mutex_t printMutex;

void handleSIGPIPE(int signo) {
	cout << RED << "ERROR" << RESET << "Parameters do not match" << endl;
	pthread_exit(NULL);
}

CopyThread::CopyThread(CopyArguments *args) {
	// Set SIGPIPE handler
	static struct sigaction act ;
	act.sa_handler = handleSIGPIPE;
	sigfillset(&(act.sa_mask));
	sigaction(SIGPIPE, &act, NULL);

	// Send size, threadNo, iterationNo, hashNo, port
	uint32_t transfer1[5] = {htonl(args->size), htonl(args->threadNo), htonl(args->iterationNo), htonl(args->hashNo), htonl(args->port)};
	for (int i = 0; i < 5; i++)
		write(args->newSocket, &(transfer1[i]), sizeof(uint32_t));
	// Send logfile length
	uint32_t len = htons(strlen(args->logfile));
	write(args->newSocket, &len, sizeof(uint32_t));
	// Send logfile
	write(args->newSocket, args->logfile, strlen(args->logfile) + 1);
	// Send seed
	uint32_t seed = htons(args->seed);
	write(args->newSocket, &seed, sizeof(uint32_t));
	cout << YEL << "Server" << RESET << " Parameters sent" << endl;

	// Lock bloom filter
	args->bloomFilter->lock();
	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " Bloom filter locked" << endl;
	UNLOCK(printMutex);

	/*
	 * Send bloomFilter bytes
	 */
	cout << "SIZE: " << args->size << endl;
	// Copy bloom filter to local space
	unsigned char *bloom = (unsigned char *) malloc(args->size * sizeof(unsigned char));
	memcpy(bloom, args->bloomFilter->bitArray, args->size);

	// Unlock bloom filter
	args->bloomFilter->unlock();
	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " Bloom filter unlocked" << endl;
	UNLOCK(printMutex);

	int bytesWritten = 0;
	cout << "Bytes written" << endl;
	while (bytesWritten < args->size) {
		int bytes;
		if ((bytes = write(args->newSocket, bloom + bytesWritten, 4096)) == -1)
			perror_exit("write");
		bytesWritten += bytes;
		cout << "\t" << bytesWritten << endl;
	}

	LOCK(printMutex);
	cout << YEL << "Server" << RESET << " Bloom filter sent" << endl;
	UNLOCK(printMutex);

	// Free resources
	free(bloom);
	delete args;
	close(args->newSocket);

	pthread_exit(NULL);
}

CopyThread::~CopyThread() {
}

