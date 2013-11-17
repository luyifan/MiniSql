//
//  Buffer.cpp
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#include "BufferManager.h"
#include <fstream>
#include <istream>
#include <cstdio>
long BufferManager::minLRUValue = 0 ;
Block BufferManager::bufferBlock [ BLOCKNUMBER ] ;
BufferManager::BufferManager()
{
	minLRUValue = 0 ;
    for ( int i = 0 ; i < BLOCKNUMBER ; i++ )
        bufferBlock[ i ].initialization() ;
    
}
void BufferManager::BufferInitilize()
{
	minLRUValue = 0 ;
	for ( int i = 0 ; i < BLOCKNUMBER ; i++ )
		bufferBlock[ i ].initialization() ;

}
void BufferManager::BufferToFile ()
{
	for ( int i = 0 ; i < BLOCKNUMBER ; i++ )
		flashBack( i ) ;
}
BufferManager::~BufferManager()
{
    for ( int i = 0 ; i < BLOCKNUMBER ; i++ )
        flashBack( i ) ;
}

void BufferManager::flashBack(int  bufferNum)
{
	if ( bufferBlock [ bufferNum ].isWritten == true && bufferBlock[ bufferNum ].isValid == true )
    {
        
	   
		string fileName = bufferBlock[ bufferNum ].fileName ;
		fstream fout ( fileName.c_str(), ios::in | ios::out | ios::binary );
		fout.seekp( bufferBlock[ bufferNum ].offset*BLOCKSIZE , fout.beg) ;
		fout.write(bufferBlock[ bufferNum ].content, BLOCKSIZE);
	
        fout.close() ;
      

    }

}


int BufferManager::UseLeastBlock (const  string & fileName )
{
    long MaxLRUValue =  minLRUValue - 1;
    int MaxLRUIndex = -1 ;
    
    for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
    {
        if ( bufferBlock [ i ].isValid == false )
			return i ;
		if ( MaxLRUValue < bufferBlock[ i ].LRUValue && bufferBlock[ i ].fileName != fileName && bufferBlock[ i ].pinned == false )
        {
            MaxLRUValue = bufferBlock [ i ].LRUValue ;
            MaxLRUIndex = i ;
        }
    }
    if ( MaxLRUIndex == -1 ) 
    {
        for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
			if ( MaxLRUValue < bufferBlock [ i ].LRUValue && bufferBlock[ i ].pinned == false )
            {
                MaxLRUValue = bufferBlock [ i ].LRUValue ;
                MaxLRUIndex = i ;
            }
    }
    if ( MaxLRUIndex == -1 )
	{
		cout << "The all blocks is pinned ;" << endl ;
		for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
		{
			if ( MaxLRUValue < bufferBlock [ i ].LRUValue  )
            {
                MaxLRUValue = bufferBlock [ i ].LRUValue ;
                MaxLRUIndex = i ;
            }
		}
	}
    return MaxLRUIndex ;
}

void BufferManager::ReadBlock( int BlockNum, const string &  fileName, int offset)
{
    flashBack( BlockNum );
    bufferBlock [ BlockNum ].isValid = true ;
    bufferBlock [ BlockNum ].fileName = fileName ;
    bufferBlock [ BlockNum ].offset = offset ;
	//判断文件类型是index还是table，如果是index，那么就要把树的根给pinned

    bufferBlock [ BlockNum ].isWritten = false ;
    bufferBlock [ BlockNum ].LRUValue = --minLRUValue ;
    if ( minLRUValue < -2000000000 ) 
        UpdateLRUValue ( );
    fstream fin ( fileName.c_str() , ios::binary| ios::in );
	   
	fin.seekp ( offset*BLOCKSIZE , fin.beg ) ;
	fin.read( bufferBlock[ BlockNum ].content , BLOCKSIZE ) ;
	
	fin.close( ) ;
}


void BufferManager::UpdateLRUValue (  )
{
    for ( int i = 0 ; i < BLOCKNUMBER ; i++ )
        bufferBlock[ i ].LRUValue -= minLRUValue ;
	minLRUValue = 0 ;
    
}

