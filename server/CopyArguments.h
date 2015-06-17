/*
 * CopyArguments.h
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#ifndef COPYARGUMENTS_H_
#define COPYARGUMENTS_H_

#include "../bloom_filter/BloomFilter.h"
#include "TransferParameters.h"

class CopyArguments {
public:
	int size;			// SIZE
	int threadNo; 		// N
	int iterationNo; 	// L
	int hashNo;			// NUM
	int port;			// PORT
	char *logfile;		// LOGFILE
	unsigned int seed;	// oracle initial seed

	BloomFilter *bloomFilter;

	int newSocket;

	CopyArguments(int size, int threadNo, int iterationNo, int hashNo, int port, char *logfile, unsigned int seed, BloomFilter *bloomFilter, int newSocket);
	virtual ~CopyArguments();

	bool equal(TransferParameters *par);
};

#endif /* COPYARGUMENTS_H_ */
