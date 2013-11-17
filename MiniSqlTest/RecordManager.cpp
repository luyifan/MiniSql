//
//  RecordManager.cpp
//  MiniSql
//
//  Created by luyifan on 13-10-20.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#include "RecordManager.h"
#include "TableRecord.h"
#include <cstdlib>
#include "indexManager.h"

void RecordManager::CreateTable( const string &  tableName)
{
    string name = tableName + ".table"  ;
    fstream fout ( name.c_str() , ios::out );
    fout.close() ;
}

void RecordManager::DropTable(  const string &  tableName )
{
    string name = tableName + ".table" ;
    fstream _file ;
    _file.open( name.c_str() , ios::in );
    if ( !_file )
    {
        cout << name <<"doesn't exist" << endl ;
    }
	_file.close();
	if ( remove( name.c_str() ) != 0 )
		cout << "Error deleting file"  << endl  ;
	BufferManager::DeleteBufferBlockOfTable( tableName ) ;
}


Data RecordManager::Select(  Table & table )
{
    string fileName = table.tableName + ".table";
    Tuple oneTuple ;

    Data data ;
    int offset ;
    int blockNum ;
    for ( int i = 0 ; i < table.counts ; i ++ )
    {
        blockNum = table.blockNums [ i ] ;
        if ( BufferManager::bufferBlock[ blockNum ].fileName != fileName || BufferManager::bufferBlock[ blockNum ].offset != i )
        {
            blockNum = BufferManager::UseLeastBlock( fileName ) ;
            BufferManager::ReadBlock( blockNum, fileName, i ) ;
            table.blockNums [ i ] = blockNum ;
        }
        
        for ( offset = 0 ; offset < BLOCKSIZE ; offset += table.tupleLength )
        if ( BufferManager::bufferBlock [ blockNum ].content [ offset ] == USED )
        {
            oneTuple = SplitString (  table ,  offset + 1 , blockNum ) ; 
            data.tuples.push_back( oneTuple );
        }
            
            
    }
    return data ;
}

