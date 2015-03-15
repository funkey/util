#include "timing.h"
#include "Logger.h"

logger::LogChannel timinglog("timinglog", "[TimingStatistics] ");

TimingStatistics TimingStatistics::_instance;

double
Timer::elapsed() const {

	boost::chrono::nanoseconds ns(_timer.elapsed().user + _timer.elapsed().system);
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

	LOG_USER(timinglog)
			<< "timing summary in seconds (user + system time):"
			<< std::endl << std::endl;

	for (int i = 0; i < _longestIdentifierLength; i++)
		LOG_USER(timinglog) << " ";
	LOG_USER(timinglog) << spacer;
	LOG_USER(timinglog) << "   # runs" << spacer;
	LOG_USER(timinglog) << "mean     " << spacer;
	LOG_USER(timinglog) << "min      " << spacer;
	LOG_USER(timinglog) << "max      " << spacer;
	LOG_USER(timinglog) << "median   " << spacer;
	LOG_USER(timinglog) << "total" << std::endl << std::endl;

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

		std::nth_element(times.begin(), times.end(), times.begin());
		std::nth_element(times.begin(), times.end(), times.begin() + numRuns - 1);
		std::nth_element(times.begin(), times.end(), times.begin() + numRuns/2);

		double min    = times[0];
		double max    = times[numRuns - 1];
		double median = times[numRuns/2];

		LOG_USER(timinglog) << identifier;
		for (int i = 0; i < _longestIdentifierLength - identifier.size(); i++)
			LOG_USER(timinglog) << " ";
		LOG_USER(timinglog) << spacer;
		LOG_USER(timinglog) << std::setw(9) << std::setfill(' ');
		LOG_USER(timinglog) << numRuns << spacer;
		LOG_USER(timinglog) << std::scientific << std::setprecision(3);
		LOG_USER(timinglog) << mean << spacer;
		LOG_USER(timinglog) << min << spacer;
		LOG_USER(timinglog) << max << spacer;
		LOG_USER(timinglog) << median << spacer;
		LOG_USER(timinglog) << total;
		LOG_USER(timinglog) << std::endl;
	}
}

void
TimingStatistics::add(const Timer& timer) {

	_timers[timer.getIdentifier()].push_back(timer);
	_longestIdentifierLength = std::max(_longestIdentifierLength, timer.getIdentifier().size());
}
