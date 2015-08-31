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

	typedef std::map<std::string, std::vector<Timer> > Timers;

	TimingStatistics();

	static void addTimer(const Timer& timer);

	~TimingStatistics();

private:

	void add(const Timer& timer);

	static TimingStatistics _instance;

	Timers _timers;

	size_t _longestIdentifierLength;
};

class Timer {

public:

	Timer(std::string identifier) :
		_identifier(identifier),
		_done(false) {

		_timer.start();
	}

	~Timer() {

		if (_done)
			return;

		_timer.stop();
		_done = true;

		TimingStatistics::addTimer(*this);
	}

	const std::string& getIdentifier() const { return _identifier; }

	/**
	 * Return the elapsed time in seconds.
	 */
	double elapsed() const;

private:

	std::string _identifier;

	boost::timer::cpu_timer _timer;

	bool _done;
};

#endif // UTIL_TIMING_H__

