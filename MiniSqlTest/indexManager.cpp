#include "Macro.h"
#include "parser.h"
#include "IndexRecord.h"
#include "BufferManager.h"
#include "TableRecord.h"
#include "Tableforlyf.h"
#include "indexManager.h"

template<class Ktype>
int BtreeNode<Ktype>::strtoi(const string& s) {
	int k=0;
	for (int i = sizeof(int)/sizeof(char) -1; i>=0; i--)
		k = (k << 8*sizeof(char)) + ((s[i]<0)?s[i]+256:s[i]);
	return k;
}
template<class Ktype>
string BtreeNode<Ktype>::itostr(const int& k) {
	char * c = (char*)&k;
	string s = "";
	for (int i=0; i<sizeof(int)/sizeof(char); i++)
		s += c[i];
	return s;
}
template<class Ktype>
Ktype BtreeNode<Ktype>::strtok(const string& s) {
	Ktype * k = (Ktype *)&s;
	return *k;
}
template<class Ktype>
string BtreeNode<Ktype>::ktostr(const Ktype& k) {
	char * c = (char*)&k;
	string s = "";
	for (int i=0; i<sizeof(Ktype)/sizeof(char); i++)
		s += c[i];
	return s;
}
template<class Ktype>
BtreeNode<Ktype>::BtreeNode(const string& indexname, const Location& loc) {
	Location b;
	string res = BufferManager::readBlockOfIndex(indexname, loc.fileBlockNum);
	map<string, int> a = TableRecord::Table_Columns_Type(IndexRecord::index_on_table[indexname]);
	int kk = a[IndexRecord::index_on_column[indexname]];
	size = strtoi(res.substr(0, sizeof(int)));
	res = res.erase(0, sizeof(int));
	type = strtoi(res.substr(0, sizeof(int)));
	res = res.erase(0, sizeof(int));
	if (kk < 2) {
		for (int i=0; i<size-1; i++) {
			k.push_back(strtok(res.substr(0, sizeof(Ktype))));
			res = res.erase(0, sizeof(Ktype));
		}
	} else {
		for (int i=0; i<size-1; i++) {
			string tmpp = res.substr(0, kk >> 2);
			k.push_back(strtok(tmpp));
			res = res.erase(0, kk >> 2);
		}
	}
	for (int i=0; i<size; i++) {
		b.fileBlockNum = strtoi(res.substr(0, sizeof(int)));
		res = res.erase(0, sizeof(int));
		b.offset = strtoi(res.substr(0, sizeof(int)));
		res = res.erase(0, sizeof(int));
		p.push_back(b);
	}
}
template<class Ktype>
string BtreeNode<Ktype>::toString(const string& indexname){
	string res = "";
	map<string, int> a = TableRecord::Table_Columns_Type(IndexRecord::index_on_table[indexname]);
	int kk = a[IndexRecord::index_on_column[indexname]];
	res += itostr(size);
	res += itostr(type);
	if (kk<2) {
		for (int i=0; i<size-1; i++)
			res += ktostr(k[i]);
	} else {
		for (int i=0; i<size-1; i++)
			res += k[i];
	}
	for (int i=0; i<size; i++)
		res += itostr(p[i].fileBlockNum) + itostr(p[i].offset);
	res.append(VOLUMN-res.size(), ' ');
	return res;
}
template<class Ktype>
unsigned BtreeNode<Ktype>::findFirstK(const Ktype& x) {
	unsigned i;
	for (i = 0; i < k.size(); i++)
	if (k[i] > x) 
		break;
	return i;
}
template<class Ktype>
Location BtreeNode<Ktype>::findLoc(const Ktype& x) {
	return p[findFirstK(x)];
}
template<class Ktype>
int BtreeNode<Ktype>::findX(const Ktype& x) {
	for (unsigned i = 0; i < k.size(); i++)
	if (k[i]==x)
		return (int)i;
	return -1;
}
template<class Ktype>
Location BtreeNode<Ktype>::findKx(const Ktype& x) {
	for (unsigned i = 0; i < k.size(); i++) {
		if (k[i] == x)
			return p[i];
		if (k[i] > x)
			return Location(-1, 0);
	}
	return Location(-1, 0);
}
template<class Ktype>
int BtreeNode<Ktype>::insertKx(const Ktype& x, const Location& xp) {		//  return 1 if the min
	unsigned i = findFirstK(x);
	k.insert(k.begin()+i, x);
	p.insert(p.begin()+i, xp);
	size++;
	return i==0;
}
template<class Ktype>
void BtreeNode<Ktype>::dispose() {
	p.clear();
	k.clear();
}
template<class Ktype>
void BtreeNode<Ktype>::print() {
	for (unsigned i=0; i<k.size(); i++)
		cout << k[i] << ' ';
	cout << endl;
}

