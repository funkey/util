
class TypeNone {

public:

	typedef TypeNone Head;
	typedef TypeNone Tail;
};

template <typename H, typename T = TypeNone >
class TypeList {

public:

	typedef H Head;
	typedef T Tail;
};

// Usage: TypeList<int, TypeList<float, ... > >
