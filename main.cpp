/*
 * main.cpp
 *
 *  Created on: May 6, 2015
 *      Author: orestis
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "lib/oracle.h"
#include <pthread.h>
#include <time.h>
#include <vector>

#include "bloom_filter/BloomFilter.h"
#include "Search.h"
#include "shortcuts.h"
#include "server/Server.h"
#include "client/Client.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <unistd.h>

using namespace std;



/*
 * GLOBAL VARIABLES
 */

// Arguments
int SIZE;					// size of bloom filter
int N;						// number of threads
int L;						// number of iterations per thread
int PORT;					// listening port
char *LOGFILE;				// log filename
int HASHNO;					// number of hash functions
char *ADDRESS;				// network address of server thread
unsigned int ORACLESEED; 	// oracle's initial seed
bool NOSERVE;				// do not serve clients
int DIFFICULTY;				// set oracle mode
int T;						// check success boolean after every T oracle calls



// Global seeds
unsigned int *seeds;

// Mutexes
pthread_mutex_t logMutex;
pthread_mutex_t endMutex;

pthread_mutex_t printMutex;

bool success;

// Data structures
BloomFilter *bloomFilter;

void* initServer(void *arguments) {
	Server *server = new Server((CopyArguments*)arguments);
	return NULL;
}

void* search(void *arguments) {
	Search *search = new Search((unsigned int *)arguments);
	return search;
}

int main(int argc, char **argv) {
	T = 200;
	pthread_mutex_init(&printMutex, NULL);

	/*
	 * Check and get arguments
	 */
	if (argc < 6 || argc > 15) {
		cout << "Usage: ./invoke-oracle-multithreaded SIZE N L PORT LOGFILE [-k NUM] [-h ADDRESS] [-s SEED] [-d DIFFICULTY] [-noserve]" << endl;
		return -1;
	}
	SIZE = atoi(argv[1]);
	pow2roundup(SIZE); // Convert to nearest greater power of 2
	N = atoi(argv[2]);
	L = atoi(argv[3]);
	PORT = atoi(argv[4]);
	LOGFILE = argv[5];
	HASHNO = 3;
	ADDRESS = "";
	ORACLESEED = 666;
	NOSERVE = false;
	DIFFICULTY = 1;
	int arg = 6;
	while (argv[arg] != NULL) {
		if (strcmp(argv[arg], "-k") == 0)
			HASHNO = atoi(argv[arg + 1]);
		else if (strcmp(argv[arg], "-h") == 0)
			ADDRESS = argv[arg + 1];
		else if (strcmp(argv[arg], "-s") == 0)
			ORACLESEED = atoi(argv[arg + 1]);
		else if (strcmp(argv[arg], "-d") == 0)
			DIFFICULTY = atoi(argv[arg + 1]);
		else {
			NOSERVE = true;
			arg++;
			continue;
		}
		arg += 2;
	}

	/*
	 * Client
	 */
	if (strcmp(ADDRESS, "") != 0) {
		Client *client = new Client(ADDRESS, new CopyArguments(SIZE, N, L, HASHNO, PORT, LOGFILE, ORACLESEED, NULL, 0));
		bloomFilter = client->getBloomFilter();
		delete client;
	}
	else
		bloomFilter = new BloomFilter(SIZE, HASHNO);


	/*
	 * Initialize logfile
	 */
	ofstream log;
	log.open(LOGFILE, ofstream::trunc);
	log.close();
	cout << RED << "Main " << RESET << "Logfile initialized" << endl;

	/*
	 * Initialize random seeds
	 */
	seeds = (unsigned int *) malloc(N * sizeof(unsigned int));
	for (int i = 0; i < N; i++)
		seeds[i] = time(NULL) + i;
		// seeds[i] = i; /* for time measurements */
		


	cout << RED << "Main " << RESET << "Seeds initialized" << endl;

	/*
	 * Initialize oracle
	 */
	switch (DIFFICULTY) {
	case 0:	//EASY
		setEasyMode();
		break;
	case 1:	// NONE
		break;
	case 2: // HARD
		setHardMode();
		break;
	}
	initSeed(ORACLESEED);
	initAlloc(malloc);
	cout << RED << "Main " << RESET << "Oracle initialized" << endl;

	/*
	 * Create server thread
	 */
	pthread_t server;
	if (!NOSERVE) {
		CREATE(server, initServer, (void*) new CopyArguments(SIZE, N, L, HASHNO, PORT, LOGFILE, ORACLESEED, bloomFilter, 0));
		cout << RED << "Main " << RESET << "Server initialized" << endl;
	}

	/*
	 * Create threads
	 */
	pthread_mutex_init(&logMutex, NULL);
	pthread_mutex_init(&endMutex, NULL);
	vector<pthread_t> threadIDs;
	for (int i = 0; i < N; i++) {
		pthread_t threadID;
		CREATE(threadID, search, &seeds[i]);
		threadIDs.push_back(threadID);
	}
	LOCK(printMutex);
	cout << RED << "Main " << RESET << "Threads created" << endl;
	UNLOCK(printMutex);

	/*
	 * Join threads
	 */
	for (int i = 0; i < (int)threadIDs.size(); i++) {
		Search *toDelete;
		JOIN(threadIDs[i], (void**)&toDelete);
		delete toDelete;
	}
	LOCK(printMutex);
	cout << RED << "Main " << RESET << "Threads terminated" << endl;
	UNLOCK(printMutex);

	if (!NOSERVE)
		JOIN(server, NULL);

	/*
	 * Free resources
	 */
	delete bloomFilter;
	pthread_mutex_destroy(&logMutex);
	pthread_mutex_destroy(&endMutex);
	free(seeds);
	threadIDs.clear();

	return 0;
}

