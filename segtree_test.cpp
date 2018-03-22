#include "segtree.h"
#include <ctime>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <string>
using namespace std;

#if RAND_MAX <= SHRT_MAX
#define rand() (rand() * (RAND_MAX+1) + rand())
#endif

void bruteForceTest(int N) {
	int L = rand() % 1000 - 500;
	int R = rand() % 1000;
	if (L > R)
		swap(L, R);
	// Create a segment tree
	segmentTree<> T(L, R);
	int *A = new int[R-L+1]();
	for (int _=0; _<N; _++) {
		int l = rand() % (R-L+1) + L;
		int len = rand() % (R-l+1);
		int r = l + len;
		int typ = rand() & 3;
		int val = rand() % 1000 - 500;
		switch (typ) {
			case 0: // Single element replacement
				A[l-L] = val;
				T.replace(l, val);
				break;
			case 1: // Range modification
				for (int i=0; i<=len; i++)
					A[l-L+i] += val;
				T.modify(l, r, val);
				break;
			case 2: {// Range query
				int sum = 0;
				for (int i=0; i<=len; i++)
					sum += A[l-L+i];
				int ret = T.query(l, r);
				if (ret != sum) {
					cerr << "On querying " << l << "-" << r << endl;
					cerr << " Expect " << sum << ", Get " << ret << endl;
					throw runtime_error("Test failed"); 
				}
				break;
			}
			case 3: // Single element modification
				A[l-L] += val;
				T.modify(l, val);
				break;
			default:
				break;
		}
	}
	cout << "BRUTE FORCE TEST PASSED." << endl;
	delete [] A;
}

struct myData{
	int maxl, maxr, maxval, sum;
	myData(int val = 0): maxl(val), maxr(val), maxval(val), sum(val) {}
	myData operator + (const myData &rhs) const {
		myData ret;
		ret.maxl = max(maxl, sum + rhs.maxl);
		ret.maxr = max(rhs.maxr, maxr + rhs.sum);
		ret.sum = sum + rhs.sum;
		ret.maxval = max(maxval, rhs.maxval);
		ret.maxval = max(ret.maxval, maxr + rhs.maxl);
		return ret;
	}
	myData &operator += (int val) {
		maxl += val;
		maxr += val;
		maxval += val;
		sum += val;
		return *this;
	}
};

struct myData2 {
	int maxl, maxr, maxval, sum;
	myData2(int val = 0): maxl(val), maxr(val), maxval(val), sum(val) {}
};

struct myTag {
	int val;
	myTag(int _val = 0): val(_val) {}

	template <class U>
	void operator () (const myTag &tag, U &data) {
		data.val.maxl += tag.val;
		data.val.maxr += tag.val;
		data.val.maxval += tag.val;
		data.val.sum += tag.val;
	}
	// clear the tag
	void operator () () {}
};

struct myUpdate {
	myData2 operator () (const myData2 &lhs, const myData2 &rhs) const {
		myData2 ret;
		ret.maxl = max(lhs.maxl, lhs.sum + rhs.maxl);
		ret.maxr = max(rhs.maxr, lhs.maxr + rhs.sum);
		ret.sum = lhs.sum + rhs.sum;
		ret.maxval = max(lhs.maxval, rhs.maxval);
		ret.maxval = max(ret.maxval, lhs.maxr + rhs.maxl);
		return ret;
	}
};

// Different kinds of usage
void functionalityTest() {
	segmentTree<> T1(0, 10);
	T1.modify(1, 9, 1);
	cout << T1.query(3, 10) << endl; // expect 7
	T1.replace(6, 10);
	cout << T1.query(5, 10) << endl; // expect 14
	T1.modify(0, -1);
	cout << T1.query(1) << endl; // expect 1
	cout << T1.query(0, 1) << endl; // expect 0
	T1.clear();
	T1.modify(2, 5, -1);
	cout << T1.query(0, 5) << endl; // expect -4

	// Test template parameter
	segmentTree<double> T2(-5, 5);
	T2.modify(1, 5, 0.2);
	cout << T2.query(-4, 2) << endl; // expect 0.4
	T2.replace(5, 2.3);
	cout << T2.query(4, 5) << endl; // expect 2.5
	T2.modify(-3, -0.5);
	cout << T2.query(0) << endl; // expect 0
	cout << T2.query(-3, 4) << endl; // expect 0.3

	/**
	 * T3 maintains max interval sum that contains at least 1 element
	 * 
	 * It simply overloads + operator; It can also provide another update function,
	 * as T4. 
	 */
	segmentTree<myData, noPushDownTag<int>, addUpdate, noPushDown> T3(1, 10);
	segmentTree<myData2, myTag, myUpdate, noPushDown> T4(1, 10);
	T3.modify(1, 4);
	T3.modify(2, -5);
	T3.modify(3, 3);
	T3.modify(4, -1);
	T3.modify(5, 3);
	T3.modify(6, -1);
	cout << T3.query(1, 10).maxval << endl; // expect 5
	cout << T3.query(1, 3).maxval << endl; // expect 4
	T3.replace(2, -1);
	cout << T3.query(1, 10).maxval << endl; // expect 8

	//same for T4
	T4.modify(1, 4);
	T4.replace(2, -5);
	T4.modify(3, 3);
	T4.modify(4, -1);
	T4.modify(5, 3);
	T4.replace(6, -1);
	cout << T4.query(1, 10).maxval << endl; // expect 5
	cout << T4.query(1, 3).maxval << endl; // expect 4
	T4.replace(2, -1);
	cout << T4.query(1, 10).maxval << endl; // expect 8

	T3.clear(); // clear and do again
	T3.replace(1, 4);
	T3.replace(2, -5);
	T3.replace(3, 3);
	T3.replace(4, -1);
	T3.replace(5, 3);
	T3.replace(6, -1);
	// Test copy
	auto T5(T3);
	cout << T5.query(1, 10).maxval << endl; // expect 5
	cout << T5.query(1, 3).maxval << endl; // expect 4
	T5.modify(2, 4);
	cout << T5.query(1, 10).maxval << endl; // expect 8
	cout << T3.query(1, 10).maxval << endl; // expect 5
	T5 = T3;
	cout << T5.query(1, 10).maxval << endl; // expect 5
}

