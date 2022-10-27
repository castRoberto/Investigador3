#ifndef _TIMESPEC_H_
#define _TIMESPEC_H_

#include <time.h>
#ifdef __MINGW32__
#include <pthread.h>
#include "time_mingw_patch.h"
#endif

#include <iostream>

/*
 * Set of operations and utilities related to the timespec struct.
 */

/**
 * Add two timespec's.
 */
struct timespec operator+ (struct timespec ts1, struct timespec ts2);

/**
 * Add a time in [us] to a timespec and returns it.
 */
struct timespec operator+ (struct timespec ts1, long useconds);

/**
 * Calculates the diference between two timespec's and return it in [us].
 */
long operator- (struct timespec ts1, struct timespec ts2);

/**
 * Convenient boolean operations.
 */
bool operator== (struct timespec ts1, struct timespec ts2);
bool operator< (struct timespec ts1, struct timespec ts2);
bool operator> (struct timespec ts1, struct timespec ts2);

/**
 * A convenient assignment operator fron long, in [us] and back.
 */
struct timespec us_to_timespec(long elapse_us);
long timespec_to_us(struct timespec ts);

/**
 * Convert to seconds in a double.
 */
double timespec_to_s(struct timespec ts);

/**
 * Returns current time.
 */
struct timespec abstime();

/**
 * (just to be completed)
 */
std::ostream& operator<< (std::ostream& os, const struct timespec& ts);

#endif /*_TIMESPEC_H_*/
