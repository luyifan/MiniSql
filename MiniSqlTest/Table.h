//
//  Table.h
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013 luyifan. All rights reserved.
//

#ifndef __MiniSql__Table__
#define __MiniSql__Table__

#include <string>
#include <vector>
#include "Attribute.h"

using namespace std ;

class Table
{
public:
    string tableName ; 
    int counts ; 
    vector<int> blockNums ; 
    vector<int> remain ; 
    vector<Attribute> attributes ;
    int attributeNum ;
    int tupleLength ; 
    
    std::vector<std::string> index;
	Table(string tn):tableName(tn), counts(0){}
   
    
};



#endif /* defined(__MiniSql__Table__) */