// Large dataset
void pressureTest(int N) {
	int L = -100000000, R = 100000000;
	unsigned int beg = clock();
	segmentTree<> T(L, R);
	for (int i = 0; i < N; i++) {
		int l = rand() % (R-L+1) + L;
		int len = rand() % (R-l+1);
		int r = l + len;
		int typ = rand() & 3;
		int val = rand() % 1000 - 500;
		switch (typ) {
			case 0: // Single element replacement
				T.replace(l, val);
				break;
			case 1: // Range modification
				T.modify(l, r, val);
				break;
			case 2: {// Range query
				int ret = T.query(l, r);
				break;
			}
			case 3: // Single element modification
				T.modify(l, val);
				break;
			default:
				break;
		}
	}
	unsigned int ed = clock();
	cout << N << " operations, total time: " << (double)(ed-beg) / CLOCKS_PER_SEC << endl;
}

// v.s. DP on maximum interval sum
void DPtest(int N) {
	int L = rand() % 1000 - 500;
	int R = rand() % 1000;
	if (L > R)
		swap(L, R);
	// Create a segment tree
	segmentTree<myData2, myTag, myUpdate, noPushDown> T(L, R);
	int *A = new int[R-L+1]();
	for (int _=0; _<N; _++) {
		int l = rand() % (R-L+1) + L;
		int len = rand() % (R-l+1);
		int r = l + len;
		int typ = rand() % 3;
		int val = rand() % 1000 - 450;
		switch (typ) {
			case 0: // Single element replacement
				A[l-L] = val;
				T.replace(l, val);
				break;
			case 1: {// Range query
				int ans = -0x3f3f3f3f, cur = -0x3f3f3f3f;
				for (int i=0; i<=len; i++) {
					int e = A[l-L+i];
					cur = max(e, cur+e);
					ans = max(ans, cur);
				}
				int ret = T.query(l, r).maxval;
				if (ret != ans) {
					cerr << "On querying " << l << "-" << r << endl;
					cerr << " Expect " << ans << ", Get " << ret << endl;
					throw runtime_error("Test failed"); 
				}
				break;
			}
			case 2: // Single element modification
				A[l-L] += val;
				T.modify(l, val);
				break;
			default:
				break;
		}
	}
	cout << "DP TEST PASSED." << endl;
	delete [] A;
}

// Test exceptions
void failureTest(){
	try {
		segmentTree<> T(1, 1);
		T.modify(1, 2, 1);
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	try {
		segmentTree<> T(1, 0);
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	try {
		segmentTree<double> T(1, 10);
		double ret = T.query(2, 8);
		T.modify(8, 7, 0);
		ret = T.query(0);
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	try {
		segmentTree<string, noPushDownTag<string>, addUpdate, noPushDown> T(1, 10);
		string ret = T.query(2, 8);
		T.modify(8, 7, string("Hi"));
		T.replace(0, string("what"));
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	try {
		segmentTree<int> T(-1, 1);
		int ret = T.query(0, -1);
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	try {
		segmentTree<int> T(-1, 1);
		T.modify(2, -1);
	} catch (const exception &e) {
		cout << e.what() << endl;
	}
	cout << "EXCEPTION TEST DONE" << endl;
}

int main() {
	srand(time(0));
	try {
		functionalityTest();
		failureTest();
		for (int i=1; i<=10; i++) {
			cout << "#" << i << " ";
			bruteForceTest(100000);
			DPtest(100000);
		}
		pressureTest(1000000);
	} catch (const exception &e) {
		cerr << "ERROR:" << e.what() << endl;
	}
	return 0;
}