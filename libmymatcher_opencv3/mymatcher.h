#ifndef MYMATCHER_H
#define MYMATCHER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "stdint.h"
int test_add(int a, int b);
void test_picture(uint8_t * frame, void * p);
void * createMatcher(int rows, int cols, int type);
void deleteMatcher(void * p);
void initMatcher(uint8_t * frame, int * bbox, void * p);
double matchMatcher(uint8_t * frame, int * bbox, void * p);

#ifdef __cplusplus
}
#endif

#endif