#include "Executor.h"
#include "TableRecord.h"
#include "IndexRecord.h"
#include <string>
#include "indexManager.h"
#include "Table.h"
#include <iostream>
#include "RecordManager.h"
using namespace std;

#ifdef TEST
string type_name[]={"Int", "Float", "Char"};
string operator_name[]={" = ", " != ", " < ", " > ", " <= ", " >= "};
#endif
#ifdef TEST_SPEED
static int select_count=0;
static int insert_count=0;
static int delete_count=0;
#endif
#ifdef TIMER
#include <ctime>
#include <cstdio>
	extern  time_t mystart;
	extern time_t lyfstart;
	extern time_t myend;
	extern time_t lyfend;
#endif
bool Executor::Select(const Operator &o){
#ifdef TEST
	cout<<"Select operation : \n";
	cout<<"Table name : "<<o.table_name<<"\n"
		<<"Selected columns : \n";
	for(int i=0; i<o.selected_col_of_table.size(); ++i)
		cout<<o.selected_col_of_table[i]<<"\n";
	cout<<"Requirements : \n";
	for(int i=0; i<o.col_in_where.size(); ++i)
		cout<<o.col_in_where[i]
				<<operator_name[o.condition_in_where[i]]
				<<o.values[i]<<"\n";
#endif
//判断用B+树搜索还是遍历搜索
	string Iname;
	string Bvalue;
	bool Bornot;
	Bornot=Select_By_Tree(o, Iname, Bvalue);

		Data data;
		vector<constraint> c;
		vector<int> columns;
		for(int i=0; i<o.condition_in_where.size(); ++i)
			c.push_back(tableforlyf::myconstrainttolyfconstraint(o.table_name, o.col_in_where[i], o.condition_in_where[i], o.values[i]));
		if(o.select_all){
			for(int i=0; i<tableforlyf::tableinfo.size(); ++i)
				if (tableforlyf::tableinfo[i].tableName==o.table_name){
#ifdef TIMER
					myend=lyfstart=clock();
#endif
					data = RecordManager::Select(tableforlyf::tableinfo[i], c, Bornot, Iname, Bvalue);//Select函数全选版本
#ifdef TIMER
					lyfend=clock();
#endif
				}
			Print_Select_Result(TableRecord::Table_Columns_Name(o.table_name), o.table_name, data);
		}	
		else{
			for(int i=0; i<o.selected_col_of_table.size(); ++i)
			columns.push_back(tableforlyf::myselectedtolyfselected(o.table_name, o.selected_col_of_table[i]));
			for(int i=0; i<tableforlyf::tableinfo.size(); ++i)
				if (tableforlyf::tableinfo[i].tableName==o.table_name){
#ifdef TIMER
					myend=lyfstart=clock();
#endif
					data = RecordManager::Select_Some(tableforlyf::tableinfo[i], c, columns, Bornot, Iname, Bvalue);//Select函数选择若干列版本
				}
#ifdef TIMER
					lyfend=clock();
#endif
			Print_Select_Result(o.selected_col_of_table, o.table_name, data);
		}
#ifdef TIMER
			cout<<"Time consumed : \n"<<"my : "<<(myend-mystart)*1.0/CLOCKS_PER_SEC
														<<"   llyf : "<<(lyfend-lyfstart)*1.0/CLOCKS_PER_SEC<<'\n';
#endif

#ifdef TEST_SPEED
		cout<<++select_count<<'\n';
#endif
	return true;
}

bool Executor::Delete(const Operator &o){
#ifdef TEST
	cout<<"Delete operation : \n";
	cout<<"Table name : "<<o.table_name<<"\n"
			<<"Requirements : \n";
	for(int i=0; i<o.col_in_where.size(); ++i)
		cout<<o.col_in_where[i]
				<<operator_name[o.condition_in_where[i]]
				<<o.values[i]<<"\n";
#endif
	string Iname;
	string Bvalue;
	bool Bornot;
 	Bornot=Select_By_Tree(o, Iname, Bvalue);

	vector<constraint> c;
	for(int i=0; i<o.condition_in_where.size(); ++i)
		c.push_back(tableforlyf::myconstrainttolyfconstraint(o.table_name, o.col_in_where[i], o.condition_in_where[i], o.values[i]));
//	}
	for(int i=0; i<tableforlyf::tableinfo.size(); ++i)
		if (tableforlyf::tableinfo[i].tableName==o.table_name){
#ifdef TIMER
			myend=lyfstart=clock();
#endif
			cout<<RecordManager::DeleteValue(tableforlyf::tableinfo[i], c, Bornot, Iname, Bvalue)<< " item(s) deleted\n";
		}
#ifdef TIMER
		lyfend=clock();
		cout<<"Time consumed : \n"<<"my : "<<(myend-mystart)*1.0/CLOCKS_PER_SEC
													<<"   llyf : "<<(lyfend-lyfstart)*1.0/CLOCKS_PER_SEC<<'\n';
#endif

#ifdef TEST_SPEED
		cout<<++delete_count<<'\n';
#endif
	return true;
}

