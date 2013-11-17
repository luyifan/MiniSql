//
//  Macro.h
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013 luyifan. All rights reserved.
//

#ifndef __MiniSql__Macro__
#define __MiniSql__Macro__

#include <vector>
#include <string>
using namespace std ;

typedef enum {
    INT ,
    CHAR ,
    FLOAT
}AttributeType ;

typedef enum
{
    Lt , // <
    Le , // <=
    Eq , // ==
    Ne , // !=
    Ge , // >=
    Gt , // >
    
}CONSTRAINTTYPE;
#define FLOATLENGTH 23

#define BLOCKSIZE 4096

#define BLOCKNUMBER 1000


#define EMPTY '$'
#define USED    '#'

class Tuple
{
public:
    vector<string> tuple ;

};
class Data
{
public:
    vector<Tuple> tuples ;
};

class constraint
{
public:
    int num ;
	CONSTRAINTTYPE conType ;
    string value ;
    constraint(int n, CONSTRAINTTYPE type ,  const std::string &v):num(n), conType( type ) , value(v){};
	 constraint(int n ,  const std::string &v):num(n), value(v){};
	constraint(){}

    
};

class PositionToInsert
{
public:
    int blockNum ;
    int offset ;
	int fileBlockNum ;//在文件中的偏移 
};
class Location{
public:
	int fileBlockNum;
	int offset;
	Location(int a=-1, int b=0):fileBlockNum(a), offset(b) {	}
	bool operator==(const Location& loc) const{		return (fileBlockNum == loc.fileBlockNum)&&(offset == loc.offset);	}
};

#endif /* defined(__MiniSql__Macro__) */
