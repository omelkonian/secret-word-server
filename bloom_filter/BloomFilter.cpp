/*
 * BloomFilter.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: Orestis Melkonian
 */

#include "BloomFilter.h"
#include <limits.h>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <set>
#include <algorithm>
#include "../shortcuts.h"

#include "../lib/hash.h"
using namespace std;

#define SECTIONSIZE 512 // in bits

BloomFilter::BloomFilter(int size, int hashNo) {
	this->sectionSize = SECTIONSIZE;
	this->hashNo = hashNo;
	this->size = size * CHAR_BIT;
	this->bitArray = (unsigned char*) malloc(size * sizeof(unsigned char));

	// Initialize mutexes
	int sectionNo = floor(float(this->size)/float(sectionSize));
	this->sectionNo = sectionNo;
	this->mutexes = (pthread_mutex_t*) malloc(sectionNo * sizeof(pthread_mutex_t));
	for (int i = 0; i < sectionNo; i++)
		pthread_mutex_init(&(this->mutexes[i]), NULL);
}

BloomFilter::~BloomFilter() {
	free(this->bitArray);
	// Destroy mutexes
	for (int i = 0; i < this->sectionNo; i++)
		pthread_mutex_destroy(&(this->mutexes[i]));
	free(this->mutexes);
}

int BloomFilter::getSize() {
	return this->size;
}


bool BloomFilter::getBit(int position) {
	assert(position < this->size && position >= 0);
	int index = position/CHAR_BIT;
	int bit = position % CHAR_BIT;
	return (this->bitArray[index] & (1 << bit));
}


void BloomFilter::setBit(int position, bool value) {
	assert(position < this->size && position >= 0);
	int index = position/CHAR_BIT;
	int bit = position % CHAR_BIT;
	if (value)
		this->bitArray[index] |= (1 << bit);
	else
		this->bitArray[index] &= ~(1 << bit);
}

void BloomFilter::insert(char *s) {
	// Get positions to set bit
	vector<int> positions;
	for (int i = 0; i < this->hashNo; i++) {
		int pos = hash_by(i, s) % this->size;
		if (find(positions.begin(), positions.end(), pos) == positions.end())
			positions.push_back(pos);
	}

	// Get corresponding sections
	vector<int> sections;
	for (int i = 0; i < positions.size(); i++) {
		int section = this->getSection(positions[i]);
		if (find(sections.begin(), sections.end(), section) == sections.end())
			sections.push_back(section);
	}

	// Sort sections in ascending order
	sort(sections.begin(), sections.end());

	// Lock sections
	for (int i = 0; i < sections.size(); i++)
		LOCK(this->mutexes[sections[i]]);

	// Set bits
	for (int i = 0; i < positions.size(); i++)
		this->setBit(positions[i], true);

	// Unlock sections
	for (int i = 0; i < sections.size(); i++)
		UNLOCK(this->mutexes[sections[i]]);

}

bool BloomFilter::check(char *s) {
	// Get positions to set bit
	vector<int> positions;
	for (int i = 0; i < this->hashNo; i++) {
		int pos = hash_by(i, s) % this->size;
		if (find(positions.begin(), positions.end(), pos) == positions.end())
				positions.push_back(pos);
	}

	// Get corresponding sections
	vector<int> sections;
	for (int i = 0; i < positions.size(); i++) {
		int section = this->getSection(positions[i]);
		if (find(sections.begin(), sections.end(), section) == sections.end())
			sections.push_back(section);
	}

	// Sort sections in ascending order
	sort(sections.begin(), sections.end());

	// Lock sections
	for (int i = 0; i < sections.size(); i++)
		LOCK(this->mutexes[sections[i]]);

	// Check bits
	bool ret = true;
	for (int i = 0; i < positions.size(); i++)
		if (!this->getBit(positions[i]))
			ret = false;

	// Unlock sections
	for (int i = 0; i < sections.size(); i++)
		UNLOCK(this->mutexes[sections[i]]);

	return ret;
}

bool BloomFilter::check_insert(char *s) {
	// Get positions to set bit
	vector<int> positions;
	for (int i = 0; i < this->hashNo; i++) {
		int pos = hash_by(i, s) % this->size;
		if (find(positions.begin(), positions.end(), pos) == positions.end())
				positions.push_back(pos);
	}

	// Get corresponding sections
	vector<int> sections;
	for (int i = 0; i < positions.size(); i++) {
		int section = this->getSection(positions[i]);
		if (find(sections.begin(), sections.end(), section) == sections.end())
			sections.push_back(section);
	}

	// Sort sections in ascending order
	sort(sections.begin(), sections.end());

	// Lock sections
	for (int i = 0; i < sections.size(); i++)
		LOCK(this->mutexes[sections[i]]);

	// Check bits
	bool ret = true;
	for (int i = 0; i < positions.size(); i++)
		if (!this->getBit(positions[i]))
			ret = false;


	if (ret == false) {
		// Insert - Set bits
		for (int i = 0; i < positions.size(); i++)
			this->setBit(positions[i], true);
	}

	// Unlock sections
	for (int i = 0; i < sections.size(); i++)
		UNLOCK(this->mutexes[sections[i]]);

	return ret;
}

void BloomFilter::lock() {
	for (int i = 0; i < this->sectionNo; i++)
		LOCK(this->mutexes[i]);
}

void BloomFilter::unlock() {
	for (int i = 0; i < this->sectionNo; i++)
		UNLOCK(this->mutexes[i]);
}

int BloomFilter::getSection(int position) {
	return floor(float(position)/float(this->sectionSize));
}

void BloomFilter::print() {
	for (int i = 0; i < 100; i++)
		cout << int(this->bitArray[i]);
	cout << endl;
}