template <class Ktype>
Location Btree<Ktype>::_BsearchNode(const Ktype& x){
	BtreeNode<Ktype> t(indexname, root);
	Location t_loc = root;
	while ((t.type != LEAFNODE) && (t.type != ROOT_LEAFNODE)) {
		t_loc = t.findLoc(x);
		BtreeNode<Ktype> tm(indexname, t_loc);
		t = tm;
	}
	return t_loc;
}
template <class Ktype>
int Btree<Ktype>::pop(){
	if (freelist.size() == 1) {
		return freelist[0]++;
	} else {
		int k = *(freelist.end()-1);
		freelist.pop_back();
		return k;
	}
}
template <class Ktype>
void Btree<Ktype>::push(int k){
	freelist.push_back(k);
}
template <class Ktype>
int Btree<Ktype>::_BinsertNode(const Location& cur_loc, const Ktype& x, const Location& xp_loc, Ktype& kup, Location& lup_loc) {			//  return 0 if nothing, 1 if the min, 2 if split, 3 if min and split
	BtreeNode<Ktype> cur(indexname, cur_loc);        //  read out
	if ((cur.type != LEAFNODE)&&(cur.type != ROOT_LEAFNODE)) {	// has child
		unsigned i = cur.findFirstK(x);		// insert position
		int res = _BinsertNode(cur.p[i], x, xp_loc, kup, lup_loc);
		switch (res) {
		case 0:	return 0;
		case 1: if (i>0)         //  min resolved in this level
					cur.k[i-1] = x;
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return i==0;	 //  i==0 means to report up
		case 2: cur.k.insert(cur.k.begin()+i, kup);
				cur.p.insert(cur.p.begin()+i+1, lup_loc);
				cur.size++;
				if (cur.size == n+1) {		// need split
					BtreeNode<Ktype> np(NONLEAFNODE, n/2+1);
					for (int i = (n+1)/2; i<n; i++) {
						np.k.push_back(cur.k[i]);
						np.p.push_back(cur.p[i]);
					}
					np.p.push_back(cur.p[n]);
					kup = cur.k[(n-1)/2];
					cur.k.erase(cur.k.begin()+(n-1)/2, cur.k.end());
					cur.p.erase(cur.p.begin()+(n+1)/2, cur.p.end());
					cur.size = (n+1)/2;
					lup_loc.fileBlockNum = pop();
					lup_loc.offset = 0;
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					BufferManager::writeBlockOfIndex(indexname, lup_loc.fileBlockNum, np.toString(indexname));
					return 2;
				}
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return 0;
		case 3: if (i>0)
					cur.k[i-1] = x;
				cur.k.insert(cur.k.begin()+i, kup);
				cur.p.insert(cur.p.begin()+i+1, lup_loc);
				cur.size++;
				if (cur.size == n+1) {		// need split
					BtreeNode<Ktype> np(NONLEAFNODE, n/2+1);
					for (int ii = (n+1)/2; ii<n; ii++) {
						np.k.push_back(cur.k[ii]);
						np.p.push_back(cur.p[ii]);
					}
					np.p.push_back(cur.p[n]);
					kup = cur.k[(n-1)/2];
					cur.k.erase(cur.k.begin()+(n-1)/2, cur.k.end());
					cur.p.erase(cur.p.begin()+(n+1)/2, cur.p.end());
					cur.size = (n+1)/2;
					lup_loc.fileBlockNum = pop();
					lup_loc.offset = 0;
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					BufferManager::writeBlockOfIndex(indexname, lup_loc.fileBlockNum, np.toString(indexname));
					return 2+(i==0);
				}
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return i==0;
		default: return -1;
		}
	} else {	// leaf
		int flag = cur.insertKx(x, xp_loc);
		//  apply for space, indeed another fun
		if (cur.size == n+1) {			//  need to leaf-split
			BtreeNode<Ktype> np(LEAFNODE, n/2+1);
			for(int i = (n+1)/2; i<n; i++) {
				np.k.push_back(cur.k[i]);
				np.p.push_back(cur.p[i]);
			}
			np.p.push_back(cur.p[n]);
			cur.k.erase(cur.k.begin() + (n+1)/2, cur.k.end());
			cur.p.erase(cur.p.begin() + (n+1)/2, cur.p.end());
			lup_loc.fileBlockNum = pop();
			lup_loc.offset = 0;
			cur.p.push_back(lup_loc);
			cur.size = (n+1)/2+1;
			kup = np.k[0];
			BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
			BufferManager::writeBlockOfIndex(indexname, lup_loc.fileBlockNum, np.toString(indexname));
			return 2+flag;
		}
		BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
		return flag;
	}
}
template <class Ktype>
int Btree<Ktype>::_BdeleteNode(const Location& cur_loc, const Ktype& x, const Location& sib_loc, Ktype& min, Ktype& kcrit) {
	BtreeNode<Ktype> cur(indexname, cur_loc);        //  read out
	BtreeNode<Ktype> sib(indexname, sib_loc);
	if ((cur.type != LEAFNODE)&&(cur.type != ROOT_LEAFNODE)) {	// has child
		unsigned i = cur.findFirstK(x);		// delete position
		Location child = cur.p[i];
		Location childsib = (i==0)?cur.p[i+1]:cur.p[i-1];
		int res = _BdeleteNode(child, x, childsib, min, cur.k[(i==0)?0:i-1]);
		switch (res) {
		case 0: BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return 0;
		case 1: if (i>0)         //  min resolved in this level
					cur.k[i-1] = min;
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return i==0;	 //  i==0 means to report up
		case 2: cur.k.erase(cur.k.begin()+((i==0)?0:i-1));
				cur.p.erase(cur.p.begin()+((i==0)?1:i));
				cur.size--;
				if ((cur.type == ROOT)&&(cur.size < 2)) {		//  root has only one child
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					return -2;
				}
				if ((cur.type == NONLEAFNODE)&&(cur.size < (n+1)/2)) {			// too few children
					if (sib.size > (n+1)/2) {			//  can lend a child
						if (sib.k[0] < min) {		//  sib is left to cur
							cur.k.insert(cur.k.begin(), kcrit);   //  transfer tail
							kcrit = sib.k[sib.size-2];
							sib.k.pop_back();
							cur.p.insert(cur.p.begin(), sib.p[sib.size-1]);
							sib.p.pop_back();
							cur.size++;
							sib.size--;
						} else {						//  sib at right side
							cur.k.push_back(kcrit);
							kcrit = sib.k[0];
							cur.p.push_back(sib.p[0]);
							cur.size++;
							sib.k.erase(sib.k.begin());
							sib.p.erase(sib.p.begin());
							sib.size--;
						}
						BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
						BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
						return 0;
					} else {							//  combine
						if (sib.k[0] < min) {		//  sib is left to cur
							sib.size += cur.size;
							sib.k.push_back(kcrit);
							for (int i=0; i<cur.size-1; i++)
								sib.k.push_back(cur.k[i]);
							for (int i=0; i<cur.size; i++)
								sib.p.push_back(cur.p[i]);
							BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
							push(cur_loc.fileBlockNum);
						} else {						//  sib at right side
							cur.size += sib.size;
							cur.k.push_back(kcrit);
							for (int i=0; i<sib.size-1; i++)
								cur.k.push_back(sib.k[i]);
							for (int i=0; i<sib.size; i++)
								cur.p.push_back(sib.p[i]);
							BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
							push(sib_loc.fileBlockNum);
						}
						return 2;
					}
				} else {
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					return 0;
				}
		case 3: cur.k.erase(cur.k.begin());
				cur.p.erase(cur.p.begin()+1);
				cur.size--;
				if ((cur.type == ROOT)&&(cur.size < 2)) {		//  root has only one child
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					return -2;
				}
				if ((cur.type == NONLEAFNODE)&&(cur.size < (n+1)/2)) {			// too few children
					if (sib.size > (n+1)/2) {			//  can lend a child
						if (sib.k[0] < min) {		//  sib is left to cur
							cur.k.insert(cur.k.begin(), min);   //  transfer tail
							kcrit = sib.k[sib.size-2];
							sib.k.pop_back();
							cur.p.insert(cur.p.begin(), sib.p[sib.size-1]);
							sib.p.pop_back();
							cur.size++;
							sib.size--;
							BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
							BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
							return 0;
						} else {						//  sib at right side
							cur.k.push_back(kcrit);
							kcrit = sib.k[0];
							cur.p.push_back(sib.p[0]);
							cur.size++;
							sib.k.erase(sib.k.begin());
							sib.p.erase(sib.p.begin());
							sib.size--;
							BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
							BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
							return 1;
						}
					} else {							//  combine
						if (sib.k[0] < min) {		//  sib is left to cur
							sib.size += cur.size;
							sib.k.push_back(min);
							for (int i=0; i<cur.size-1; i++)
								sib.k.push_back(cur.k[i]);
							for (int i=0; i<cur.size; i++)
								sib.p.push_back(cur.p[i]);
							BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
							push(cur_loc.fileBlockNum);
							return 2;
						} else {						//  sib at right side
							cur.size += sib.size;
							cur.k.push_back(kcrit);
							for (int i=0; i<sib.size-1; i++)
								cur.k.push_back(sib.k[i]);
							for (int i=0; i<sib.size; i++)
								cur.p.push_back(sib.p[i]);
							BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
							push(sib_loc.fileBlockNum);
							return 3;
						}
					}
				} else {
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					return 1;
				}
		default: return res;
		}
	} else {			//  leaf
		int i = cur.findX(x);
		if (i==-1)		//  can't find x
			return -1;
		cur.k.erase(cur.k.begin() + i);
		cur.p.erase(cur.p.begin() + i);
		cur.size--;
		if (cur.size == 1) {	    //  delete to empty
			min = sib.k[0]; 		// initial value in case of one-size node
			if (cur.type == ROOT_LEAFNODE) {
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				return -3;
			}
			if (cur.p[0] == sib_loc) {		// sib at right side suggests cur is head
				cur.p.pop_back();
				for (int ii=0; ii < sib.size-1; ii++)
					cur.k.push_back(sib.k[ii]);
				for (int ii=0; ii < sib.size; ii++)
					cur.p.push_back(sib.p[ii]);
				BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
				push(sib_loc.fileBlockNum);
				return 3;
			} else {												//  sib is on the left
				sib.p[sib.size-1] = cur.p[0];
				BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
				push(cur_loc.fileBlockNum);
				return 2;
			}
		}
		min = cur.k[0];     // it will be useful to maintain k upward
		if ((cur.type == LEAFNODE)&&(cur.size == n/2)) {			//  too few children
			if (sib.size > n/2+1) {									//  can lend a child
				if (sib.k[0] < min) {								//  sib is left to cur
					cur.k.insert(cur.k.begin(), sib.k[sib.size-2]);   //  transfer tail
					sib.k.pop_back();
					cur.p.insert(cur.p.begin(), sib.p[sib.size-2]);
					cur.size++;
					sib.p.erase(sib.p.begin()+(sib.size-2));
					sib.size--;
					kcrit = cur.k[0];  //  update k of father
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
					return 0;
				} else {               //  sib at right side
					cur.k.push_back(sib.k[0]);				//  transfer head
					cur.p.insert(cur.p.begin()+(cur.size-1), sib.p[0]);
					cur.size++;
					sib.k.erase(sib.k.begin());
					sib.p.erase(sib.p.begin());
					sib.size--;
					kcrit = sib.k[0];   //  update k of father
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
					return (i==0);
				}
			} else {					// combine
				if (sib.k[0] < min) {	//  sib is left to cur
					sib.size += cur.size-1;
					sib.p.pop_back();
					for (int i=0; i<cur.size-1; i++)
						sib.k.push_back(cur.k[i]);
					for (int i=0; i<cur.size; i++)
						sib.p.push_back(cur.p[i]);
					BufferManager::writeBlockOfIndex(indexname, sib_loc.fileBlockNum, sib.toString(indexname));
					push(cur_loc.fileBlockNum);
					return 2;
				} else {                //  sib at right side
					cur.size += sib.size-1;
					cur.p.pop_back();
					for (int ii=0; ii<sib.size-1; ii++)
						cur.k.push_back(sib.k[ii]);
					for (int ii=0; ii<sib.size; ii++)
						cur.p.push_back(sib.p[ii]);
					BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
					push(sib_loc.fileBlockNum);
					return 2+(i==0);
				}
			}
		} else {
			BufferManager::writeBlockOfIndex(indexname, cur_loc.fileBlockNum, cur.toString(indexname));
			return i==0;
		}
	}
}
template <class Ktype>
void Btree<Ktype>::dfsprint(const Location& t_loc, int depth) {
	cout << "depth:" << depth<< "   ";
	BtreeNode<Ktype> t(indexname, t_loc);
	if ((t.type != LEAFNODE)&&(t.type != ROOT_LEAFNODE))
	for (unsigned i=0; i<t.size; i++)
		dfsprint(t.p[i], depth+1);
}
template <class Ktype>
Btree<Ktype>::Btree() {
	freelist.push_back(0);
	n = (VOLUMN-2*sizeof(int)+sizeof(Ktype))/(sizeof(Location) + sizeof(Ktype));
}
template <class Ktype>
Btree<Ktype>::Btree(const string& iname) {
	indexname = iname;
	map<string, int> a = TableRecord::Table_Columns_Type(IndexRecord::index_on_table[indexname]);
	int kk = a[IndexRecord::index_on_column[indexname]];
	if (kk < 2)
		n = (VOLUMN-2*sizeof(int)+sizeof(Ktype))/(sizeof(Location) + sizeof(Ktype));
	else
		n = (VOLUMN-2*sizeof(int)+(kk>>2))/(sizeof(Location) + (kk>>2));
	freelist = IndexRecord::remain[iname];
	root.fileBlockNum = *(freelist.end()-1);
	freelist.pop_back();
	root.offset = 0;
}
template <class Ktype>
Location Btree<Ktype>::Bsearch(const Ktype& x) {
	if (root.fileBlockNum == -1)
		return Location(-1, 0); 
	BtreeNode<Ktype> tmp(indexname, _BsearchNode(x));
	return tmp.findKx(x);
}
template <class Ktype>
Location Btree<Ktype>::Binsert(const Ktype& x, const Location& xp_loc) {
	Location res(0, root.fileBlockNum);
	Ktype kup;
	Location lup_loc = root;
	if (root.fileBlockNum < 0) {          //  empty tree
		BtreeNode<Ktype> r(ROOT_LEAFNODE, 2);
		r.k.push_back(x);
		r.p.push_back(xp_loc);
		r.p.push_back(Location(-1, 0));
		root.fileBlockNum = pop();
		root.offset = 0;
		BufferManager::writeBlockOfIndex(indexname, root.fileBlockNum, r.toString(indexname));
	} else
	if (_BinsertNode(root, x, xp_loc, kup, lup_loc) > 1) {    //  root split up
		BtreeNode<Ktype> r(indexname, root);
		r.type = (r.type == ROOT)?NONLEAFNODE:LEAFNODE;   //  no more root || root_leafnode
		BufferManager::writeBlockOfIndex(indexname, root.fileBlockNum, r.toString(indexname));  
		BtreeNode<Ktype> np(ROOT, 2);	
		np.k.push_back(kup);
		np.p.push_back(root);
		np.p.push_back(lup_loc);
		root.fileBlockNum = pop(); // new root
		root.offset = 0;
		BufferManager::writeBlockOfIndex(indexname, root.fileBlockNum, np.toString(indexname));  
	}
	res.fileBlockNum = root.fileBlockNum;
	return res;
}
template <class Ktype>
Location Btree<Ktype>::Bdelete(const Ktype& x) {		//  return -1 if null tree, 0 if x not found, 1 if deletion completed, 2 if root emptied
	Location re(root.fileBlockNum, root.fileBlockNum);
	Location sib(root.fileBlockNum, root.offset);
	Ktype tmp1, tmp2;
	if (root.fileBlockNum == -1)
		return -1;
	BtreeNode<Ktype> r(indexname, root);
	Location np = r.p[0];
	int res = _BdeleteNode(root, x, sib, tmp1, tmp2);
	if (res == -1) {
		return re;
	} else if (res == -2) {
		push(root.fileBlockNum);
		root = np;
		BtreeNode<Ktype> rr(indexname, root);
		rr.type = (rr.type==LEAFNODE)?ROOT_LEAFNODE:ROOT;
		BufferManager::writeBlockOfIndex(indexname, root.fileBlockNum, rr.toString(indexname));  
		re.fileBlockNum = root.fileBlockNum;
		return re;
	} else if (res == -3) {
		push(root.fileBlockNum);
		root.fileBlockNum = -1;
		root.offset = 0;
		re.fileBlockNum = -1;
		return re;
	} else
		return re;
}
template <class Ktype>
Btree<Ktype>::~Btree() {
	IndexRecord::tupleLength[indexname] = n;//写入size等信息
	freelist.push_back(root.fileBlockNum);
	IndexRecord::remain[indexname] = freelist;
}

