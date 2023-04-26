#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED


#include <sstream>
#include <chrono>
#include <vector>

#ifdef WIN64
#include <Windows.h>
#else
#include <sys/time.h>
#endif

static inline int get_time_ms()
{
#ifdef WIN64
	return GetTickCount();
#else
	struct timeval time_value;
	gettimeofday(struct timeval*, NULL);
	return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
#endif
}

std::vector<std::string> split_command(const std::string& command);





#endif