PositionToInsert BufferManager::getInsertPosition(Table &table)
{
    int blockNum ;
    PositionToInsert position ;
    int index = -1 ;
    string fileName = table.tableName + ".table" ;
    if ( table.counts == 0 )
    {
        position.blockNum = addBlockForFile( table );
		position.fileBlockNum = 0 ;
        position.offset = 0 ;
        table.remain [ 0 ] -- ;
        return position ;
    }
    for ( int i = 0 ; i < table.counts ; i ++ )
    {
        blockNum = table.blockNums [ i ] ;
        
        if ( bufferBlock [ blockNum ].fileName == fileName &&  bufferBlock [ blockNum ].offset == i && bufferBlock
			[ blockNum ].isValid == true )
            if ( table.remain [ i ] != 0 )
            {
                index  = i ;
				bufferBlock[ blockNum ].LRUValue = --minLRUValue;
				if ( minLRUValue < 2000000000 )
					UpdateLRUValue( ) ;
                break ;
            }
    }
    if ( index == -1 )
    {
        for ( int i = 0 ; i < table.counts ; i ++ )
        if ( table.remain [ i ] != 0 )
        {
            index = i ;
            blockNum = UseLeastBlock( fileName );
			table.blockNums [ index ] = blockNum ;
            ReadBlock( blockNum , fileName , index );
            break ;
        }
    }
    if ( index == -1 )
    {
        position.blockNum = addBlockForFile( table );
        position.offset = 0 ;
		position.fileBlockNum = table.counts - 1 ;
        table.remain [ table.counts - 1 ] -- ;
        return position ;
    }
    table.remain [ index ] -- ;
    for ( int i = 0 ; i < BLOCKSIZE ; i = i + table.tupleLength )
    if ( bufferBlock[ blockNum ].content [ i ] == EMPTY )
    {
        position.blockNum = blockNum ;
        position.offset = i ;
		position.fileBlockNum = index ;
        break ;
    }
    return position ;
}

