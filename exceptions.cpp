#include "exceptions.h"

stack_trace_::stack_trace_() {

	void*  stack[20];
	size_t size;

	size = backtrace(stack, 20);

	char** symbols = backtrace_symbols(stack, size);

	_stack_trace += "\n";
	for (int i = 0; i < size; i++) {

		_stack_trace += symbols[i];
		_stack_trace += "\n";
	}
}

std::string
stack_trace_::get_stack_trace() {

	return _stack_trace;
}

std::ostream& operator<<(std::ostream& out, stack_trace_& trace) {

	return (out << trace.get_stack_trace());
}
