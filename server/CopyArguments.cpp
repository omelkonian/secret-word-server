/*
 * CopyArguments.cpp
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#include "CopyArguments.h"
#include <cstring>
#include <cstdlib>

#include <iostream>
using namespace std;

CopyArguments::CopyArguments(int size, int threadNo, int iterationNo, int hashNo, int port, char *logfile, unsigned int seed, BloomFilter *bloomFilter, int newSocket) {
	this->size = size;
	this->threadNo = threadNo;
	this->iterationNo = iterationNo;
	this->hashNo = hashNo;
	this->port = port;
	this->logfile = (char *) malloc(strlen(logfile) + 1);
	strcpy(this->logfile, logfile);
	this->seed = seed;
	this->bloomFilter = bloomFilter;
	this->newSocket = newSocket;
}
CopyArguments::~CopyArguments() {
	free(this->logfile);
}

bool CopyArguments::equal(TransferParameters* par) {
	if (size != par->size || threadNo != par->threadNo || iterationNo != par->iterationNo || hashNo != par->hashNo || port != par->port
	||  strcmp(logfile, par->logfile) != 0 || seed != par->seed)
		return false;
	return true;
}