bool Executor::Insert(const Operator &o){
#ifdef TEST
	cout<<"Insert operation : \n";
	cout<<"Table name : "<<o.table_name<<"\n"
			<<"Values : \n";
	for(int i=0; i<o.values.size(); ++i)
		cout<<o.values[i]<<"\n";
#endif
	try{
		string pkey=TableRecord::Table_Primary_Key(o.table_name);
		const vector<string> &col_name=TableRecord::Table_Columns_Name(o.table_name);
		const unordered_set<string> &unique=TableRecord::Table_Uniquenesses(o.table_name);
		Tuple t;
		for(int i=0; i<o.values.size(); ++i){
			if(((col_name[i]==pkey||unique.find(col_name[i])!=unique.end()))){//若该列是unique或primary key
				vector<constraint> c;
				c.push_back(tableforlyf::myconstrainttolyfconstraint(o.table_name, col_name[i], EQUAL, o.values[i]));
				string Iname;
				bool Bornot;
				Bornot=Select_By_Tree_Insert(o.table_name, col_name[i], Iname);
				for(int j=0; j<tableforlyf::tableinfo.size(); ++j)
					if (tableforlyf::tableinfo[j].tableName==o.table_name)
						if(RecordManager::Select(tableforlyf::tableinfo[j], c, Bornot, Iname, o.values[i]).tuples.size())
							throw Table_Insertion_Error("Already exists value \'"+Print_Value(TableRecord::Table_Columns_Type(o.table_name).at(col_name[i]), o.values[i])+"\' at unique column : "+col_name[i]);
						else break;
			}
			t.tuple.push_back(o.values[i]);
		}
		for(int i=0; i<tableforlyf::tableinfo.size(); ++i)
			if (tableforlyf::tableinfo[i].tableName==o.table_name){
#ifdef TIMER
				myend=lyfstart=clock();
#endif
				RecordManager::InsertValue(tableforlyf::tableinfo[i], t);
			}
		cout<<"Insertion succeeded\n";
#ifdef TIMER
		lyfend=clock();
		cout<<"Time consumed : \n"<<"my : "<<(myend-mystart)*1.0/CLOCKS_PER_SEC
													<<"   llyf : "<<(lyfend-lyfstart)*1.0/CLOCKS_PER_SEC<<'\n';
#endif

#ifdef TEST_SPEED
		cout<<++insert_count<<'\n';
#endif
		return true;
	}catch(Exception &e){
		e.Print();
	}
	return false;
}

bool Executor::CreateTable(const Operator &o){
#ifdef TEST
	cout<<"Create table operation : \n";
	cout<<"Table name : "<<o.table_name<<"\n"
		<<"Primary key : "<<o.primary_key<<"\n";
	cout<<"Columns : \n";
// ‰≥ˆ√ø¡––≈œ¢
	for(int i=0; i<o.table_attr_name.size(); ++i){
		string col_name=o.table_attr_name[i];
		cout<<col_name<<" ---- ";
		int type=o.table_attr_type.at(col_name);
		if(type>2)
			cout<<type_name[2]<<" ("<<type/4<<") ";
		else
			cout<<type_name[type];
		if(o.table_uniqueness.find(col_name)!=o.table_uniqueness.end())
			cout<<"  unique\n";
		else
			cout<<"\n";
	}
#endif
//‘ˆº”±Ì–≈œ¢
	TableRecord::Create_Table(o.table_name, o.primary_key, o.table_attr_name, o.table_attr_type, o.table_uniqueness);
//‘ˆº”÷˜º¸À˜“˝–≈œ¢
	IndexRecord::CreateIndex(o.table_name+"Pindex", o.table_name, o.primary_key);
	tableforlyf::tableinfo.push_back(tableforlyf::mytabletolyftable(o.table_name));
	RecordManager::CreateTable( o.table_name );
	indexManager iM(o.table_name+"Pindex");
	iM.createIndex();
	
	cout<<"Table "<<o.table_name<<" created.\n";
	return true;
}

bool Executor::DropTable(Operator &o){
#ifdef TEST
	cout<<"Drop table operation : \n"
			<<"Table name : "<<o.table_name<<"\n";
#endif
//…æ≥˝±Ì–≈œ¢
	TableRecord::Drop_Table(o.table_name);
//…æ≥˝À˘”–±Ì…œÀ˜“˝–≈œ¢
	int ite=tableforlyf::Find_Position2(o.table_name);
	o.table_of_index=o.table_name;
	for(int i=0; i<tableforlyf::tableinfo[ite].index.size(); ++i){
		o.index_name=tableforlyf::tableinfo[ite].index[i];
		DropIndex(o);
	}
	tableforlyf::Delete_Table(o.table_name);
	RecordManager::DropTable( o.table_name );
	fstream test;
	bool exist;
	test.open(TITLE+o.table_name+".tablerecord.column");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((TITLE+o.table_name+".tablerecord.column").c_str());
	test.open(TITLE+o.table_name+".tablerecord.unique");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((TITLE+o.table_name+".tablerecord.unique").c_str());
	cout<<"Table "<<o.table_name<<" dropped.\n";
	return true;
	}

