#ifndef UTIL_LOCKABLE_H__
#define UTIL_LOCKABLE_H__

#include <mutex>

// forward declaration
class LockGuard;

/**
 * Base class for lockable classes, to be used with LockGuard:
 *
 *   LockGuard guard(a);
 *
 * ensures that Lockable object a is accessed by only one thread in the current 
 * context. The same thread can aquire a lock to one object more than once.
 */
class Lockable {

private:

	std::recursive_mutex& getMutex() const { return _mutex; }

	mutable std::recursive_mutex _mutex;

	friend class LockGuard;
};

class LockGuard {

public:

	LockGuard(const Lockable& lockable) : _guard(lockable.getMutex()) {}

private:

	std::lock_guard<std::recursive_mutex> _guard;
};

#endif // UTIL_LOCKABLE_H__