int indexManager::getType() {
	map<string, int> a = TableRecord::Table_Columns_Type(IndexRecord::index_on_table[indexname]);
	return a[IndexRecord::index_on_column[indexname]];
}
int indexManager::getTypeLength() {
	int k = getType();
	switch (k) {
	case INTEGER:
		return INT_LEN;
	case FLOATNUM:
		return FLOAT_LEN;
	default:
		return k >> 2;
	}
}
Location indexManager::doSelectByIndex(const string& value) {
	int k = getType();
	if (k == INTEGER) {
		Btree<int> a(indexname);
		return a.Bsearch(stoi(value));
	} else if (k == FLOATNUM) {
		Btree<float> a(indexname);
		return a.Bsearch(stof(value));
	} else {
		Btree<string> a(indexname);
		return a.Bsearch(value);
	}
}
Location indexManager::insertIntoIndex(const string& value, const Location& loc) {
	int k = getType();
	if (k == INTEGER) {
		Btree<int> a(indexname);
		return a.Binsert(stoi(value), loc);
	} else if (k == FLOATNUM) {
		Btree<float> a(indexname);
		return a.Binsert(stof(value), loc);
	} else {
		Btree<string> a(indexname);
		return a.Binsert(value, loc);
	}
}
Location indexManager::deleteIndex(const string& value) {
	int k = getType();
	if (k == INTEGER) {
		Btree<int> a(indexname);
		return a.Bdelete(stoi(value));
	} else if (k == FLOATNUM) {
		Btree<float> a(indexname);
		return a.Bdelete(stof(value));
	} else {
		Btree<string> a(indexname);
		return a.Bdelete(value);
	}
}
void indexManager::createIndex() {
	IndexRecord::tupleLength[indexname] = 0;
	IndexRecord::remain.at(indexname).push_back(0);
	IndexRecord::remain.at(indexname).push_back(-1);
	ofstream f(indexname+".index", ios::out);
	f.close();
}
void indexManager::dropIndex() {
	fstream test(indexname+".index");
	bool exist;
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((indexname+".index").c_str());
}
