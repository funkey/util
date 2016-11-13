#include <algorithm>
#include <iostream>
#include <iomanip>
#include <boost/chrono.hpp>
#include "timing.h"

TimingStatistics TimingStatistics::_instance;

float
Timer::elapsed() const {

	boost::chrono::nanoseconds ns(_timer.elapsed().system + _timer.elapsed().user);
	boost::chrono::duration<float> s = ns;

	return s.count();
}
TimingStatistics::TimingStatistics() {}

void
TimingStatistics::addTimer(const std::string& identifier, float elapsed) {

	_instance._times[identifier].push_back(elapsed);
}

TimingStatistics::~TimingStatistics() {

	if (_times.size() == 0)
		return;

	const std::string spacer("   ");

	std::cout
			<< "timing summary in seconds (cpu time):"
			<< std::endl << std::endl;

	int longestIdentifierLength = 0;
	for (const Times::value_type& p : _times) {

		const std::string&  identifier = p.first;
		longestIdentifierLength = std::max(longestIdentifierLength, (int)identifier.size());
	}

	for (int i = 0; i < longestIdentifierLength; i++)
		std::cout << " ";
	std::cout << spacer;
	std::cout << "   # runs" << spacer;
	std::cout << "mean     " << spacer;
	std::cout << "min      " << spacer;
	std::cout << "max      " << spacer;
	std::cout << "median   " << spacer;
	std::cout << "total" << std::endl << std::endl;

	for (Times::value_type& p : _times) {

		const std::string&  identifier = p.first;
		std::vector<float>& times      = p.second;

		int numRuns = times.size();
		float total = 0;

		for (float t : times)
			total += t;

		float mean = total/numRuns;

		std::nth_element(times.begin(), times.begin(), times.end());
		float min    = times[0];
		std::nth_element(times.begin(), times.begin() + numRuns - 1, times.end());
		float max    = times[numRuns - 1];
		std::nth_element(times.begin(), times.begin() + numRuns/2, times.end());
		float median = times[numRuns/2];

		std::cout << identifier;
		for (int i = 0; i < longestIdentifierLength - identifier.size(); i++)
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