int BufferManager::addBlockForFile ( Table & table )
{
    string fileName = table.tableName + ".table" ;
    int blockNum = UseLeastBlock ( fileName );
    table.counts ++ ;
    table.blockNums.push_back( blockNum );
    table.remain.push_back( BLOCKSIZE / table.tupleLength ) ;
    flashBack( blockNum );
    bufferBlock [ blockNum ].fileName = fileName ;
    bufferBlock [ blockNum ].offset = table.counts - 1 ; 
    bufferBlock [ blockNum ].isValid = true ;
    bufferBlock [ blockNum ].isWritten = true ;
    bufferBlock [ blockNum ].LRUValue = --minLRUValue ;
	if ( minLRUValue < 2000000000 )
		UpdateLRUValue( ) ;
    for ( int i = 0 ; i < BLOCKSIZE ; i = i + table.tupleLength )
        bufferBlock [ blockNum ].content [ i ] = EMPTY ;
    return blockNum ;
    
}

 string BufferManager::readBlockOfIndex ( const string & indexName , int offset )
 {
	 string temp = "" ;
	 int blockNum = -1 ;
	 string fileName = indexName + ".index" ;
	 //暴力法
/*	 for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
		 if ( bufferBlock [ i ].fileName == fileName && bufferBlock [ i ].offset == offset && bufferBlock [ i ].isValid == true )
		 {
			 blockNum = i ;
			 break ;
		 }
	if ( blockNum == -1 )
	{
		blockNum = UseLeastBlock ( fileName );
		ReadBlock ( blockNum , fileName , offset );
	}
	
	for ( int i = 0 ; i < BLOCKSIZE ; i ++ )
		temp += bufferBlock [ blockNum ].content [ i ] ;
	return temp ;
	*/
	//优化版
	blockNum = IndexRecord::blockNums [ indexName ][ offset ];
	if (!( bufferBlock [ blockNum ].fileName == fileName && bufferBlock [ blockNum ].offset == offset && bufferBlock [ blockNum ].isValid == true ))
	{
		blockNum = UseLeastBlock ( fileName );
		IndexRecord::blockNums [ indexName ][ offset ] = blockNum ;
		ReadBlock ( blockNum , fileName , offset );
	}
	
	for ( int i = 0 ; i < BLOCKSIZE ; i ++ )
		temp += bufferBlock [ blockNum ].content [ i ] ;
	return temp ;




 }

 
  string BufferManager::readPartOfBlockOfIndex (const  string & indexName , int offset , int start , int len )
 {
	 int blockNum = -1 ;
	 string temp = "" ;
	 string fileName = indexName + ".index" ;
	 //暴力法
/*	 for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
		 if ( bufferBlock [ i ].fileName == fileName && bufferBlock [ i ].offset == offset && bufferBlock [ i ].isValid == true )
		 {
			 blockNum = i ;
			 break ;
		 }
	if ( blockNum == -1 )
	{
		blockNum = UseLeastBlock ( fileName );
		ReadBlock ( blockNum , fileName , offset );
	}
	
	for ( int i = start ; i < start + len ; i ++ )
		temp += bufferBlock [ blockNum ].content [ i ] ;
	return temp ;
	*/
	//优化版
	blockNum = IndexRecord::blockNums [ indexName ][ offset ];
	if (!( bufferBlock [ blockNum ].fileName == fileName && bufferBlock [ blockNum ].offset == offset && bufferBlock [ blockNum ].isValid == true ))
	{
		blockNum = UseLeastBlock ( fileName );
		IndexRecord::blockNums [ indexName ][ offset ] = blockNum ;
		ReadBlock ( blockNum , fileName , offset );
	}
	
	for ( int i = start ; i < start + len ; i ++ )
		temp += bufferBlock [ blockNum ].content [ i ] ;
	return temp ;




 }

 void BufferManager::writeBlockOfIndex ( const string &  indexName , int offset , const string  & content )
 {
	 int blockNum = -1 ;
	 string fileName = indexName + ".index" ;
	 //暴力法
/*	 for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
		 if ( bufferBlock [ i ].fileName == fileName && bufferBlock [ i ].offset == offset && bufferBlock [ i ].isValid == true )
		 {
			 blockNum = i ;
			 break ;
		 }
	if ( blockNum == -1 )
	{
		blockNum = UseLeastBlock ( fileName );
		ReadBlock ( blockNum , fileName , offset );
	}
	for ( int i = 0 ; i < BLOCKSIZE ; i++ )
		bufferBlock [ blockNum ].content [ i ] = content [ i ] ;
	bufferBlock[ blockNum ].isWritten = true ;
	*/
	//优化版
	if ( IndexRecord::index_counts [ indexName ] - 1 < offset )
	{
		for ( int i = IndexRecord::index_counts [ indexName ] ; i <= offset ; i ++ )
		{
			addBlockForIndex ( indexName );
		}
	}
	blockNum = IndexRecord::blockNums [ indexName ] [ offset ] ;
	if (!( bufferBlock [ blockNum ].fileName == fileName && bufferBlock [ blockNum ].offset == offset && bufferBlock [ blockNum ].isValid == true ))
	{
		blockNum = UseLeastBlock ( fileName );
		IndexRecord::blockNums [ indexName ][ offset ] = blockNum ;
		ReadBlock ( blockNum , fileName , offset );
	}
	for ( int i = 0 ; i < BLOCKSIZE ; i++ )
		bufferBlock [ blockNum ].content [ i ] = content [ i ] ;
	bufferBlock[ blockNum ].isWritten = true ;

 }

 void BufferManager::writePartOfBlockOfIndex ( const string &  indexName , int offset , int start , const string &  content )
 {
	 int blockNum = -1 ;
	 string fileName = indexName + ".index" ;
	 //暴力法
/*	 for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
		 if ( bufferBlock [ i ].fileName == fileName && bufferBlock [ i ].offset == offset && bufferBlock [ i ].isValid == true )
		 {
			 blockNum = i ;
			 break ;
		 }
	if ( blockNum == -1 )
	{
		blockNum = UseLeastBlock ( fileName );
		ReadBlock ( blockNum , fileName , offset );
	}
	for ( unsigned i = start ; i < start + content.length()  ; i++ )
		bufferBlock [ blockNum ].content [ i ] = content [ i ] ;
	bufferBlock[ blockNum ].isWritten = true ;
	*/
	//优化版
	if ( IndexRecord::index_counts [ indexName ] - 1 < offset )
	{
		for ( int i = IndexRecord::index_counts [ indexName ] ; i <= offset ; i ++ )
		{
			addBlockForIndex ( indexName );
		}
	}
	blockNum = IndexRecord::blockNums [ indexName ] [ offset ] ;
	if (!( bufferBlock [ blockNum ].fileName == fileName && bufferBlock [ blockNum ].offset == offset && bufferBlock [ blockNum ].isValid == true ))
	{
		blockNum = UseLeastBlock ( fileName );
		IndexRecord::blockNums [ indexName ][ offset ] = blockNum ;
		ReadBlock ( blockNum , fileName , offset );
	}
	for ( unsigned i = start ; i < start + content.length()  ; i++ )
		bufferBlock [ blockNum ].content [ i ] = content [ i ] ;
	bufferBlock[ blockNum ].isWritten = true ;

 }
 int BufferManager::addBlockForIndex (const string &  indexName  )
{
    string fileName = indexName + ".index" ;
    int blockNum = UseLeastBlock ( fileName );
	if ( IndexRecord::index_counts [ indexName ] == 0 )
	{
		bufferBlock [ blockNum ].pinned = true ;
	}
    IndexRecord::index_counts [ indexName ] ++ ;
	IndexRecord::blockNums [ indexName ].push_back ( blockNum ) ;
    flashBack( blockNum );
    bufferBlock [ blockNum ].fileName = fileName ;
	bufferBlock [ blockNum ].offset = IndexRecord::index_counts [ indexName ] - 1;
    bufferBlock [ blockNum ].isValid = true ;
    bufferBlock [ blockNum ].isWritten = true ;
    bufferBlock [ blockNum ].LRUValue = --minLRUValue ;
	if ( minLRUValue < 2000000000 )
		UpdateLRUValue( ) ;
    return blockNum ;
    
}


 void BufferManager::DeleteBufferBlockOfTable (const  string &  TableName ) 
  {
	  string filename = TableName + ".table" ;
	  for ( int i = 0 ; i < BLOCKNUMBER ; i ++ ) 
	  {
		  if ( bufferBlock[ i ].fileName == filename )
		  {
			  bufferBlock[ i ].isValid = false ;
		  }
	  }
  }
 void BufferManager::DeleteBufferBlockOfIndex ( const string &  IndexName )
   {
		string filename = IndexName + ".index" ;
		for ( int i = 0 ; i < BLOCKNUMBER ; i ++ ) 
		{
			if ( bufferBlock[ i ].fileName == filename )
			{
				bufferBlock[ i ].isValid = false ;
			}
		}
   }  
