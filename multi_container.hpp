#include <vector>
#include <algorithm>
#include <boost/static_assert.hpp>

#include "typelist.h"

namespace detail {

/**
 * multi_container base class for the type Type (=Head). A multi_container will 
 * be composed of several such class templates, one for each type it should 
 * hold.
 */
template <typename Head, typename Tail>
class MultiContainerImpl : public MultiContainerImpl<typename Tail::Head, typename Tail::Tail> {

public:

	typedef Head                                                           Type;
	typedef MultiContainerImpl<typename Tail::Head, typename Tail::Tail>   Parent;
	typedef typename Parent::Type                                          ParentType;

protected:

	/**
	 * Add an element of type Type.
	 */
	void add(const Type& t) { _elems.push_back(t); }

	/**
	 * Call functor with every element in our container and ask our parent to do 
	 * the same afterwards.
	 */
	template <typename F>
	void for_each(F functor) { std::for_each(_elems.begin(), _elems.end(), functor); Parent::for_each(functor); }

	/**
	 * The elements of type Type in this container.
	 */
	std::vector<Type> _elems;
};

// template specialization for no-tail (uppermost) case
template <>
class MultiContainerImpl<TypeNone, TypeNone> {

public:

	typedef TypeNone Type;
	typedef TypeNone Parent;
	typedef TypeNone ParentType;

protected:

	void add(const TypeNone&) {}

	template <typename F>
	void for_each(F) {}
};

/**
 * Metafunction to get the type of the MultiContainerImpl that holds elements of 
 * the type Type, as created by a multi_container with the typelist specified by 
 * TypesHead and TypesTail.
 */
template <typename Type, typename TypesHead, typename TypesTail>
struct GetMultiContainerImplType {

	// default: delegate to next
	typedef typename GetMultiContainerImplType<Type, typename TypesTail::Head, typename TypesTail::Tail>::Value Value;
};
template <typename Type, typename TypesTail>
struct GetMultiContainerImplType<Type, Type, TypesTail> {

	// abort: Type and TypesHead match
	typedef MultiContainerImpl<Type, TypesTail> Value;
};
template <typename Type>
struct GetMultiContainerImplType<Type, TypeNone, TypeNone> {

	// ERROR: you asked for a type that is not strored in this container
	BOOST_STATIC_ASSERT_MSG(sizeof(Type) == 0, "The requested type is not part of this multi-container.");
};

} // namespace detail

/**
 * A container holding elements of different types in different collections. The 
 * types are given in a typelist as a template argument.
 */
template <typename Types>
class multi_container :
		public detail::MultiContainerImpl<
				typename Types::Head,
				typename Types::Tail> {

public:

	typedef detail::MultiContainerImpl<typename Types::Head, typename Types::Tail> ImplType;

	/**
	 * Add an element of type T to its corresponding collection.
	 */
	template <typename T>
	void add(const T& t) {

		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		TContainerType::add(t);
	}

	/**
	 * Get an element of type T by its index. Indices range from 0 until 
	 * (excluding) size<T>().
	 */
	template <typename T>
	T& get(unsigned int i) {
	
		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		return TContainerType::_elems[i];
	}
	template <typename T>
	const T& get(unsigned int i) const {
	
		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		return TContainerType::_elems[i];
	}

	/**
	 * Get the collection of all elements of type T.
	 */
	template <typename T>
	std::vector<T>& get() {
	
		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		return TContainerType::_elems;
	}
	template <typename T>
	const std::vector<T>& get() const {
	
		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		return TContainerType::_elems;
	}

	/**
	 * Get the number of elements of type T in this container.
	 */
	template <typename T>
	unsigned int size() const { return get<T>().size(); }

	/**
	 * Delete all entries of type T.
	 */
	template <typename T>
	void clear() {

		typedef typename detail::GetMultiContainerImplType<T, typename Types::Head, typename Types::Tail>::Value TContainerType;
		TContainerType::_elems.clear();
	}

	/**
	 * Pass each element of each type to the given functor.
	 */
	template<typename F>
	void for_each(F f) { ImplType::for_each(f); }
};
