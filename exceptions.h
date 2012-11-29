#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <vector>
#include <execinfo.h>

#include <boost/exception/all.hpp>

/*
 * EXCEPTIONS
 */

// top level
struct Exception : virtual boost::exception, virtual std::exception {};

// top level per module
struct GuiError      : virtual Exception {};
struct SignalsError  : virtual Exception {};

// general errors
struct Segfault          : virtual Exception {};
struct IOError           : virtual Exception {};
struct SizeMismatchError : virtual Exception {};

/*
 * TAGS
 */

class stack_trace_ {

public:

	stack_trace_();

	const std::vector<std::string>& get_stack_trace() const;

private:

	const std::string& get_program_name();

	void initialise_program_name();

	std::vector<std::string> _stack_trace;

	std::string _program_name;
};

template <class OS>
std::ostream& operator<<(OS& out, const stack_trace_& trace) {

	for (unsigned int i = 0; i < trace.get_stack_trace().size(); i++)
		out << trace.get_stack_trace()[i] << std::endl;

	return out;
}

typedef boost::error_info<struct tag_error_message, std::string> error_message;

typedef boost::error_info<struct tag_stack_trace, stack_trace_>  stack_trace;
#define STACK_TRACE stack_trace(stack_trace_())

typedef boost::error_info<struct tag_mismatch_size1, int> mismatch_size1;
typedef boost::error_info<struct tag_mismatch_size2, int> mismatch_size2;

#endif // EXCEPTIONS_H__

