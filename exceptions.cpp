#include <boost/lexical_cast.hpp>

#include "demangle.h"
#include "exceptions.h"

stack_trace_::stack_trace_() {

	void*  stack[20];
	size_t size;

	size = backtrace(stack, 20);

	char** symbols = backtrace_symbols(stack, size);

	if (!symbols)
		return;

	_stack_trace.push_back(std::string("[trace] back trace:"));

	/* skip first stack frame (points here) */
	for (int i = 1; i < size; i++) {

		char* begin_name;

		// find beginning of function name part in symbol
		// ./module(function+0x15c) [0x8048a6d]
		for (begin_name = symbols[i]; *begin_name && *begin_name != '('; begin_name++);

		// extract function name only
		std::string funname;

		// could we find a '('?
		if (*begin_name) {

			// if we found a '(', the name starts right after it
			begin_name++;

			for (; *begin_name && *begin_name != '+'; begin_name++)
				funname += *begin_name;

			// could we find a '+'?
			if (!*begin_name)
				funname = "";
		}

		_stack_trace.push_back("[trace] #" + boost::lexical_cast<std::string>(i) + ": " + demangle(funname.c_str()));

		char command[256];
		sprintf(command, "addr2line %p -e %s", stack[i], get_program_name().c_str());
		FILE* pipe = popen(command, "r");

		if (pipe) {

			_stack_trace.push_back("[trace] in file: ");
			char line[1024];
			while (fgets(line, 1024, pipe))
				_stack_trace.back() += line;

			// remove the newline character from the end of the line
			if (_stack_trace.back().size() > 0)
				_stack_trace.back().erase(_stack_trace.back().size() - 1);
		}

		pclose(pipe);
	}

	free(symbols);
}

const std::vector<std::string>&
stack_trace_::get_stack_trace() const {

	return _stack_trace;
}

const std::string&
stack_trace_::get_program_name() {

	if (_program_name == "")
		initialise_program_name();

	return _program_name;
}

void
stack_trace_::initialise_program_name() {

	char link[1024];
	char name[1024];

	snprintf(link, sizeof link, "/proc/%d/exe", getpid());

	int size = readlink(link, name, sizeof link);
	if (size == -1) {

		_program_name = "[program name not found]";
		return;
	}

	for (int i = 0; i < size; i++)
		_program_name += name[i];
}