Data RecordManager::Select( Table &table , const vector<constraint> & constraints , bool Bornot ,  const string &  Iname ,  const string &  Bvalue )
{
    string fileName = table.tableName + ".table" ;
    Tuple oneTuple ;
    Data data ;
    int offset ;
    int blockNum ;
	if (Bornot == false )
	{
		for ( int i = 0 ; i < table.counts ; i++ )
		{
			blockNum = table.blockNums [ i ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != fileName || BufferManager::bufferBlock [ blockNum ].offset != i || BufferManager::bufferBlock [ blockNum ].isValid == false )
			{
				blockNum = BufferManager::UseLeastBlock( fileName );
				BufferManager::ReadBlock( blockNum , fileName , i ) ;
				table.blockNums [ i ] = blockNum ;
			}
			for ( offset = 0 ; offset < BLOCKSIZE ; offset += table.tupleLength )
			if ( BufferManager::bufferBlock[ blockNum ].content [ offset ] == USED )
			{
				oneTuple = SplitString(   table , offset + 1, blockNum );
				if ( CheckConstrains( table , oneTuple , constraints ) )
					data.tuples.push_back( oneTuple );
                
			}
		}
	}
	else
	{
		Location tuplelocation ;
		
		//查找通过index
		indexManager iM(Iname);
		tuplelocation = iM.doSelectByIndex ( Bvalue );
		if ( tuplelocation.fileBlockNum != -1 )
		{
			blockNum = table.blockNums [ tuplelocation.fileBlockNum ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != fileName || BufferManager::bufferBlock [ blockNum ].offset != tuplelocation.fileBlockNum || BufferManager::bufferBlock [ blockNum ].isValid == false 
				)
			{
				blockNum = BufferManager::UseLeastBlock( fileName );
				BufferManager::ReadBlock( blockNum , fileName ,  tuplelocation.fileBlockNum ) ;	
				table.blockNums [ tuplelocation.fileBlockNum ] = blockNum ;
			}
			oneTuple = SplitString(  table , tuplelocation.offset + 1 , blockNum );
			data.tuples.push_back ( oneTuple );	
		}
	}
    return data ;
}
bool RecordManager::CheckConstrains( Table & table , const Tuple & tuple , const vector<constraint> & constraints) 
{
    int num ;
    CONSTRAINTTYPE conType ;
    
    
    for ( int i = 0 ; i < constraints.size() ; i ++ )
    {
        num = constraints [ i ].num ;
        conType = constraints [ i ].conType ;
        switch ( table.attributes[ num ].type )
        {
        
            case INT:
            {
                int value2 = atoi( constraints[ i ].value.c_str());
                int value1 = atoi( tuple.tuple [ num ].c_str() );
                switch ( conType) {
                    case Lt:
                        if ( value1 >= value2 ) //<
                            return false ;
                        break ;
                    case Le:
                        if ( value1 > value2 ) //<=
                            return false;
                        break ;
                    case Eq:
                        if ( value1 != value2 ) //==
                            return false ;
                        break ;
                    case Ne:
                        if ( value1 == value2 ) //!=
                            return false ;
                        break ;
                    case Ge:
                        if ( value1 < value2  ) //>=
                            return false ;
                        break ;
                    case Gt:
                        if ( value1 <= value2 ) //>
                            return false ;
                        break ;
                }
            }
                break ;
            case FLOAT:
            {
                float value2 = atof( constraints[ i ].value.c_str() );
                float value1 = atof( tuple.tuple [ num ].c_str() );
                switch ( conType) {
                    case Lt:
                        if ( value1 >= value2 ) //<
                            return false ;
                        break ;
                    case Le:
                        if ( value1 > value2 ) //<=
                            return false;
                        break ;
                    case Eq:
                        if ( value1 != value2 ) //==
                            return false ;
                        break ;
                    case Ne:
                        if ( value1 == value2 ) //!=
                            return false ;
                        break ;
                    case Ge:
                        if ( value1 < value2  ) //>=
                            return false ;
                        break ;
                    case Gt:
                        if ( value1 <= value2 ) //>
                            return false ;
                        break ;
                }
            }
                break ;
            case CHAR:
            {
                string value2 =  constraints[ i ].value;
                string value1 =  tuple.tuple [ num ] ;
                switch ( conType) {
                    case Lt:
                        if ( value1 >= value2 ) //<
                            return false ;
                        break ;
                    case Le:
                        if ( value1 > value2 ) //<=
                            return false;
                        break ;
                    case Eq:
                        if ( value1 != value2 ) //==
                            return false ;
                        break ;
                    case Ne:
                        if ( value1 == value2 ) //!=
                            return false ;
                        break ;
                    case Ge:
                        if ( value1 < value2  ) //>=
                            return false ;
                        break ;
                    case Gt:
                        if ( value1 <= value2 ) //>
                            return false ;
                        break ;
                }

            }
                break ;
            
            
                
        }
    }
    return true ;
    
}
Tuple RecordManager::SplitString (  Table & table , int start , int blockNum )
{
    int attributeLen ;
    int j = start ;
    string o ;
    Tuple tuple ;
    for ( int i = 0 ; i < table.attributeNum ; i ++ )
    {
        attributeLen = table.attributes [ i ].len ;
        o = BufferManager::bufferBlock [ blockNum ].getValues( j , attributeLen , BufferManager::minLRUValue );
		
        j += attributeLen ;
        tuple.tuple.push_back( o ) ;
    }
	if ( BufferManager::minLRUValue < 2000000000 )
			BufferManager::UpdateLRUValue();
    return tuple ;
    
}

