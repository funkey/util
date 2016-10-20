#ifndef UTIL_CACHE_HPP__
#define UTIL_CACHE_HPP__

#include <map>
#include <queue>
#include "assert.h"

class size_limit_policy {

public:

	size_limit_policy() : _max_size(256) {}

	void set_max_size(size_t size) { _max_size = size; }

protected:

	template <typename K, typename V>
	bool limit_exceeded(const std::map<K,V>& cache) const {

		return (cache.size() > _max_size);
	}

private:

	size_t _max_size;
};

template <typename K, typename V>
class eliminate_oldest_first {

protected:

	void notify_put(const K& k, const V&) {

		_keys.push(k);
	}

	void eliminate_item(std::map<K,V>& cache) {

		UTIL_ASSERT_REL(cache.size(), ==, _keys.size())
		UTIL_ASSERT_REL(cache.size(), >, 0)

		K oldest = _keys.front();
		_keys.pop();

		UTIL_ASSERT_REL(cache.count(oldest), ==, 1)

		cache.erase(oldest);
	}

private:

	std::queue<K> _keys;
};

template <typename K, typename V, typename LimitPolicy = size_limit_policy, typename EliminationPolicy = eliminate_oldest_first<K,V>>
class cache : public LimitPolicy, public EliminationPolicy {

public:

	template <typename Factory>
	V get(const K& k, const Factory& factory) {

		if (_cache.count(k))
			return _cache[k];

		V v = factory();
		put(k, v);

		while (LimitPolicy::limit_exceeded(_cache))
			EliminationPolicy::eliminate_item(_cache);

		return v;
	}

	size_t size() const { return _cache.size(); }

	bool clear() { if (!size()) return false; _cache.clear(); return true; }

private:

	void put(const K& k, V v) {

		_cache[k] = v;
		EliminationPolicy::notify_put(k, v);
	}

	std::map<K,V> _cache;
};

#endif // UTIL_CACHE_HPP__

