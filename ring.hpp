#ifndef RING_H_
#define RING_H_

#include <vector>

#include "circular_iterator.hpp"

namespace util {

/**
 * Class ring.
 *
 * This class realizes a ring buffer of a fixed size. As soon as more elements
 * are added to the buffer, the least recent elements are removed.
 */

template<typename T>
class ring {

public:

	typedef std::vector<T>                          container_type;
	typedef typename container_type::iterator       container_iterator;
	typedef circular_iterator<container_type>       iterator;
	typedef const_circular_iterator<container_type> const_iterator;

	ring(int capacity) :
		_buffer(capacity+1),
		_head(_buffer.begin(),_buffer.end()),
		_tail(_buffer.begin(),_buffer.end())
		{};

	const const_circular_iterator<container_type> begin() const {
		return _head;
	}

	const const_circular_iterator<container_type> end() const {
		return _tail;
	}

	circular_iterator<container_type> begin() {
		return _head;
	}

	circular_iterator<container_type> end() {
		return _tail;
	}

	void push_front(const T& t) {

		_head--;
		(*_head) = t;

		if (_head == _tail)
			_tail--;
	}

	void push_back(const T& t) {

		(*_tail) = t;
		_tail++;

		if (_tail == _head)
			_head++;
	}

	void pop_front() {

		if (_head != _tail)
			_head++;
	}

	void pop_back() {

		if (_tail != _head)
			_tail--;
	}

	void clear() {
		_tail = _head;
	}

private:

	// the fixed size buffer to store the elements of the ring
	container_type    _buffer;

	// head and tail mark the current beginning and end of the ring
	circular_iterator<container_type> _head;
	circular_iterator<container_type> _tail;
};

}; // namespace util

#endif // RING_H_
