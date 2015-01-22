#ifndef UTIL_CONT_MAP_H__
#define UTIL_CONT_MAP_H__

namespace util {

/**
 * Default converter that does nothing.
 */
template <typename T>
struct identity {

	typedef T TargetType;

	static const TargetType& operator()(const T& t) { return t; }
};

/**
 * Implements a std::map interface for keys that have a numerical interpretation 
 * and are expected to be continuous.
 */
template <
		typename Key,
		typename T,
		typename NumConverter = identity<Key>,
		typename Alloc = std::allocator<std::pair<const typename NumConverter::TargetType, T> > >
class cont_map {

public:

	// forward declaration
	class cont_map_iterator;

	typedef cont_map<Key, T, NumConverter, Alloc> map_type;
	typedef typename NumConverter::TargetType num_key_type;

	// map interface
	typedef Key                              key_type;
	typedef T                                mapped_type;
	typedef std::pair<const Key, T>          value_type;
	typedef Alloc                            allocator_type;
	typedef allocator_type::reference        reference;
	typedef allocator_type::const_reference  const_reference;
	typedef allocator_type::pointer          pointer;
	typedef allocator_type::const_pointer    const_pointer;
	typedef cont_map_iterator                iterator;
	typedef const cont_map_iterator          const_iterator;
	typedef cont_map_reverse_iterator        reverse_iterator;
	typedef const cont_map_reverse_iterator  const_reverse_iterator;
	typedef std::vector<value_type, Alloc>   list_type;
	typedef list_type::difference_type       difference_type;
	typedef list_type::size_type             size_type;

	////////////////////////////////////////////////////////////////////////////////
	// iterator
	////////////////////////////////////////////////////////////////////////////////

	class cont_map_iterator {

	public:

		cont_map_iterator(list_type& list, num_key_type i, NumConverter& converter) :
			_list(list),
			_i(std::min(i, end())),
			_converter(converter) {

			skip_invalids();
		}

		mapped_type& operator*() { return _list[i]; }
		const mapped_type& operator*() const { return _list[i]; }

		iterator& operator++() { _i++; skip_invalids(); return *this; }
		const_iterator& operator++() const { _i++; skip_invalids(); return *this; }

	private:

		void skip_invalids() {

			// i == end?
			if (_i == end())
				return;

			// keys of invalid elements point to the next valid one, keys of 
			// valid elements point to themselves
			_i = _converter(_list[i].first);
		}

		inline num_key_type end() { return _list.size(); }

		map_type&             _list;
		mutable num_key_type  _i;
		mutable NumConverter& _converter;
	};

	////////////////////////////////////////////////////////////////////////////////
	// member functions
	////////////////////////////////////////////////////////////////////////////////

	cont_map(const NumConverter& converter = NumConverter()) :
		_converter(converter) {}

	// capacity
	bool empty() { return _list.empty(); }
	size_type size() { return _list.size(); }
	size_type max_size() { return _list.max_size(); }

	// element access
	mapped_type& operator[](const key_type& key) {

		num_key_type k = _converter(key);

		// key is beyond current list size
		if (k >= _list.size()) {

			// create new fields at the end, with keys that point to the last 
			// element (which is k)
			_list.resize(k, std::make_pair(_converter(k), T()));
		}

		// key is not assignmed, yet
		if (k != _converter(_list[k].first)) {

			_list[k].first = key;
		}

		return _list[k].second;
	}

	const mapped_type& at(const key_type& key) const {

		num_key_type k = _converter(key);

		if (k >= _list.size() || k != _converter(_list[k].first))
			throw std::out_of_range("cont_map::at");

		return _list[k].second;
	}

	// modifiers
	std::pair<iterator, bool> insert(const value_type& value) {

		num_key_type k = _converter(value.first);

		// key is beyond current list size
		if (k >= _list.size()) {

			// create new fields at the end, with keys that point to the end of 
			// the list
			_list.resize(k, std::make_pair(_converter(k), T()));
		}

		bool contained = is_valid(k);
		_list[k] = value;

		return std::make_pair(iterator(_list, k, _converter), contained);
	}

	iterator insert(iterator position, const value_type& value) {
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

		if (!is_valid(position.index))
			return;

		// find next valid element or end
		num_key_type i = position.index + 1;
		while (!is_valid(i))
			i++;

		// invalidate position
		position->first = _converter(i);
	}

	size_type erase(const key_type& key) {

		iterator position(_list, _converter(key), _converter);
		if (!is_valid(position))
			return 0;

		erase(position);
		return 1;
	}

	void erase(iterator first, iterator last) {
		while (first != last) {
			erase(first);
			first++;
		}
	}

	void swap(map_type& other) {
		_list.swap(other._list);
		std::swap(_converter, other._converter);
	}

	void clear() {
		_list.clear();
	}

	// observers
	std::less<num_key_type> key_comp() { return std::less<num_key_type>(); }
	std::map<Key, T, Alloc>::value_comp() { return std::map<Key, T, Alloc>::value_comp(); }

	// operations
	iterator find(const key_type& key) {
		return __find<iterator>(key);
	}
	const_iterator find(const key_type& key) const {
		return __find<const_iterator>(key);
	}

	size_type count(const key_type& key) {
		return is_valid(_converter(key));
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
		return std::make_pair(lower_bound(), upper_bound());
	}
	std::pair<const_iterator,const_iterator> equal_range(const key_type& key) const {
		return std::make_pair(lower_bound(), upper_bound());
	}

	// allocator
	allocator_type get_allocator() const { return _list.get_allocator(); }

private:

	inline bool is_valid(num_key_type k) { return (k < _list.size() && _converter(_list[k].first) == k;) }

	template <typename Iterator>
	inline Iterator __find(const key_type& key) {
		num_key_type k = _converter(key);
		if (is_valid(k))
			return Iterator(k);
		return end();
	}

	list_type    _list;
	NumConverter _converter;
};

} // namespace util

#endif // UTIL_CONT_MAP_H__

