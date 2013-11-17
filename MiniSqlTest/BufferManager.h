//
//  Buffer.h
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#ifndef __MiniSql__Buffer__
#define __MiniSql__Buffer__

#include "IndexRecord.h"
#include "Macro.h"
#include "Block.h"
#include "Table.h"
class BufferManager
{
public:
    static Block bufferBlock [ BLOCKNUMBER ] ; 
    BufferManager();
    ~BufferManager();
   static void BufferInitilize (  ) ;

   static void BufferToFile ( );
   static void flashBack ( int bufferNum );
   static long minLRUValue ;
    

   static void ReadBlock ( int BlockNum , const string &  fileName , int offset );
    
   static int UseLeastBlock (const  string & fileName  );
    
   static void UpdateLRUValue ( );
    
   static PositionToInsert getInsertPosition ( Table & table );
    
   static int addBlockForFile ( Table & table );
   
   static string readBlockOfIndex ( const string & indexName , int offset );

   static void writeBlockOfIndex (  const string & indexName , int offset ,  const string &  content );

   static int addBlockForIndex (  const string &  indexName );

   static string readPartOfBlockOfIndex (  const string &  indexName , int offset , int start , int len ) ;

   static void writePartOfBlockOfIndex (  const string &   indexName , int offset , int start ,  const string &  content );

   
   static void DeleteBufferBlockOfTable (  const string &  TableName ) ;
   static void DeleteBufferBlockOfIndex (  const string &  IndexName );
   static void ReleasePinned (  const string &  IndexName , int fileBlockNum ) ;

	static void GetLock (  const string &  IndexName , int fileBlockNum );
};

#endif /* defined(__MiniSql__Buffer__) */

