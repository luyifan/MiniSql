//
//  RecordManager.h
//  MiniSql
//
//  Created by luyifan on 13-10-20.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#ifndef __MiniSql__RecordManager__
#define __MiniSql__RecordManager__

#include "BufferManager.h"
#include "Macro.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include "Table.h"
using namespace std ;
class RecordManager
{

public:
    static void CreateTable (  const string &  fileName );
    static void DropTable (  const string &  fileName );
    
    static Data Select (  Table & table );
    
    static Data Select (  Table & table ,const  vector<constraint> & constraints , bool Bornot ,  const string &  Iname ,  const string &  Bvalue );
    
    static Tuple SplitString (  Table & table ,  int starpositon , int blockNum );
    
    static string ConnectTuple ( const Tuple & tuple );
    
    static bool CheckConstrains ( Table & table , const Tuple & oneTuple , const vector<constraint> & constraints );
    
    static bool InsertValue (  Table & table , const Tuple & tuple );
    
    static long DeleteValue (   Table & table );
    
    static long DeleteValue (  Table & table , const vector<constraint> & constraints , bool Bornot ,  const string &  Iname ,  const string &  Bvalue );

	static Data Select_Some( Table & table , const vector<constraint> & constraints , vector<int> columns , bool Bornot ,  const string &  Iname ,  const string &  Bvalue ) ;
	
	static void InsertIndexByCreateIndex ( Table & table ,  const string &  Iname ) ;
};

#endif /* defined(__MiniSql__RecordManager__) */
