#include "timer.h"

Timer::Timer() {

}

Timer::~Timer() {

}

void Timer::start() {
	gettimeofday( &tv , 0 );
	m_start = ( double )tv.tv_sec * 1000.0 + ( double )tv.tv_usec / 1000.0;
}

double Timer::look() {
	gettimeofday( &tv , 0 );
	double end = ( double )tv.tv_sec * 1000.0 + ( double )tv.tv_usec / 1000.0;
	return end - m_start;
}