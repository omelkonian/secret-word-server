/*
 * shortcuts.h
 *
 *  Created on: May 6, 2015
 *      Author: orestis
 */

#ifndef SHORTCUTS_H_
#define SHORTCUTS_H_

#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

#include <iostream>
using namespace std;


#define RED  "\033[22;31m"
#define GRN  "\033[22;32m"
#define YEL  "\033[22;33m"
#define BLU  "\033[22;34m"
#define MAG  "\033[22;35m"
#define CYA  "\033[22;36m"
#define RESET "\033[0m"

#define perror_exit(msg) {perror(msg); exit(1);}
#define perror2(s, e) fprintf(stderr, "%s : %s\n", s, strerror(e));

#define LOCK(mtx) {int err; if((err = pthread_mutex_lock(&mtx))){perror2("pthread_mutex_lock",err);exit(1);}}
#define UNLOCK(mtx) {int err; if((err = pthread_mutex_unlock(&mtx))){perror2("pthread_mutex_unlock",err);exit(1);}}

#define CREATE(t, function, arg) {int err;if((err=pthread_create(&t,NULL,function,arg))){perror2("pthread_create",err);exit(1);}}
#define JOIN(t,toDelete) {int err;if ((err = pthread_join(t,toDelete))){perror2("pthread_join", err);exit(1);}}
#define DETACH(t) {int err;if((err=pthread_detach(t)) != 0){perror2("pthread_detach",err);exit(1);}}

#define pow2roundup(x) { --x; x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16; x++; }

#endif /* SHORTCUTS_H_ */
