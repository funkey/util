#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <execinfo.h>

#include <boost/exception/all.hpp>

/*
 * EXCEPTIONS
 */

// top level
struct Exception : virtual boost::exception, virtual std::exception {};

// top level per module
struct GuiError      : virtual Exception {};
struct PipelineError : virtual Exception {};
struct SignalsError  : virtual Exception {};

// general errors
struct IOError           : virtual Exception {};
struct SizeMismatchError : virtual Exception {};

/*
 * TAGS
 */

class stack_trace_ {

public:

	stack_trace_();

	std::string get_stack_trace();

private:

	std::string _stack_trace;
};

std::ostream& operator<<(std::ostream& out, stack_trace_& trace);

typedef boost::error_info<struct tag_error_message, std::string> error_message;

typedef boost::error_info<struct tag_stack_trace, stack_trace_>  stack_trace;
#define STACK_TRACE stack_trace(stack_trace_())

typedef boost::error_info<struct tag_mismatch_size1, int> mismatch_size1;
typedef boost::error_info<struct tag_mismatch_size2, int> mismatch_size2;

#endif // EXCEPTIONS_H__

