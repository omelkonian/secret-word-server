/*
 * Server.h
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "../bloom_filter/BloomFilter.h"
#include "CopyArguments.h"

class Server {
	int size;			// SIZE
	int threadNo; 		// N
	int iterationNo; 	// L
	int hashNo;			// NUM
	int port;			// PORT
	char *logfile;		// LOGFILE
	unsigned int seed;	// oracle initial seed

	BloomFilter *bloomFilter;
public:
	Server(CopyArguments *args);
	virtual ~Server();
};

#endif /* SERVER_H_ */