void BufferManager::ReleasePinned ( const string &  IndexName , int fileBlockNum ) 
   {
	  if ( fileBlockNum == -1 ) return ;
	  string filename = IndexName + ".index" ;
	  int i ;
	  for (  i = 0 ; i < BLOCKNUMBER ; i++ )
		  if ( filename ==  bufferBlock[ i ].fileName && bufferBlock[ i ].offset == fileBlockNum )
			break ;
	  if ( i < BLOCKNUMBER )
			bufferBlock[ i ].pinned = false ;
   }
	 void BufferManager::GetLock (const  string & IndexName , int fileBlockNum )
	{
		int i ;
		if ( fileBlockNum == - 1 ) return ;
		string filename = IndexName + ".index" ;
		for ( i = 0 ; i < BLOCKNUMBER ; i++ )
		if ( filename ==  bufferBlock[ i ].fileName && bufferBlock[ i ].offset == fileBlockNum )
			break ;
		if ( i < BLOCKNUMBER )
		{
			BufferManager::bufferBlock[ i ].pinned = true ;
		}
		else
		{
				int blockNum = UseLeastBlock ( filename );
				IndexRecord::blockNums [ IndexName ][ fileBlockNum ] = blockNum ;
				ReadBlock ( blockNum , filename , fileBlockNum );
				BufferManager::bufferBlock[ blockNum ].pinned = true ;
		}

	}