string RecordManager::ConnectTuple(const Tuple &  tuple)
{
    string recordString ;
    for ( int i = 0 ; i < tuple.tuple.size() ; i ++ )
        recordString += tuple.tuple[ i ] ;
    
    return recordString ;
    
    
}
bool RecordManager::InsertValue( Table & table, const Tuple &  tuple)
{
    int blockNum ;
    int j ;
    string recordString = ConnectTuple( tuple );
    PositionToInsert position =  BufferManager::getInsertPosition ( table );
    blockNum = position.blockNum ;
    j = position.offset ;
    BufferManager::bufferBlock [ blockNum ].content [ j ] = USED ;
    j ++ ;
    for ( int i = 0 ; i < table.tupleLength - 1 ; i ++ , j ++  )
    {
        BufferManager::bufferBlock [ blockNum ].content [ j ] = recordString [ i ] ;
    }
    BufferManager::bufferBlock [ blockNum ].isWritten = true ;
	//加入调用index的部分
	vector<string> all_index_name = TableRecord::Table_Columns_Name( table.tableName ) ;
	for ( int i = 0 ; i < table.index.size() ; i ++)
	{
		string indexName = table.index [ i ] ;
		string name = IndexRecord::index_on_column[ indexName ] ;
		for (  j = 0 ; j < all_index_name.size() ; j ++)
			if ( name == all_index_name [ j ] ) break ;
		Location pLocation ;
		pLocation.fileBlockNum = position.fileBlockNum  ;
		pLocation.offset = position.offset ;
		indexManager iM(table.index[ i ] );
		pLocation = iM.insertIntoIndex ( tuple.tuple[ j ] , pLocation );
		if ( pLocation.fileBlockNum != pLocation.offset )
		{
			BufferManager::ReleasePinned( indexName , pLocation.offset ) ;
			BufferManager::GetLock ( indexName , pLocation.fileBlockNum ) ;
		}
	}
	
    return true ;
    
}

long RecordManager::DeleteValue( Table & table)
{
    long counts = 0 ;
    long count ;
    int blockNum ;
    int offset ;
	Tuple oneTuple ;
	int jj ;
    string filename = table.tableName + ".table" ;
	vector<string> all_index_name = TableRecord::Table_Columns_Name( table.tableName ) ;
    for ( int i = 0; i < table.counts ; i ++ )
    {
        count = 0  ;
        blockNum = table.blockNums [ i ] ;
        if ( BufferManager::bufferBlock [ blockNum ].fileName != filename || BufferManager::bufferBlock [ blockNum ].offset != i || BufferManager::bufferBlock[ blockNum ].isValid == false )
      
        {
            blockNum = BufferManager::UseLeastBlock( filename );
            BufferManager::ReadBlock( blockNum, filename, i );
            table.blockNums[ i ] = blockNum ;
        }
        for (  offset = 0 ; offset < BLOCKSIZE ; offset = offset + table.tupleLength )
        if ( BufferManager::bufferBlock [ blockNum ].content[ offset ] != EMPTY )
        {
             BufferManager::bufferBlock [ blockNum ].content[ offset ] = EMPTY ;
			 oneTuple = SplitString( table, offset + 1, blockNum ) ;
			 table.remain [ i ] -- ;
			 //调用index中的删除
			 
			
			 for ( int ii = 0 ; ii < table.index.size() ; ii ++ )
			 {
				string indexName = table.index [ ii ] ;
				string name = IndexRecord::index_on_column [ indexName ] ;
				for (  jj = 0 ; jj < all_index_name.size() ; jj ++)
					if ( all_index_name [ jj ] == name ) break ;
				Location pLocation ;
				indexManager iM(table.index[ ii ] );
				pLocation = iM.deleteIndex ( oneTuple.tuple[ jj ] ) ;
				if ( pLocation.fileBlockNum != pLocation.offset )
				{
					BufferManager::ReleasePinned( indexName , pLocation.fileBlockNum ) ;
					BufferManager::GetLock ( indexName , pLocation.offset ) ;
				}
				
			 }	
            count ++ ;
        }
        if ( count != 0 )
            BufferManager::bufferBlock [ blockNum ].isWritten = 1 ;
        counts += count ;
        
    }
    
    return counts ;
    
}

