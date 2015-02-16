#ifndef UTIL_CONT_MAP_H__
#define UTIL_CONT_MAP_H__

#include <vector>
#include <map>
#include <stdexcept>

namespace util {

/**
 * Default converter that does nothing.
 */
template <typename T>
struct identity {

	typedef T TargetType;

	const TargetType& operator()(const T& t) const { return t; }
};

/**
 * Implements a std::map interface for keys that have a numerical interpretation 
 * and are expected to be continuous.
 *
 * Key-value pairs are internally stored as std::pair elements of a std::vector, 
 * such that the index i in the vector is the same as the numerical 
 * interpretation of the key (pair.first). Empty elements are represented by 
 * keys that are not the same as the index, but point to the next valid element 
 * for fast forward iteration. Consequently, backward iteration is slower than 
 * forward iteration.
 */
template <
		typename Key,
		typename T,
		typename NumConverter = identity<Key>,
		typename Alloc = std::allocator<std::pair<Key, T> > >
class cont_map {

public:

	// forward declaration
	template <typename Direction>
	class cont_map_iterator_base;
	class forward_direction;
	class backward_direction;

	typedef cont_map_iterator_base<forward_direction>  cont_map_iterator;
	typedef cont_map_iterator_base<backward_direction> cont_map_reverse_iterator;

	typedef cont_map<Key, T, NumConverter, Alloc> map_type;
	typedef typename NumConverter::TargetType num_key_type;

	// map interface
	typedef Key                                       key_type;
	typedef T                                         mapped_type;
	typedef std::pair<Key, T>                         value_type;
	typedef Alloc                                     allocator_type;
	typedef typename allocator_type::reference        reference;
	typedef typename allocator_type::const_reference  const_reference;
	typedef typename allocator_type::pointer          pointer;
	typedef typename allocator_type::const_pointer    const_pointer;
	typedef cont_map_iterator                         iterator;
	typedef const cont_map_iterator                   const_iterator;
	typedef cont_map_reverse_iterator                 reverse_iterator;
	typedef const cont_map_reverse_iterator           const_reverse_iterator;
	typedef std::vector<value_type, Alloc>            list_type;
	typedef typename list_type::difference_type       difference_type;
	typedef typename list_type::size_type             size_type;

	////////////////////////////////////////////////////////////////////////////////
	// iterator
	////////////////////////////////////////////////////////////////////////////////

	template <typename Direction>
	class cont_map_iterator_base : public Direction {

	public:

		typedef cont_map_iterator_base<Direction> iterator_type;

		cont_map_iterator_base(list_type& list, num_key_type i, NumConverter& converter) :
			Direction(list, i, converter),
			_list(list),
			_i(i),
			_converter(converter) {

			if (_i < 0)            _i = 0;
			if (_i > static_cast<num_key_type>(_list.size())) _i = _list.size();

			Direction::skip_invalids(_i);
		}

		value_type&       operator*()       { return _list[_i]; }
		const value_type& operator*() const { return _list[_i]; }

		      value_type* operator->()       { return &_list[_i]; }
		const value_type* operator->() const { return &_list[_i]; }

		iterator_type        operator++(int)       {       iterator_type p = *this; Direction::inc(_i); return p; }
		const iterator_type  operator++(int) const { const iterator_type p = *this; Direction::inc(_i); return p; }
		iterator_type&       operator++()          { Direction::inc(_i); return *this; }
		const iterator_type& operator++()    const { Direction::inc(_i); return *this; }

		bool operator==(const iterator_type& other) const { return _i == other._i; }
		bool operator!=(const iterator_type& other) const { return _i != other._i; }

		inline num_key_type index() { return _i; }

	private:

		list_type&            _list;
		mutable num_key_type  _i;
		NumConverter&         _converter;
	};

	class forward_direction {

	public:

		forward_direction(list_type& list, num_key_type&, NumConverter& converter) :
			_list(list),
			_converter(converter) {}

		void inc(num_key_type& i) const { i++; skip_invalids(i); }

	protected:

		num_key_type end() const { return _list.size(); }

		void skip_invalids(num_key_type& i) const {

			if (i == end())
				return;

			// keys of invalid elements point to the next valid one, keys of 
			// valid elements point to themselves
			i = _converter(_list[i].first);
		}

	private:

		list_type&            _list;
		NumConverter&         _converter;
	};

	class backward_direction {

	public:

		backward_direction(list_type& list, num_key_type& i, NumConverter& converter) :
			_list(list),
			_converter(converter) {

			// we represent element i by keeping an index to i+1 (this way we 
			// can have 0 as end)
			i++;
		}

		void inc(num_key_type& i) const { i--; skip_invalids(i); }

	protected:

		num_key_type end() const { return 0; }

		void skip_invalids(num_key_type& i) const {

			// traverse the list until we find a valid element
			while (i != end() && i-1 != _converter(_list[i-1].first))
				i--;
		}

	private:

		list_type&            _list;
		NumConverter&         _converter;
	};

	////////////////////////////////////////////////////////////////////////////////
	// member functions
	////////////////////////////////////////////////////////////////////////////////

	cont_map(const NumConverter& converter = NumConverter()) :
		_size(0),
		_converter(converter) {}

