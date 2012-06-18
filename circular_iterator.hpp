#ifndef CIRCULAR_ITERATOR_H__
#define CIRCULAR_ITERATOR_H__

#include <iterator>

namespace util {

template<typename T>
class const_circular_iterator : public std::iterator<std::bidirectional_iterator_tag, typename T::value_type> {

private:

	typedef typename T::iterator   iterator_type;
	typedef typename T::value_type value_type;

protected:

	iterator_type begin;
	iterator_type end;
	iterator_type iterator;

public:

	const_circular_iterator(T& t) :
			iterator(t.begin()),
			begin(t.begin()),
			end(t.end())
			{};

	const_circular_iterator(iterator_type b, iterator_type e) :
			iterator(b),
			begin(b),
			end(e)
			{};

	const_circular_iterator<T>& operator++() {

		++iterator;

		if (iterator == end)
			iterator = begin;

		return (*this);
	}

	const_circular_iterator<T> operator++(int) {

		const_circular_iterator<T> t(*this);
		++(*this);
		return t;
	}

	const_circular_iterator<T>& operator--() {

		if (iterator == begin)
			iterator = end;

		--iterator;

		return (*this);
	}

	const_circular_iterator<T> operator--(int) {

		const_circular_iterator<T> t(*this);
		--(*this);
		return t;
	}

	const value_type operator*() const {
		return (*iterator);
	}

	bool operator==(const const_circular_iterator<T>& rhs) const {
		return (iterator == rhs.iterator);
	}

	bool operator!=(const const_circular_iterator<T>& rhs) const {
		return ! operator==(rhs);
	}
};

template<typename T>
class circular_iterator : public const_circular_iterator<T> {

private:

	typedef typename T::iterator   iterator_type;
	typedef typename T::value_type value_type;

public:

	circular_iterator(T& t) :
			const_circular_iterator<T>(t)
			{};

	circular_iterator(iterator_type b, iterator_type e) :
			const_circular_iterator<T>(b,e)
			{};

	const value_type operator*() const  {
		return *(this->iterator);
	}

	value_type& operator*() {
		return *(this->iterator);
	}
};

}; // namespace util

#endif // CIRCULAR_ITERATOR_H__