long RecordManager::DeleteValue( Table &table , const vector<constraint> & constraints , bool Bornot ,  const string &  Iname ,  const string &  Bvalue )
{
    long counts = 0 ;
    long count ;
    int blockNum ;
    int offset ;
	int jj ;
    Tuple oneTuple ;
    string filename = table.tableName + ".table" ;
	vector<string> all_index_name = TableRecord::Table_Columns_Name( table.tableName ) ;
 
	if (  Bornot == false )
	{
		for ( int i = 0 ; i < table.counts ; i++ )
		{
			count = 0 ;
			blockNum = table.blockNums [ i ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != filename || BufferManager::bufferBlock [ blockNum ].offset != i || BufferManager::bufferBlock [ blockNum ].isValid == false )
			{
				blockNum = BufferManager::UseLeastBlock( filename );
				BufferManager::ReadBlock( blockNum, filename, i ) ;
				table.blockNums[ i ] = blockNum ;
			}
			for (  offset = 0 ; offset < BLOCKSIZE ; offset = offset + table.tupleLength )
			if ( BufferManager::bufferBlock [ blockNum].content [ offset ] != EMPTY )
			{
				oneTuple = SplitString( table, offset + 1, blockNum ) ;
				if ( CheckConstrains( table, oneTuple, constraints) )
				{
					BufferManager::bufferBlock [ blockNum ].content [ offset ] = EMPTY ;
					table.remain [ i ] -- ;
					 //调用index中的删除
					for ( int ii = 0 ; ii < table.index.size() ; ii ++ )
					{
						string indexName = table.index [ ii ] ;
						string name = IndexRecord::index_on_column [ indexName ] ;
						for ( jj = 0 ; jj < all_index_name.size() ; jj ++)
							if ( all_index_name [ jj ] == name ) break ;
						Location pLocation ;
						indexManager iM(table.index[ ii ] );
						pLocation = iM.deleteIndex ( oneTuple.tuple[ jj ] ) ;
						if ( pLocation.fileBlockNum != pLocation.offset )
						{
							BufferManager::ReleasePinned( indexName , pLocation.offset) ;
							BufferManager::GetLock ( indexName , pLocation.fileBlockNum ) ;
						}

					 }	
					

					count ++ ;
				}
			}
			if ( count != 0 )
				BufferManager::bufferBlock [ blockNum ].isWritten = true ;
			counts += count ;
		}
	}
	else
	{
		Location tuplelocation ;
		//*查找通过index
		indexManager iM(Iname);
		tuplelocation = iM.doSelectByIndex ( Bvalue );
		
		//如果没有select到
		if ( tuplelocation.fileBlockNum == -1 )
			counts = 0 ;
		else
		{

			
			blockNum = table.blockNums [ tuplelocation.fileBlockNum ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != filename || BufferManager::bufferBlock [ blockNum ].offset != tuplelocation.fileBlockNum || BufferManager::bufferBlock[ blockNum ].isValid == false 
				)
			{
				blockNum = BufferManager::UseLeastBlock( filename );
				BufferManager::ReadBlock( blockNum , filename ,  tuplelocation.fileBlockNum ) ;	
				table.blockNums [ tuplelocation.fileBlockNum ] = blockNum ;
			}
			BufferManager::bufferBlock[ blockNum ].content [ tuplelocation.offset ] = EMPTY;
			counts = 1 ;
			BufferManager::bufferBlock[ blockNum ].isWritten = true ;
			Location pLocation ;
			indexManager iM(Iname);
			 pLocation = iM.deleteIndex ( Bvalue ) ;
			if ( pLocation.fileBlockNum != pLocation.offset )
			{
				BufferManager::ReleasePinned( Iname , pLocation.offset) ;
				BufferManager::GetLock ( Iname , pLocation.fileBlockNum ) ;
			}
		}

	}
    return counts ;
}

