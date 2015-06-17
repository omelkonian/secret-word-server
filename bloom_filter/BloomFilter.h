/*
 * BloomFilter.h
 *
 *  Created on: Feb 24, 2015
 *      Author: Orestis Melkonian
 */

#ifndef BLOOM_FILTER_BLOOMFILTER_H_
#define BLOOM_FILTER_BLOOMFILTER_H_

#include <stdint.h>
#include <string>

/*
 * Class representing an over-estimated set of all strings given to oracle.
 * 	- Interface using methods insert(), check().
 * 	- Implemented as a bit array, invisible to the outside classes.
 */
class BloomFilter {
	int sectionNo;
	int sectionSize;
	int hashNo;	// the number of hash functions to use
	int size;
	pthread_mutex_t *mutexes;
public:
	unsigned char *bitArray;

	BloomFilter(int size, int hashNo);
	virtual ~BloomFilter();

	int getSize();

	void insert(char *s); // inserts a string to the set by using the results of the hash functions
	bool check(char *s); // checks if a string has been previously given to oracle

	bool check_insert(char *s); // inserts a string to the set only if has not been previously given to oracle

	void lock();
	void unlock();

	void print();
private:
	bool getBit(int position);
	void setBit(int position, bool value);
	int getSection(int position);
};

#endif /* BLOOM_FILTER_BLOOMFILTER_H_ */
