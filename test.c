/*
 * File name   : test.c
 * Author      : lanmeng
 * Date        : Created on 2022/11/19.
 * Description : 
 * Others      : No supplementary explanation for the time being.
 */

#include "clog.h"
#include "test.h"
#include <stdio.h>

// #define CLOG_TEST_THREAD

void test() {
#ifndef CLOG_TEST_THREAD
    printf("clog test no thread\n");
    clogTest();
#else
    printf("clog test thread\n");
    // clogThreadTest();
#endif
}
 