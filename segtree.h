#ifndef H_SEGTREE
#define H_SEGTREE

#include <stdexcept>

/**
 * @brief Default tag for adding some value to a range
 * 
 * Every tag type must overload () in order to get a tag.
 */
template <class T>
struct addTag {
	// Record sum
	T sum;
	// Constructor
	addTag(): sum() {}
	addTag(const T &val): sum(val) {}
	// Update the tag
	template <class U>
	void operator () (const addTag<T> &tag, U &data) {
		data.val += tag.sum * (data.getR()-data.getL()+1);
		sum += tag.sum;
	}
	// clear the tag
	void operator () () {
		sum = T();
	}
};

/**
 * @brief No range modification tag
 * 
 */
template <class T>
struct noPushDownTag {
	// Record sum
	T sum;
	// Constructor
	noPushDownTag(): sum() {}
	noPushDownTag(const T &val): sum(val) {}
	// Update the tag
	template <class U>
	void operator () (const noPushDownTag<T> &tag, U &data) {
		data.val += tag.sum;
	}
	// clear the tag
	void operator () () {}
};

/**
 * @brief Default update function for summing up values
 * 
 * Every update function must overload () to merge two
 * values.
 */
struct addUpdate {
	template <class T>
	T operator () (const T &lhs, const T &rhs) const {
		return lhs + rhs;
	}
};

/**
 * @brief Default pushdown function for tag distribution
 * 
 * Every pushdown funtion must overload () to pushdown a tag.
 */
struct addPushdown {
	template <class T>
	void operator () (T &par, T &lhs, T &rhs) const {
		lhs.tag(par.tag, lhs);
		rhs.tag(par.tag, rhs);
		par.tag();
	}
};

/**
 * @brief Empty pushdown function for no range queries
 * 
 */
struct noPushDown {
	void operator () (...) const {}
};

/**
 * @brief Segment tree definition
 * 
 * @author changyuz@usc.edu
 * @tparam dType The type of data to store in the segment tree
 * @tparam tType The type of tag for range queries
 * @tparam upd The update function
 * @tparam pushd The pushdown function for tags
 */
template <class dType = int, class tType = addTag<dType>, class upd = addUpdate,
	class pushd = addPushdown>
class segmentTree {
private:
	// Update and pushdown function
	upd update;
	pushd pushdown;
	// Node in segment tree
	class Node {
		friend class segmentTree;
	private:
		int L, R;
		Node *l, *r;
	public:
		dType val;
		tType tag;
		// Get value of L
		int getL() {
			return L;
		}
		// Get value of R
		int getR() {
			return R;
		}
		// Constructor
		Node(int _L, int _R):L(_L), R(_R), val(), tag() {
			l = r = NULL;
		}
	};
	// Root of the segment tree
	Node* root;

	// helper function of Destructor
	void clear(Node* &cur) {
		if (cur == NULL)
			return;
		clear(cur->l);
		clear(cur->r);
		delete cur;
		cur = NULL;
	}

	// check if a node exists; if not, create it
	void check(Node* &cur, int l, int r) {
		if (cur == NULL) {
			cur = new Node(l, r);
		}
	}

	// helper function of replace
	void replace(Node* cur, int pos, const dType &val) {
		if (cur->L == cur->R) {
			cur->val = val;
			return;
		}
		int mid((cur->L+cur->R) >> 1);
		check(cur->l, cur->L, mid);
		check(cur->r, mid+1, cur->R);
		pushdown(*cur, *cur->l, *cur->r);
		if (pos <= mid) {
			replace(cur->l, pos, val);
		} else {
			replace(cur->r, pos, val);
		}
		cur->val = update(cur->l->val, cur->r->val);
	}

	// helper function of range modification
	void modify(Node* cur, int l, int r, const tType &tag) {
		if (cur->L == l && cur->R == r) {
			cur->tag(tag, *cur);
			return;
		}
		int mid((cur->L+cur->R) >> 1);
		check(cur->l, cur->L, mid);
		check(cur->r, mid+1, cur->R);
		pushdown(*cur, *cur->l, *cur->r);
		if (r <= mid) {
			modify(cur->l, l, r, tag);
		} else if (l > mid) {
			modify(cur->r, l, r, tag);
		} else {
			modify(cur->l, l, mid, tag);
			modify(cur->r, mid+1, r, tag);
		}
		cur->val = update(cur->l->val, cur->r->val);
	}

	// helper function for queries
	dType query(Node* cur, int l, int r) {
		if (cur->L == l && cur->R == r) {
			return cur->val;
		}
		int mid((cur->L+cur->R) >> 1);
		check(cur->l, cur->L, mid);
		check(cur->r, mid+1, cur->R);
		pushdown(*cur, *cur->l, *cur->r);
		if (r <= mid)
			return query(cur->l, l, r);
		else if (l > mid)
			return query(cur->r, l, r);
		else
			return update(query(cur->l, l, mid), query(cur->r, mid+1, r));
	}

public:
	/**
	 * @brief Constructor of segment tree
	 * 
	 * Range is [l, r]
	 */
	segmentTree(int l, int r) {
		if (l > r)
			throw std::range_error("Range must contain at least 1 element");
		root = new Node(l, r);
	}

	/**
	 * @brief Destructor of segment tree
	 */
	~segmentTree() {
		clear(root);
	}

	/**
	 * @brief Clear all elements in the segment tree
	 * 
	 * The range of the tree remains unchanged.
	 */
	void clear() {
		clear(root->l);
		clear(root->r);
	}

	/**
	 * @brief Replace a single spot
	 * 
	 * @param pos Index to replace
	 * @param val Data to put in that spot
	 */
	void replace(int pos, const dType &val) {
		if (pos < root->L || pos > root->R)
			throw std::out_of_range("Index out of range");
		replace(root, pos, val);
	}

	/**
	 * @brief Modify a range with tag
	 * 
	 * @param l start pos of modification
	 * @param r end pos of modification
	 * @param tag tag to add to the range
	 */
	void modify(int l, int r, const tType &tag) {
		if (l == r+1) // Empty range
			return;
		if (l > r)
			throw std::range_error("Invalid modification argument");
		if (l < root->L || r > root->R)
			throw std::out_of_range("Modification index out of range");
		modify(root, l, r, tag);
	}

	/**
	 * @brief Modify a single element with tag
	 * @param pos index of modification
	 * @param tag tag to add to that element
	 */
	void modify(int pos, const tType &tag) {
		modify(pos, pos, tag);
	}

	/**
	 * @brief Query (merge a range of values)
	 * 
	 * @param l start pos of query
	 * @param r end pos of query
	 */
	dType query(int l, int r) {
		if (l > r)
			throw std::range_error("Invalid query argument");
		if (l < root->L || r > root->R)
			throw std::out_of_range("Query index out of range");
		return query(root, l, r);
	}
	// single element query
	dType query(int pos) {
		return query(pos, pos);
	}
};
#endif