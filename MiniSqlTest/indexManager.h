#ifndef __INDEXMANAGER_H__
#define __INDEXMANAGER_H__

#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <map>
#include <fstream>

using namespace std;
#define VOLUMN 4096
const int LEAFNODE = 0;
const int NONLEAFNODE = 1;
const int ROOT = 2;
const int ROOT_LEAFNODE = 3;


template <class Ktype>
class BtreeNode {
	int strtoi(const string&);
	string itostr(const int&);
	Ktype strtok(const string&);
	string ktostr(const Ktype&);
public:
	int size;
	int type;		// leafnode. nonleafnode. root. root&leafnode.
	vector<Location> p;
	vector<Ktype> k;
	BtreeNode(int t, int n=0):type(t), size(n) {};
	BtreeNode(const string&, const Location&);
	string toString(const string&);
	unsigned findFirstK(const Ktype&);
	Location findLoc(const Ktype&);
	int findX(const Ktype& x);
	Location findKx(const Ktype&);
	int insertKx(const Ktype&, const Location&);
	void dispose();
	void print();
	~BtreeNode() {	p.clear();	k.clear();}
};

template <class Ktype>
class Btree {
private:
	int n;
	string indexname;
	vector<int> freelist;
	Location root;
	Location _BsearchNode(const Ktype&);
	int pop();
	void push(int);
	int _BinsertNode(const Location&, const Ktype&, const Location&, Ktype&, Location&);			//  return 0 if nothing, 1 if the min, 2 if split, 3 if min and split
	int _BdeleteNode(const Location&, const Ktype&, const Location&, Ktype&, Ktype&);
	void dfsprint(const Location&, int);
public:
	Btree();
	Btree(const string&);
	Location Bsearch(const Ktype&);
	Location Binsert(const Ktype&, const Location&);
	Location Bdelete(const Ktype& x);		//  return -1 if null tree, 0 if x not found, 1 if deletion completed, 2 if root emptied
	void rootprint() {	dfsprint(root, 0); }
	~Btree();
};

class indexManager{
private:
	string indexname;
	int getType();
	int getTypeLength();
public:
	indexManager(const string& i):indexname(i) {}
	Location doSelectByIndex(const string&);
	Location insertIntoIndex(const string&, const Location&);
	Location deleteIndex(const string&);
	void createIndex();
	void dropIndex();
};
#endif