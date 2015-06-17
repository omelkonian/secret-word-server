/*
 * TransferParameters.h
 *
 *  Created on: May 9, 2015
 *      Author: orestis
 */

#ifndef TRANSFERPARAMETERS_H_
#define TRANSFERPARAMETERS_H_

class TransferParameters {
public:
	int size;
	int threadNo;
	int iterationNo;
	int hashNo;
	int port;
	char *logfile;
	unsigned int seed;

	TransferParameters(int size, int threadNo, int iterationNo, int hashNo, int port, char *logfile, unsigned int seed);
	virtual ~TransferParameters();
};

#endif /* TRANSFERPARAMETERS_H_ */
