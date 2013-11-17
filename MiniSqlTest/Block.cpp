//
//  Block.cpp
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#include "Block.h"
Block::Block()
{
    initialization ( );
}

void Block::initialization()
{
    isValid = false ;
    isWritten = false ;
    LRUValue = 0 ;
	pinned = false ;
    
}

string Block::getValues ( int startPos , int len  )
{
    string temp = "" ;
    for ( int i = startPos ; i < startPos + len ; i++ )
        temp += content [ i ] ;


    return temp ;
}

string Block::getValues ( int startPos , int len  , long & minLRU )
{
	string temp = "" ;
	for ( int i = startPos ; i < startPos + len ; i++ )
		temp += content [ i ] ;
	if ( LRUValue != minLRU )
	{
		LRUValue = minLRU ;
		minLRU -- ;

	}


	return temp ;
}