	// iterators
	iterator begin() { return iterator(_list, 0, _converter); }
	iterator end() { return iterator(_list, _list.size(), _converter); }
	const_iterator begin() const { return iterator(_list, 0, _converter); }
	const_iterator end() const { return iterator(_list, _list.size(), _converter); }
	reverse_iterator rbegin() { return reverse_iterator(_list, _list.size() - 1, _converter); }
	reverse_iterator rend() { return ++reverse_iterator(_list, 0, _converter); }
	const_reverse_iterator rbegin() const { return reverse_iterator(_list, _list.size() - 1, _converter); }
	const_reverse_iterator rend() const { return ++reverse_iterator(_list, 0, _converter); }

	// capacity
	bool empty() { return _size == 0; }
	size_type size() { return _size; }
	size_type max_size() { return _list.max_size(); }

	double overhead() { return (double)_list.size()/size(); }

	// element access
	inline mapped_type& operator[](const key_type& key) {

		num_key_type k = _converter(key);

		accomodate(k);

		// make valid
		if (!is_valid(k)) {
			reverse_iterator prev_valid(_list, k, _converter);
			for (num_key_type i = prev_valid.index(); i <= k; i++)
				_list[i].first = key;
			_size++;
		}

		return _list[k].second;
	}

	inline const mapped_type& operator[](const key_type& key) const {

		if (!count(key))
			throw std::out_of_range("util::cont_map::operator[]");

		return at(key);
	}

	inline const mapped_type& at(const key_type& key) const {

		num_key_type k = _converter(key);

		if (k >= _list.size() || k != _converter(_list[k].first))
			throw std::out_of_range("cont_map::at");

		return _list[k].second;
	}

	inline const mapped_type& at_index(const num_key_type& index) const { return _list[index].second; }
	inline mapped_type& at_index(const num_key_type& index)             { return _list[index].second; }

	// modifiers
	std::pair<iterator, bool> insert(const value_type& value) {

		num_key_type k = _converter(value.first);

		accomodate(k);
		bool contained = is_valid(k);

		_list[k] = value;
		if (!contained)
			_size++;

		return std::make_pair(iterator(_list, k, _converter), contained);
	}

	iterator insert(iterator /*position*/, const value_type& value) {
		return insert(value).first;
	}
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) {
			insert(*first);
			first++;
		}
	}

	void erase(iterator position) {

		if (position == end())
			return;

		if (!is_valid(position.index()))
			return;

		// find previous and next valid element or end
		iterator         next_valid = position;
		reverse_iterator prev_valid(_list, position.index(), _converter);
		next_valid++;
		prev_valid++;

		// update pointers to next valid
		for (num_key_type i = prev_valid.index(); i <= position.index(); i++)
			_list[i].first = _converter(next_valid.index());

		_size--;
	}

	size_type erase(const key_type& key) {

		iterator position(_list, _converter(key), _converter);
		if (position == end())
			return 0;

		erase(position);
		return 1;
	}

	void erase(iterator first, iterator last) {
		while (first != last) {
			erase(first);
			++first;
		}
	}

	void swap(map_type& other) {
		_list.swap(other._list);
		std::swap(_size, other._size);
		std::swap(_converter, other._converter);
	}

	void clear() {
		_list.clear();
		_size = 0;
	}

	// observers
	std::less<num_key_type> key_comp() { return std::less<num_key_type>(); }
	typename std::map<Key, T, Alloc>::value_compare value_comp() { return std::map<Key, T, Alloc>::value_comp(); }

	// operations
	iterator find(const key_type& key) {
		return __find<iterator>(key);
	}
	const_iterator find(const key_type& key) const {
		return __find<const_iterator>(key);
	}

	size_type count(const key_type& key) const {
		return _converter(key) < _list.size() && is_valid(_converter(key));
	}

	iterator lower_bound(const key_type& key) {
		return iterator(_list, _converter(key), _converter);
	}
	const_iterator lower_bound(const key_type& key) const {
		return const_iterator(_list, _converter(key), _converter);
	}

	iterator upper_bound(const key_type& key) {
		iterator i(_list, _converter(key), _converter);
		if (i.index() == _converter(key))
			i++;
		return i;
	}
	const_iterator upper_bound(const key_type& key) const {
		const_iterator i(_list, _converter(key), _converter);
		if (i.index() == _converter(key))
			i++;
		return i;
	}

	std::pair<iterator,iterator> equal_range(const key_type& key) {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}
	std::pair<const_iterator,const_iterator> equal_range(const key_type& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	// allocator
	allocator_type get_allocator() const { return _list.get_allocator(); }

private:

	inline bool is_valid(num_key_type k) const { return _converter(_list[k].first) == k; }

	// grow the list to accomodate keys with numerical value k
	inline void accomodate(num_key_type k) {

		// key is beyond current list size
		if (k >= static_cast<num_key_type>(_list.size())) {

			// create new fields at the end, with keys that point to the 
			// one-past last element (which is k+1)
			_list.resize(k+1, std::make_pair(_converter(k+1), T()));
		}
	}

	template <typename Iterator>
	inline Iterator __find(const key_type& key) {
		if (count(key))
			return Iterator(_converter(key));
		return end();
	}

	list_type    _list;
	size_type    _size;
	NumConverter _converter;
};

} // namespace util

#endif // UTIL_CONT_MAP_H__

