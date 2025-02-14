#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <unistd.h>
#include <iostream>


class Timer {
private:
	timeval tv;
	double m_start;

public:

	Timer();
	~Timer();
	void start();
	double look();
};

#endif // TIMER_H
