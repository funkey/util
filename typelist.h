#ifndef UTIL_TYPELIST_H__
#define UTIL_TYPELIST_H__

class TypeNone {

public:

	typedef TypeNone Head;
	typedef TypeNone Tail;
};

/**
 * A typelist to give several types as a single template argument.
 * 
 * Usage: TypeList<int, TypeList<float, ... > >
 */
template <typename H, typename T = TypeNone >
class TypeList {

public:

	typedef H Head;
	typedef T Tail;
};

// convenience macros for the easy creation of typelists
#define TYPELIST_1(x1) TypeList<x1>
#define TYPELIST_2(x1, x2) TypeList<x1, TypeList<x2> >
#define TYPELIST_3(x1, x2, x3) TypeList<x1, TypeList<x2, TypeList<x3> > >
#define TYPELIST_4(x1, x2, x3, x4) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4> > > >
#define TYPELIST_5(x1, x2, x3, x4, x5) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5> > > > >
#define TYPELIST_6(x1, x2, x3, x4, x5, x6) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5, TypeList<x6> > > > > >
#define TYPELIST_7(x1, x2, x3, x4, x5, x6, x7) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5, TypeList<x6, TypeList<x7> > > > > > >
#define TYPELIST_8(x1, x2, x3, x4, x5, x6, x7, x8) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5, TypeList<x6, TypeList<x7, TypeList<x8> > > > > > > >
#define TYPELIST_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5, TypeList<x6, TypeList<x7, TypeList<x8, TypeList<x9> > > > > > > > >
#define TYPELIST_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) TypeList<x1, TypeList<x2, TypeList<x3, TypeList<x4, TypeList<x5, TypeList<x6, TypeList<x7, TypeList<x8, TypeList<x9, TypeList<x10> > > > > > > > > >

#endif // UTIL_TYPELIST_H__

