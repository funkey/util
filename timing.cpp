#include <algorithm>
#include <iostream>
#include <iomanip>
#include "timing.h"

TimingStatistics TimingStatistics::_instance;

double
Timer::elapsed() const {

	boost::chrono::nanoseconds ns(_timer.elapsed().wall);
	boost::chrono::duration<double> s = ns;

	return s.count();
}
TimingStatistics::TimingStatistics() :
	_longestIdentifierLength(0) {}

void
TimingStatistics::addTimer(const Timer& timer) {

	_instance.add(timer);
}

TimingStatistics::~TimingStatistics() {

	if (_timers.size() == 0)
		return;

	const std::string spacer("   ");

	std::cout
			<< "timing summary in seconds (wall time):"
			<< std::endl << std::endl;

	for (int i = 0; i < _longestIdentifierLength; i++)
		std::cout << " ";
	std::cout << spacer;
	std::cout << "   # runs" << spacer;
	std::cout << "mean     " << spacer;
	std::cout << "min      " << spacer;
	std::cout << "max      " << spacer;
	std::cout << "median   " << spacer;
	std::cout << "total" << std::endl << std::endl;

	for (const Timers::value_type& p : _timers) {

		const std::string&        identifier = p.first;
		const std::vector<Timer>& timers     = p.second;

		int numRuns = timers.size();
		double total = 0;
		std::vector<double> times;

		for (const Timer& t : timers) {

			total += t.elapsed();
			times.push_back(t.elapsed());
		}

		double mean = total/numRuns;

		std::nth_element(times.begin(), times.begin(), times.end());
		double min    = times[0];
		std::nth_element(times.begin(), times.begin() + numRuns - 1, times.end());
		double max    = times[numRuns - 1];
		std::nth_element(times.begin(), times.begin() + numRuns/2, times.end());
		double median = times[numRuns/2];

		std::cout << identifier;
		for (int i = 0; i < _longestIdentifierLength - identifier.size(); i++)
			std::cout << " ";
		std::cout << spacer;
		std::cout << std::setw(9) << std::setfill(' ');
		std::cout << numRuns << spacer;
		std::cout << std::scientific << std::setprecision(3);
		std::cout << mean << spacer;
		std::cout << min << spacer;
		std::cout << max << spacer;
		std::cout << median << spacer;
		std::cout << total;
		std::cout << std::endl;
	}
}

void
TimingStatistics::add(const Timer& timer) {

	_timers[timer.getIdentifier()].push_back(timer);
	_longestIdentifierLength = std::max(_longestIdentifierLength, timer.getIdentifier().size());
}
