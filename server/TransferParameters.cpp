/*
 * TransferParameters.cpp
 *
 *  Created on: May 9, 2015
 *      Author: orestis
 */

#include "TransferParameters.h"
#include <cstdlib>
#include <cstring>

TransferParameters::TransferParameters(int size, int threadNo, int iterationNo, int hashNo, int port, char *logfile, unsigned int seed) {
	this->size = size;
	this->threadNo = threadNo;
	this->iterationNo = iterationNo;
	this->hashNo = hashNo;
	this->port = port;
	this->logfile = (char *) malloc(strlen(logfile) + 1);
	strcpy(this->logfile, logfile);
	this->seed = seed;
}

TransferParameters::~TransferParameters() {
	free(this->logfile);
}

