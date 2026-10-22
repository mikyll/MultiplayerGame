#pragma once

/*
* Minimal test helpers: CHECK records a failure and keeps going,
* TEST_RESULT is the exit code of the test executable.
*/

#include <stdio.h>

static int testFailures = 0;

#define CHECK(condition) \
    do { \
        if (!(condition)) \
        { \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #condition); \
            testFailures++; \
        } \
    } while (0)

#define TEST_RESULT (testFailures == 0 ? 0 : 1)
