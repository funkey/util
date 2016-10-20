#ifndef UTIL_TIMING_H__
#define UTIL_TIMING_H__

#include <map>
#include <vector>
#include <boost/timer/timer.hpp>
#include "typename.h"

#define UTIL_TIME_SCOPE(name) Timer __util_scope_timer(name);
#define UTIL_TIME_METHOD      Timer __util_method_timer(typeName(*this) + "::" + __FUNCTION__ + "()");

class Timer;

class TimingStatistics {

public:

	typedef std::map<std::string, std::vector<float>> Times;

	TimingStatistics();

	static void addTimer(const std::string& identifier, float elapsed);

	~TimingStatistics();

private:

	static TimingStatistics _instance;

	Times _times;
};

class Timer {

public:

	Timer(std::string identifier) :
		_identifier(identifier) {

		_timer.start();
	}

	~Timer() {

		_timer.stop();
		TimingStatistics::addTimer(getIdentifier(), elapsed());
	}

	const std::string& getIdentifier() const { return _identifier; }

	/**
	 * Return the elapsed time in seconds.
	 */
	float elapsed() const;

private:

	std::string _identifier;

	boost::timer::cpu_timer _timer;
};

#endif // UTIL_TIMING_H__

