/**
 * Very simple unbalanced binary tree implementation.
 *
 * Sometimes faster than std::set and sometimes not. Why? :)
 */

#pragma once

/**
 * In small projects like this encapsulating things in a separate namespace is
 * not really necessary but a good practice in general.
 *
 * For example if some other file is included that also contains a node class,
 * these can be differentiated as pfp::node and someting_else::node.
 *
 * If you dislike explicitly stating the namespace see "c++ using namespace" on
 * you facourite search engine. I genrally don't use namespaces since I like the
 * clarity of "pfp::node" compared to a using somewhere and then just saying
 * "node" locally.
 */
namespace pfp {

/**
 * The node class is just meant to be the internal node of the binary tree.
 *
 * Here the template parameter "dtype" allows the tree to be (re)used with any
 * types that support comparison with ">" and "==". The compiler will check what
 * types the node class is used with and compare separate implementations for
 * them.
 *
 * For this project the only type used is "int" so the templating is redundant
 * but good practice for code reusability.
 */
template <class dtype>
class node {
   private:
    dtype val;
    node* left = nullptr;
    node* right = nullptr;

   public:
    /**
     * Constructor for node objects. Logically equivalent to
     *
     * node (dtype value) {
     *    val = value;
     * }
     *
     * this type is often easier for the compiler to optimize leading to faster
     * code.
     *
     * For more info see the Effective c++ book (probably mentioned on the
     * course).
     */
    node(dtype value) : val(value) {}

    /**
     * Destructor for node objects.
     *
     * Not strictly needed since program termination will deallocate the memory,
     * but if the data structure didn't live for the duration of execution, not
     * explicitly deleting any children would be a massive memory leak.
     *
     * The default destructor would simply deallocate the node itself and leave
     * any children orphaned in memory.
     */
    ~node() {
        if (left != nullptr) delete (left);
        if (right != nullptr) delete (right);
    }

    /**
     * Inserts "value" to the subtree rooted at this node.
     *
     * If the value already exists nothing is done (since this is logically a
     * set).
     */
    void insert(dtype value) {
        // For any given node in a tree, it is expected to be unlikely that
        // the value to be inserted is the same as the value of the node.
        // The [[unlikely]] annotation tells the compiler to optimize
        // accordingly. This can have a surpisingly big effect on performance
        // due to the aggressinve pipelining in modern michroprosessors. 
        // 
        // if (val == value) {
        //    [[unlikely]] return;
        // }
        //
        // Unfortunatelu CSES does not compile wit [[(un)likely]] annotations present. 
        // The same could be accomplished by flipping the condition:
        //
        // if (val != value) {
        //    <rest of function body>
        // } else {
        //    return;
        // }
        //
        // However the [[(un)likely]] annotation often leads to more readable
        // code. (And better conveys the intention of the programmer.)
        //
        // Feel free to play around with annotations locally if you feel like it.
        if (val == value) {
            return;
        }
        if (value > val) {
            if (right == nullptr) {
                right = new node(value);
            } else {
                right->insert(value);
            }
        } else {
            if (left == nullptr) {
                left = new node(value);
            } else {
                left->insert(value);
            }
        }
    }

    bool query(dtype value) const {
        if (value == val) {
            return true;
        }
        // Ternary expressions are often very easy for the compiler to optimize,
        // especially in tail recursion situations like this.
        // The first (affirmative) branch should be the more likely one.
        if (value > val) {
            return right != nullptr ? right->query(value) : false;
        }
        return left != nullptr ? left->query(value) : false;
    }
};

/**
 * The binary_ree class is the actual class meant to be used as a binary tree
 * structure.
 *
 * Again, the "dtype" allows the tree to be (re)used with any types that support
 * comparison.
 */
template <class dtype>
class binary_tree {
   private:
    typedef node<dtype> root_type;
    root_type* root = nullptr;

   public:
    /**
     * No constructor is explicitly defined here since the default constructor
     * generated by the compiler assigns sufficiently good values and nothing
     * more is needed.
     *
     * The destructor is again required to guard agains memory leaks.
     */
    ~binary_tree() {
        if (root != nullptr) delete (root);
    }

    void insert(dtype value) {
        if (root == nullptr) {
            root = new node(value);
        } else {
            root->insert(value);
        }
    }

    /**
     * In c++17, the set structures do not support simple membership queries and
     * count needs to be used instead. Due to this, since the new data
     * structures need to work with the same interface, the membership query
     * here will be called count aswell.
     */
    int count(dtype value) const {
        return root != nullptr ? root->query(value) : false;
    }
};

}  // namespace pfp