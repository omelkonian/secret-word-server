/*
 * Search.cpp
 *
 *  Created on: May 6, 2015
 *      Author: orestis
 */

#include "Search.h"
#include <cstdlib>
#include <algorithm>
#include <pthread.h>
#include "lib/oracle.h"
#include "bloom_filter/BloomFilter.h"


#include <iostream>
#include <fstream>
using namespace std;

extern BloomFilter *bloomFilter;
extern pthread_mutex_t endMutex;
extern pthread_mutex_t logMutex;
extern pthread_mutex_t printMutex;
extern bool success;
extern int L;
extern int T;
extern string LOGFILE;

#define thread << BLU << "Thread " << pthread_self() << " " << RESET

Search::Search(unsigned int *seed) {
	this->seed = seed;
	this->searchCount = 0;
	this->wordsChecked = 0;
	this->wordsAlreadyChecked = 0;

	LOCK(printMutex);
	cout thread << "Starting search" << endl;
	UNLOCK(printMutex);
	this->go();
}

Search::~Search() {
	this->secretWord.clear();
	while (!this->words.empty()) {
		free(this->words.top());
		this->words.pop();
	}
}

void Search::go() {
	const char **oracleResults;
	char *cur;
	this->searchCount++;
	bool finished = false;
	int counter = 0, iterations = 0;

	// Construct initial random word
	char *initialWord = (char*) malloc(30);

	for (int i = 0; i < 5; i++)
		initialWord[i] = rand_r(this->seed)%16 + 65;
	initialWord[5] = '\0';

	this->words.push(initialWord);

	while (!this->words.empty()) {
		cur = this->words.top();
		this->words.pop();

		if (!bloomFilter->check_insert(cur)) { // only check words not previously given to oracle
			oracleResults = oracle(cur);
			counter++;
			if (oracleResults == NULL) {
				cout thread << "Secret word is " << cur << endl;
				this->secretWord = cur;

				// Notify other threads
				LOCK(endMutex);
				success = true;
				UNLOCK(endMutex);

				free(cur);
				this->finish();
			}
			int i = 0;
			while (oracleResults[i] != NULL)
				this->words.push((char*) oracleResults[i++]);
			free(oracleResults);
		}
		else
			this->wordsAlreadyChecked++;

		this->wordsChecked++;

		iterations++;
		if (counter == T) {
			// Check global boolean
			LOCK(endMutex);
			finished = success;
			UNLOCK(endMutex);
			counter = 0;
		}

		if (finished)
			break;

		free(cur);
	}

	if (this->searchCount == L || finished)
		this->finish();
	else
		this->go();
}

void Search::outputStatistics() {
	float percentage = (float(this->wordsAlreadyChecked)/float(this->wordsChecked))*100;
	ofstream log;
	LOCK(logMutex);
	cout thread << "Outputting statistics" << endl;
	log.open(LOGFILE.c_str(), ofstream::out | ofstream::app);
	log << "Thread " << pthread_self() << ":" << endl;
	log << "    Words checked: " << this->wordsChecked << endl;
	log << "    Words already checked percentage: " << percentage << " %" << endl;
	log << "    Secret word: " << this->secretWord << endl;
	log << endl;
	log.close();
	UNLOCK(logMutex);
}

void Search::finish() {
	LOCK(printMutex);
	cout thread << "Finished" << endl;
	UNLOCK(printMutex);
	this->outputStatistics();
	pthread_exit(0);
}
