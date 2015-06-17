/*
 * CopyThread.h
 *
 *  Created on: May 8, 2015
 *      Author: orestis
 */

#ifndef COPYTHREAD_H_
#define COPYTHREAD_H_

#include "CopyArguments.h"

class CopyThread {
public:
	CopyThread(CopyArguments *args);
	virtual ~CopyThread();
};

#endif /* COPYTHREAD_H_ */
