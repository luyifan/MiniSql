//
//  Block.h
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#ifndef __MiniSql__Block__
#define __MiniSql__Block__
#include <string>
#include <iostream>
#include "Macro.h"
using namespace std ;


class Block
{
public:
    string fileName ;
    int offset ;
    char content [  BLOCKSIZE   ] ;
    bool isWritten ; 
    bool isValid ;    
	long LRUValue ;
	long pinned ;
    
    Block() ;
    void initialization ( ) ;
    
    string getValues ( int startPos , int Len );

    string getValues ( int startPos , int Len , long & minLRU );

};

#endif /* defined(__MiniSql__Block__) */