Data RecordManager::Select_Some( Table & table , const vector<constraint> & constraints , vector<int> columns , bool Bornot ,  const string &  Iname ,  const string &  Bvalue ) 
{
	string fileName = table.tableName + ".table" ;
	Tuple oneTuple ;
	Tuple oneTuplePart ;
	Data data ;
	int offset ;
	int blockNum ;
	if ( Bornot == false )
	{
		for ( int i = 0 ; i < columns.size() ; i ++ ) 
			oneTuplePart.tuple.push_back ( "" ) ;

		for ( int i = 0 ; i < table.counts ; i++ )
		{
			blockNum = table.blockNums [ i ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != fileName || BufferManager::bufferBlock [ blockNum ].offset != i || BufferManager::bufferBlock[ blockNum ].isValid == false  )
			{
				blockNum = BufferManager::UseLeastBlock( fileName );
				BufferManager::ReadBlock( blockNum , fileName , i ) ;
				table.blockNums [ i ] = blockNum ;
			}
			for ( offset = 0 ; offset < BLOCKSIZE ; offset += table.tupleLength )
				if ( BufferManager::bufferBlock[ blockNum ].content [ offset ] == USED )
				{
					oneTuple = SplitString(   table , offset + 1, blockNum );
					if ( CheckConstrains( table , oneTuple , constraints ) )
					{
						for ( int j = 0 ; j < columns.size() ; j ++ ) 
							oneTuplePart.tuple [ j ] = oneTuple.tuple [ columns [ j ] ] ;
						data.tuples.push_back( oneTuplePart );
					}
				}
		}
	}
	else
	{
		Location tuplelocation ;
		//查找通过index
		indexManager iM(Iname);
		tuplelocation = iM.doSelectByIndex ( Bvalue );
		
		if ( tuplelocation.fileBlockNum != -1 )
		{
			for ( int i = 0 ; i < columns.size() ; i ++ ) 
				oneTuplePart.tuple.push_back ( "" ) ;

			blockNum = table.blockNums [ tuplelocation.fileBlockNum ] ;
			if ( BufferManager::bufferBlock [ blockNum ].fileName != fileName || BufferManager::bufferBlock [ blockNum ].offset != tuplelocation.fileBlockNum || BufferManager::bufferBlock[ blockNum ].isValid == false 
				)
			{
				blockNum = BufferManager::UseLeastBlock( fileName );
				BufferManager::ReadBlock( blockNum , fileName ,  tuplelocation.fileBlockNum ) ;	
				table.blockNums [ tuplelocation.fileBlockNum ] = blockNum ;
			}
			oneTuple = SplitString(  table , tuplelocation.offset + 1 , blockNum );
			for ( int j = 0 ; j < columns.size() ; j ++ ) 
								oneTuplePart.tuple [ j ] = oneTuple.tuple [ columns [ j ] ] ;
			data.tuples.push_back( oneTuplePart );
		}
	}
	return data ;

}




void RecordManager::InsertIndexByCreateIndex ( Table & table ,  const string &  Iname ) 
{
    int blockNum ;
    int offset ;
	Tuple oneTuple ;
	int jj ;
    string filename = table.tableName + ".table" ;
	vector<string> all_index_name = TableRecord::Table_Columns_Name( table.tableName ) ;
	string name = IndexRecord::index_on_column [ Iname ] ;
	indexManager iM(Iname );
	for ( jj = 0 ; jj < all_index_name.size() ; jj ++)
		if ( all_index_name [ jj ] == name ) break ;
    for ( int i = 0; i < table.counts ; i ++ )
    {
        blockNum = table.blockNums [ i ] ;
        if ( BufferManager::bufferBlock [ blockNum ].fileName != filename || BufferManager::bufferBlock [ blockNum ].offset != i || BufferManager::bufferBlock[ blockNum ].isValid == false )
      
        {
            blockNum = BufferManager::UseLeastBlock( filename );
            BufferManager::ReadBlock( blockNum, filename, i );
            table.blockNums[ i ] = blockNum ;
        }
		
		for (  offset = 0 ; offset < BLOCKSIZE ; offset = offset + table.tupleLength )
        if ( BufferManager::bufferBlock [ blockNum ].content[ offset ] != EMPTY )
        {
			 oneTuple = SplitString( table, offset + 1, blockNum ) ;
			 //调用index插入
			
			
			
				Location pLocation ;
				pLocation.fileBlockNum = i ;
				pLocation.offset = offset ;
				pLocation = iM.insertIntoIndex(oneTuple.tuple[ jj ] , pLocation );
				if ( pLocation.fileBlockNum != pLocation.offset )
				{
					BufferManager::ReleasePinned( Iname, pLocation.fileBlockNum ) ;
					BufferManager::GetLock ( Iname , pLocation.offset ) ;
				}
				
        
        }
        
    }
}