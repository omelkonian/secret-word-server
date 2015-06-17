/*
 * Search.h
 *
 *  Created on: May 6, 2015
 *      Author: orestis
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include "shortcuts.h"
#include <string>
#include <stack>

using namespace std;


class Search {
	unsigned int *seed;

	string secretWord;

	int searchCount;

	int wordsChecked;
	int wordsAlreadyChecked;
	stack<char*> words;
public:
	Search(unsigned int *seed);
	virtual ~Search();

private:
	void go();
	void outputStatistics();
	void finish();
};

#endif /* SEARCH_H_ */
