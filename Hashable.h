#ifndef UTIL_HASHABLE_H__
#define UTIL_HASHABLE_H__

/**
 * Template base class providing hashing functionality for a derived type T.  
 * hash_value(const T&) has to be defined.
 */
template <typename T, typename HashType>
class Hashable {

public:

	Hashable() : _hashDirty(true) {}

	/**
	 * Get the hash value of this object. The hash will be computed if it is not 
	 * up-to-date.
	 */
	HashType hashValue() const {

		if (_hashDirty) {

			_hash = hash_value(*static_cast<const T*>(this));
			_hashDirty = false;
		}

		return _hash;
	}

	/**
	 * Mark the stored hash value as dirty, such that it gets recomputed with 
	 * the next call to hashValue().
	 */
	void setHashDirty() {

		_hashDirty = true;
	}

private:

	mutable HashType _hash;
	mutable bool     _hashDirty;
};

#endif // UTIL_HASHABLE_H__

