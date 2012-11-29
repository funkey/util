#include "SignalHandler.h"

namespace util {

logger::LogChannel signalhandlerlog("signalhandlerlog", "[SignalHandler] ");

void
SignalHandler::init() {

	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGSEGV, &sa, 0);
	sigaction(SIGUSR1, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
}

void
SignalHandler::handle_signal(int signal) {

	if (signal == SIGSEGV) {

		LOG_ERROR(signalhandlerlog) << "got segfault at:" << std::endl;
		LOG_ERROR(signalhandlerlog) << stack_trace_() << std::endl;

		BOOST_THROW_EXCEPTION(Segfault() << error_message("segfault") << STACK_TRACE);

	} else {

		LOG_USER(signalhandlerlog) << "got signal " << signal << " at:" << std::endl;
		LOG_USER(signalhandlerlog) << stack_trace_() << std::endl;
	}
}

} // namespace util
