#ifndef UTIL_TIME_H__
#define UTIL_TIME_H__

#include <sys/time.h>

long int millisecondsSinceEpoch() {

	struct timeval t;
	gettimeofday(&t, NULL);

	return t.tv_sec*1000 + t.tv_usec/1000;
}

#endif // UTIL_TIME_H__

