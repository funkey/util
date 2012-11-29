#ifndef UTIL_SIGNAL_HANDLER_H__
#define UTIL_SIGNAL_HANDLER_H__

#include <signal.h>
#include "Logger.h"
#include "exceptions.h"

namespace util {

class SignalHandler {

public:

	static void init();

private:

	static void handle_signal(int signal);
};

/**
 * Callable to launch a thread with properly initialized signal handlers.
 */
struct ThreadLauncher {

	ThreadLauncher(const boost::function0<void>& fun_) :
		fun(fun_) {}

	void operator()() {

		SignalHandler::init();

		fun();
	}

	const boost::function0<void>& fun;
};

} // namespace util

#endif // UTIL_SIGNAL_HANDLER_H__

