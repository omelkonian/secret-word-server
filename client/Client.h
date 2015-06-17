/*
 * Client.h
 *
 *  Created on: May 9, 2015
 *      Author: orestis
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "../server/CopyArguments.h"
#include "../server/TransferParameters.h"

class Client {
	char *address;
	BloomFilter *bloomFilter;
public:
	Client(char *address, CopyArguments *args);
	virtual ~Client();

	BloomFilter* getBloomFilter();
};

#endif /* CLIENT_H_ */
