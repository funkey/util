#ifndef UTIL_VERSION_H__
#define UTIL_VERSION_H__

#include <cassert>

/**
 * A version tag to check for changes in multi-threaded environments.
 *
 * Usage example to prevent dirty reads on a shared variable 'foo':
 *
 *   Thread A:
 *
 *     while (true) {
 *       tag.lock();
 *       update(foo);
 *       tag.unlock();
 *     }
 *
 *   Thread B:
 *
 *     while (true) {
 *
 *       version_tag::version_type v = tag.get_version()
 *
 *       if (version_tag::is_locked(v))
 *         continue;
 *
 *       bar = foo;
 *
 *       if (tag.changed(v))
 *         continue;
 *
 *       // now we know that foo was not changed while we were reading it
 *       process(bar);
 *     }
 */
class version_tag {

public:

	typedef unsigned char version_type;

	version_tag() : _version(0) {}

	/**
	 * Get the current version of the tag.
	 */
	version_type get_version() { return _version; }

	/**
	 * Mark the version tag as locked.
	 */
	void lock() { assert(_version%2 == 0); _version++; }

	/**
	 * Unlock the tag and increment the version.
	 */
	void unlock() { assert(_version%2 == 1); _version++; }

	/**
	 * Check whether the given version is locked.
	 */
	static bool is_locked(version_type v) { return v%2; }

	/**
	 * Check whether the tag's version changed compared to the given version.
	 */
	bool changed(version_type v) { return v != _version; }

private:

	unsigned char _version;
};

#endif // UTIL_VERSION_H__

