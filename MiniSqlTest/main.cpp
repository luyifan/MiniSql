#include "parser.h"
#include "Executor.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include "BufferManager.h"
#include "TableRecord.h"
#include "IndexRecord.h"
#include "tableforlyf.h"
#include "test.h"

#ifdef TIMER
#include <ctime>
time_t mystart=0;
time_t lyfstart=0;
time_t myend=0;
time_t lyfend=0;
#endif

using namespace std;
#ifdef TOTAL_TIME
#include "ctime"
time_t tottimestart;
time_t tottimeend;
#endif

void Init();
void Windup();
int main(){
#ifdef TOTAL_TIME
	tottimestart=clock();
#endif
	Init();
	Query query;
	Parser parser;
    parser.goon=true;
	while(parser.goon){
		if(parser.flag==INPUT_FROM_FILE)
			Parser::infile>>query;
		else if(parser.flag==INPUT_FROM_CONSOLE)
			cin >> query;
		else{
			cout<<"Input Error"<<endl;
			exit(0);
		}
		parser.Input_Statement(query.getContent());
		if(parser.Parse()){
			Operator o=parser.Get_Operator();
			switch(o.opt){
				case TABLE_CREATE: Executor::CreateTable(o); break;
				case TABLE_DROP: Executor::DropTable(o); break;
				case TABLE_SELECT: Executor::Select(o); break;
				case TABLE_INSERT: Executor::Insert(o); break;
				case TABLE_DELETE: Executor::Delete(o); break;
				case INDEX_CREATE: Executor::CreateIndex(o); break;
				case INDEX_DROP: Executor::DropIndex(o); break;
				case EXEC_FILE:
				case QUIT: break;
				case -1: cout<<"Unknown error happened"<<endl;
			}

			cout<<"\n\n";
		}
	}
	Windup();
#ifdef TOTAL_TIME
	tottimeend=clock();
	cout<<"Total time : "<<(tottimeend-tottimestart)*1.0/CLOCKS_PER_SEC<<endl;
#endif
	//RecordManager::CreateTable( "jb" ) ;
	//RecordManager::DropTable( "jb") ;
	/*
	Table table1 ( "jb" ) ;

	Attribute attribute1 (  "row1" , INT , 2 , true , false );
	
	Attribute attribute2  (  "row2" ,  FLOAT , 3 , false , true ) ;
	Attribute attribute3  (  "row3" ,  CHAR , 3 , false , false );
	
	table1.attributes.push_back( attribute1 ) ;
	table1.attributes.push_back( attribute2 ) ;
	table1.attributes.push_back( attribute3 ) ;
	
	table1.attributeNum = 3 ;
	table1.tupleLength =  2 + 3 + 3 + 1 ;

	Tuple tuple2  ;
	tuple2.tuple.push_back ( "01" );
	tuple2.tuple.push_back ( "2.1" );
	tuple2.tuple.push_back ( "cat" ) ;
	//BufferManager::minLRUValue = 0 ;
	for ( int i = 0 ; i < BLOCKNUMBER ; i ++ )
	BufferManager::bufferBlock[ i ].initialization();

	BufferManager::BufferInitilize() ;
	

	Tuple tuple1  ;
	tuple1.tuple.push_back ( "02" );
	tuple1.tuple.push_back ( "2.2" );
	tuple1.tuple.push_back ( "dog" ) ;
	RecordManager::InsertValue(  table1 , tuple1 );
	RecordManager::InsertValue(  table1 , tuple2 );
	Data data = RecordManager::Select ( table1  ) ;
	constraint con1 ( 0 ,Gt ,"01" ) ;
	vector<constraint> cons ;
	cons.push_back( con1 ) ;
    data = RecordManager::Select( table1 , cons ) ;

	int l = RecordManager::DeleteValue( table1 , cons ) ;

	BufferManager::BufferToFile() ;
	*/
} 

void Init(){
	TableRecord::Read_From_Disk();
	IndexRecord::Read_From_Disk();
	tableforlyf::Read_From_Disk();
	Parser::opts_initial();
	return ;
}

void Windup(){
	BufferManager::BufferToFile();
	tableforlyf::Store_Into_Disk();
	IndexRecord::Store_Into_Disk();
	TableRecord::Store_Into_Disk();
	return ;
}