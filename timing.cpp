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

	LOG_USER(timinglog) << "timing summary (user time):" << std::endl;

	for (int i = 0; i < _longestIdentifierLength; i++)
		LOG_USER(timinglog) << " ";
	LOG_USER(timinglog) << "\t# runs";
	LOG_USER(timinglog) << "\tmean";
	LOG_USER(timinglog) << "\tmin";
	LOG_USER(timinglog) << "\tmax";
	LOG_USER(timinglog) << "\tmedian";
	LOG_USER(timinglog) << "\ttotal" << std::endl;;

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

		for (int i = 0; i < _longestIdentifierLength - identifier.size(); i++)
			LOG_USER(timinglog) << " ";
		LOG_USER(timinglog) << identifier;
		LOG_USER(timinglog) << "\t" << numRuns;
		LOG_USER(timinglog) << "\t" << mean;
		LOG_USER(timinglog) << "\t" << min;
		LOG_USER(timinglog) << "\t" << max;
		LOG_USER(timinglog) << "\t" << median;
		LOG_USER(timinglog) << "\t" << total;
		LOG_USER(timinglog) << std::endl;
	}
}

void
TimingStatistics::add(const Timer& timer) {

	_timers[timer.getIdentifier()].push_back(timer);
	_longestIdentifierLength = std::max(_longestIdentifierLength, timer.getIdentifier().size());
}
