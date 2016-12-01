#ifndef UTIL_TREE_H__
#define UTIL_TREE_H__

/**
 * Macro to include in the public part of a tree element that is supposed to be 
 * visitible by a static tree visitor, e.g.,:
 *
 *   class ConcreteElement {
 *
 *   public:
 *
 *     UTIL_TREE_VISITABLE();
 *   };
 *
 * Ensures that the correct visitor method for the type of the element is 
 * called.
 */
#define UTIL_TREE_VISITABLE() template <typename VisitorType> void accept(VisitorType& visitor) { visitor.enter(*this); visitor.visit(*this); visitor.traverse(*this, visitor); visitor.leave(*this); }

/**
 * Base class for all tree nodes.
 */
class TreeNode {};

/**
 * Base-class for static tree visitors. Implements no-op callbacks for enter(), 
 * visit(), and leave(). Implements basic support for traverse() for 
 * multi_conainters.
 *
 * Concrete visitor implementations are supposed to override these methods. The 
 * sequence of calls for these method are for each element e and the concrete 
 * visitor v:
 *
 *   enter(e);
 *   visit(e);
 *   traverse(e, v);
 *   leave(e);
 *
 * The point of having an enter() and a visit() method is to perform more 
 * general operations on enter() (which can be caught by a base-class reference) 
 * and perform more specialized operations on visit().
 */
class TreeVisitor {

public:

	/**
	 * Will be called whenever the visitor enters an element of type T.
	 *
	 * Default implementation does nothing.
	 */
	template <typename T>
	void enter(T&) {}

	/**
	 * Will be called after the visitor enters an element of type T.
	 *
	 * Default implementation does nothing.
	 */
	template <typename T>
	void visit(T&) {}

	/**
	 * Will be called after enter() to traverse deeper in the tree. Type 
	 * information about the concrete visitor is available as the VisitorType 
	 * template argument.
	 *
	 * Default implementation does nothing, i.e., treats the element as a leaf.
	 */
	template <typename VisitorType>
	void traverse(TreeNode&, VisitorType&) {}

	/**
	 * Will be called whenever the visitor leaves an element of type T.
	 *
	 * Default implementation does nothing.
	 */
	template <typename T>
	void leave(T&) {}
};

#endif // UTIL_TREE_H__

