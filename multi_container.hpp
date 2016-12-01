#ifndef UTIL_MULTI_CONTAINER_H__
#define UTIL_MULTI_CONTAINER_H__

#include <vector>
#include <algorithm>

namespace util {

namespace detail {

template <typename Head, typename ... Tail>
class MultiContainerImpl : protected std::vector<Head>, public MultiContainerImpl<Tail...> {

protected:

	typedef MultiContainerImpl<Tail...> ParentType;

	std::size_t size() const { return std::vector<Head>::size() + ParentType::size(); }

	template<typename F>
	void for_each(F f) { std::for_each(std::vector<Head>::begin(), std::vector<Head>::end(), f); ParentType::for_each(f); }
};
template <typename Last>
class MultiContainerImpl<Last> : protected std::vector<Last> {

protected:

	std::size_t size() const { return std::vector<Last>::size(); }

	template<typename F>
	void for_each(F f) { std::for_each(std::vector<Last>::begin(), std::vector<Last>::end(), f); }
};

} // namespace detail

/**
 * A container holding elements of different types in different collections. The 
 * types are given in a typelist as a template argument.
 */
template <typename ... Types>
class multi_container : public detail::MultiContainerImpl<Types...> {

public:

	typedef detail::MultiContainerImpl<Types...> ParentType;

	/**
	 * Add an element of type T to its corresponding collection.
	 */
	template <typename T>
	void add(const T& t) { std::vector<T>::push_back(t); }

	/**
	 * Get an element of type T by its index. Indices range from 0 until 
	 * (excluding) size<T>().
	 */
	template <typename T>
	T& get(std::size_t i) { return std::vector<T>::operator[](i); }
	template <typename T>
	const T& get(std::size_t i) const { return std::vector<T>::operator[](i); }

	/**
	 * Get the collection of all elements of type T.
	 */
	template <typename T>
	std::vector<T>& get() { return static_cast<std::vector<T>&>(*this); }
	template <typename T>
	const std::vector<T>& get() const { return static_cast<const std::vector<T>&>(*this); }

	/**
	 * Get the number of elements of type T in this container.
	 */
	template <typename T>
	std::size_t size() const { return std::vector<T>::size(); }

	/**
	 * Get the total number of elements in this container.
	 */
	std::size_t size() const { return ParentType::size(); }

	/**
	 * Delete all entries of type T.
	 */
	template <typename T>
	void clear() { std::vector<T>::clear(); }

	/**
	 * Pass each element of each type to the given functor.
	 */
	template<typename F>
	void for_each(F f) {  ParentType::for_each(f); }
};

} // namespace util

#endif // UTIL_MULTI_CONTAINER_H__