bool Executor::CreateIndex(const Operator &o){
#ifdef TEST
	cout<<"Create index operation : \n";
	cout<<"Table name : "<<o.table_of_index<<"\n"
		<<"Index name : "<<o.index_name<<"\n"
		<<"Index on column "<<o.name_of_table_of_index<<"\n";
#endif
//‘ˆº”À˜“˝–≈œ¢
	IndexRecord::CreateIndex(o.index_name, o.table_of_index, o.name_of_table_of_index);
	tableforlyf::Add_Index(o.table_of_index, o.index_name);
	indexManager iM(o.index_name);
	iM.createIndex();
	for(int i=0; i<tableforlyf::tableinfo.size(); ++i){
		if(tableforlyf::tableinfo[i].tableName==o.table_of_index)
			RecordManager::InsertIndexByCreateIndex(tableforlyf::tableinfo[i], o.index_name);
	}
	cout<<"Index "<<o.table_of_index<<" created.\n";
	return true;
}

bool Executor::DropIndex(const Operator &o){
#ifdef TEST
	cout<<"Drop index operation : \n";
	cout<<"Index name : "<<o.index_name<<"\n";
#endif
//…æ≥˝À˜“˝–≈œ¢
	IndexRecord::DropIndex(o.index_name);
	tableforlyf::Delete_Index(o.table_of_index, o.index_name);
	indexManager iM(o.index_name);
	iM.dropIndex();
	BufferManager::DeleteBufferBlockOfIndex( o.index_name ) ;
	fstream test;
	bool exist;
	test.open(TITLE+o.index_name+".index.column1");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((TITLE+o.index_name+".index.column1").c_str());
	test.open(TITLE+o.index_name+".index.column2");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((TITLE+o.index_name+".index.column2").c_str());
	cout<<"Index "<<o.index_name<<" dropped.\n";
	return true;
}

bool Executor::Print_Select_Result(const vector<string> &selected_col_name, const string &tname, const Data &result){
	vector<int> length;//存储每列宽度
//判断列名和列数据宽度，取最大的就是该列宽度
	cout.setf(std::ios::left, std::ios::adjustfield);//左对齐
	for(int i=0; i<selected_col_name.size(); ++i){
		int type=(TableRecord::Table_Columns_Type(tname)).at(selected_col_name[i]);
		switch(type){
		case INTEGER:
			length.push_back(INT_LEN<selected_col_name[i].size()?selected_col_name[i].size():INT_LEN); break;
		case FLOATNUM: 
			length.push_back(FLOAT_LEN<selected_col_name[i].size()?selected_col_name[i].size():FLOAT_LEN); break;
		default: 
			length.push_back(type/4<selected_col_name[i].size()?selected_col_name[i].size():type/4); break;
		}
		cout.width(length.back()+PLACE_HOLDER);//输出列名
		cout<<selected_col_name[i];
	}
	cout<<'\n';
	for(int i=0; i<result.tuples.size(); ++i)
		Print_Row(result.tuples[i].tuple, length, selected_col_name, tname);
	cout<<"Totally "<<result.tuples.size()<<" item(s).\n";
	return true;
}

bool Executor::Print_Row(const vector<string> &values, const vector<int> &length, const vector<string> &selected_col_name, const string &tname){
	map<string, int> col_type=TableRecord::Table_Columns_Type(tname);
	for(int i=0; i<values.size(); ++i){
		int j=0; 
		if(col_type.at(selected_col_name[i])<2){//去除占位符
			while(values[i][j]=='0')
				++j;
			if(values[i].size()==j)
				--j;
			else if(values[i][j]=='.')
				--j;
		}
		else
			while(values[i][j]==' ')
				++j;
		cout.width(length[i]+PLACE_HOLDER);//输出该数值
		cout<<values[i].substr(j, values[i].size()-j);
	}
	cout<<'\n';
	return true;
}

string Executor::Print_Value(int type, const string &value){
	int i=0;
	switch(type){
		case INTEGER: case FLOATNUM: 
			while(value[i]=='0')
				i++;
			break;
		default:
			while(value[i]==' ')
				i++;
			break;
	}
	return value.substr(i, value.size()-i);
}

bool Executor::Select_By_Tree(const Operator &o, string &iname, string &bvalue){
	const vector<string> &indices=IndexRecord::All_Indices();

	for(int i=0; i<o.col_in_where.size(); ++i)
		for(int j=0; j<indices.size(); ++j)
			if(o.col_in_where[i]==IndexRecord::Column_Of_Index(indices[j])&&o.condition_in_where[i]==EQUAL){
				iname=indices[j];
				bvalue=o.values[i];
				return true;
			}
	return false;
}

bool Executor::Select_By_Tree_Insert(const string &tname, const string &colname, string &Iname){
	const Table &t=tableforlyf::Get_Table(tname);
	for(int i=0; i<t.index.size(); ++i)
		if(IndexRecord::Column_Of_Index((t.index)[i])==colname){
			Iname=(t.index)[i];
			return true;
		}
	return false;
}