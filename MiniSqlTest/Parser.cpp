#include "parser.h"
#include "exception.h"
#include "TableRecord.h"
#include "IndexRecord.h"
#include <sstream>
#include <fstream>

#ifdef TIMER
#include <cstdio>
#include <ctime>
#endif

using namespace std;
vector<string> Parser::opts;
int Parser::flag=INPUT_FROM_CONSOLE;
ifstream Parser::infile;


istream &operator >>(istream &in, Query &q){
	q.query="";
	while(1){
		std::string temp;
		getline(in,temp);
		q.query+=temp;
		if(temp[temp.length()-1]==';'){
			q.query.erase(q.query.size()-1, 1);
			break;
		}
	}
	return in;
}

std::ifstream &operator >>(std::ifstream &infile, Query &q){
	q.query="";
	while(infile.eof()!=true){
		std::string temp;
		getline(Parser::infile, temp);
		//if ( temp [ temp.size() - 1 ] == 'r')
		//	temp = temp.substr (  0 , temp.size( ) - 1 ) ;
		if(temp==""||temp[0]=='/')
			continue;
		q.query+=temp;
		if(temp[temp.length()-1]==';'){
			q.query.erase(q.query.size()-1, 1);
			break;
		}
	}
	if(infile.eof()){
		Parser::flag=INPUT_FROM_CONSOLE;
		infile.close();
	}
	return infile;
}

const std::string &Query::getContent() const{ return query; }

Operator &Parser::Get_Operator() { return optor; }

void Parser::Input_Statement(std::string input){ statement=input; }

void Parser::Clean(){ optor.Clean(); }//清除上次操作

Parser::Parser(string input):statement(input), goon(true){}

void Parser::Normalize(){
	bool flag=false;
	while(statement[0]==' ')
		statement.erase(0, 1);
	for(int i=0; i<statement.size(); ++i)
		if(statement[i]==' ')
			if(flag)
				statement.erase(i, 1);
			else
				flag=true;
		else
			flag=false;
	return ;
}

std::vector<std::string> Parser::Split(const string &input, const string &delimit){
	vector<string> sentences;
	int head=0, tail=0;
	tail=input.find(delimit, head);
	while(tail!=string::npos){
		sentences.push_back(input.substr(head, tail-head));
		head=tail+delimit.size();
		tail=input.find(delimit, head);
	}
	if(head<input.size())
		sentences.push_back(input.substr(head, input.size()-head));
	return sentences;
}

const string Parser::lstrip(const string &input, const string &elim) const{
	int i;
	for(i=0; i<input.size(); ++i)
		if(elim.find(input[i])==string::npos)
			break;
	return i==input.size()?"":input.substr(i, input.size()-i);
}

const string Parser::rstrip(const string &input, const string &elim) const{
	int i;
	for(i=input.size()-1; i>=0; --i)
		if(elim.find(input[i])==string::npos)
			break;
	return i==-1?"":input.substr(0, i+1);
}

bool Parser::Syntax_Check(){//检查语法错误以及确认主操作
	int type;
	for(type=0; type<opts.size(); ++type)//遍历所有可能操作
		if(statement.substr(0, opts[type].size())==opts[type])
			break;
	optor.opt=type;
	if(type<opts.size()){//如果发现操作，则将操作枚举值赋给optor
		if(type==2&&statement.find("from")==string::npos)//选择操作，需要有"from"
			throw Syntax_Error();
		if(type==3&&statement.find("values")==string::npos)//插入操作，需要有"values"
			throw Syntax_Error();
		if(type==4&&statement.find("where")==string::npos)//删除操作，需要有"where"
			throw Syntax_Error();
		if(type==5&&statement.find("on")==string::npos)//创建索引，需要有"on"
			throw Syntax_Error();
		return true;
	}
	else
		throw Syntax_Error();
}

bool Parser::Find_Col_Name(const vector<string> &names, const string &name){
	for(int i=0; i<names.size(); ++i)
		if(names[i]==name)
			return true;
	return false;
}

bool Parser::Get_Table_Create_Info(){
	int begin=statement.find('{');//找到{ }内的内容
	int end=statement.find('}');
	if(begin==string::npos||end==string::npos)
		throw Table_Creation_Error("Couldn't find curly braces");
	//判断表是否已经存在
	string tname=lstrip(rstrip(statement.substr(13, begin-13), " "), " ");
	if(TableRecord::Table_Exists(tname))
		throw Table_Creation_Error("Already exists table : "+tname);
	optor.table_name=tname;
	vector<string> attrs=Split(statement.substr(begin+1, end-begin-1), ",");//attrs内装有每一列的名称和属性
	for(int i=0; i<attrs.size(); ++i){
		vector<string> temp=Split(attrs[i], " ");
		if(temp.size()>3)//过多项，错误
			throw Table_Creation_Error("Unexpected term : "+temp[2]);
		if(temp.at(0)=="primary"&&temp.at(1)=="key"){//找到主键
			if(optor.primary_key!="")
				throw Table_Creation_Error("Duplicate primary keys : "+optor.primary_key+" and "+temp.at(2));
			int begin=temp.at(2).find('(');
			int end=temp.at(2).find(')');
			if(begin==string::npos||end==string::npos)
				throw Table_Creation_Error("Couldn't find primary key name");
			optor.primary_key=lstrip(rstrip(temp[2].substr(begin+1, end-begin-1), " "), " ");
			continue;
		}
		if(Find_Col_Name(optor.table_attr_name, temp.at(0)))//检查是否已存在列名
			throw Table_Creation_Error("Duplicate column name : "+temp[0]);
		optor.table_attr_name.push_back(temp.at(0));
		if(temp.at(1).substr(0, 4)=="char"){//判断char长度是否符合标准
			int tempint;
			stringstream ss;
			ss<<temp.at(1).substr(5, temp.at(1).size()-6);
			ss>>tempint;
			if(!(tempint>=1&&tempint<=255))
				throw Table_Creation_Error("Unsupported character length : "+temp[1]);
			optor.table_attr_type[temp[0]]=tempint*4+CHARACTERS;
		}
		else if(temp[1]=="int")
			optor.table_attr_type[temp[0]]=INTEGER;
		else if(temp[1]=="float")
			optor.table_attr_type[temp[0]]=FLOATNUM;
		else 
			throw Table_Creation_Error("Unsupported type : "+temp[1]);
		if(temp.size()==3)
			if(temp[2]=="unique")
				optor.table_uniqueness.insert(temp[0]);
			else
				throw Table_Creation_Error("Unexpected term : "+temp[2]);
	}
	if(optor.primary_key=="")
		throw Table_Creation_Error("No primary key found");
	return true;
}

bool Parser::Get_Table_Drop_Info(){
	vector<string> tname=Split(statement, " ");
	if(tname.size()>3)//过多项
		throw Table_Drop_Error("Unexpected term : "+tname[3]);
	else if(tname.size()==2)//无表名
		throw Table_Drop_Error("Table name not found");
	if(TableRecord::Table_Exists(tname[2]))//是否存在该表
		optor.table_name=tname[2];
	else
		throw Table_Drop_Error("No such table : "+tname[2]);
	return true;
}

bool Parser::Get_Index_Create_Info(){
	vector<string> tname=Split(statement, " ");//tname装有分开的每一项
	if(tname.size()>6)//过多项
		throw Index_Creation_Error("Unexpected term : "+tname[6]);
	else if(tname.size()==3)//无索引名
		throw Index_Creation_Error("Index name not found");
	if(IndexRecord::Index_Exists(tname[2]))//是否存在该索引名
		throw Index_Creation_Error("Already exists index : "+tname[2]);
	else
		optor.index_name=tname[2];
	if(TableRecord::Table_Exists(tname[4]))//检查是否存在表
		optor.table_of_index=tname[4];
	else
		throw Index_Creation_Error("No such table : "+tname[4]);
	//检查表内是否存在该列
	int begin=tname.at(5).find('(');
	int end=tname.at(5).find(')');
	if(begin==string::npos||end==string::npos)
		throw Index_Creation_Error("Couldn't find the column on which the index is built");
	string column=tname[5].substr(begin+1, end-begin-1);
	vector<string> columns=TableRecord::Table_Columns_Name(tname[4]);
	int i;
	for (i=0; i<columns.size(); ++i)
		if(columns[i]==column)
			break;
	if(i==columns.size())
		throw Index_Creation_Error("No such column in table "+tname[4]+" : "+column);
	else
		optor.name_of_table_of_index=column;
	return true;
}

bool Parser::Get_Index_Drop_Info(){
	vector<string> tname=Split(statement, " ");
	if(tname.size()>3)//过多项
		throw Index_Drop_Error("Unexpected term : "+tname[3]);
	else if(tname.size()==2)//无索引名
		throw Index_Drop_Error("Table name not found");
	if(IndexRecord::Index_Exists(tname[2])){//是否存在该索引
		optor.index_name=tname[2];
		optor.table_of_index=IndexRecord::Table_Of_Index(optor.index_name);
	}
	else
		throw Index_Drop_Error("No such index : "+tname[2]);
	return true;
}

bool Parser::Get_Table_Select_Info(){
	vector<string> partition1=Split(statement, "from");
	//获得所选择的列
	vector<string> columns=Split(partition1[0].substr(6, partition1.size()-6), ",");
	vector<string> partition2=Split(partition1[1], "where");
	//检查是否存在表
	optor.table_name=lstrip(rstrip(partition2[0], " "), " ");
	if(!TableRecord::Table_Exists(optor.table_name))
		throw Table_Selection_Error("No such table exists : "+optor.table_name);
	if(rstrip(lstrip(columns[0], " "), " ")=="*"){
		optor.select_all=true;
	}
	else{
	optor.select_all=false;
	//找到表的所有列
	vector<string> columns_in_table=TableRecord::Table_Columns_Name(optor.table_name);
	//检查是否所选择的列都在表中出现,并将其加入选择表记录中
	for( int i=0; i<columns.size(); ++i){
		int j;
		string col=lstrip(rstrip(columns[i], " "), " ");
		for(j=0; j<columns_in_table.size(); ++j)
			if(col==columns_in_table[j])
				break;
		if(j==columns_in_table.size())
			throw Table_Selection_Error("No such column in table "+optor.table_name+" : "+col);
		else
			optor.selected_col_of_table.push_back(col);
	}
	}
	//从where子句中找到条件
	if(partition2.size()==2)
		Get_Where_Conditions(partition2[1]);
	else if(partition2.size()>2)
		throw Table_Selection_Error("So many \' where \' conditions");
	return true;
}

bool Parser::Get_Table_Insert_Info(){
	vector<string> partition=Split(statement, "values");
	//找到表名
	optor.table_name=lstrip(rstrip(partition[0].substr(12, partition[0].size()-12), " "), " ");
	if(!TableRecord::Table_Exists(optor.table_name))
		throw Table_Insertion_Error("No such table exists : "+optor.table_name);
	//得到表每列的名称以及类型
	vector<string> col_in_table=TableRecord::Table_Columns_Name(optor.table_name);
	map<string, int> col_type=TableRecord::Table_Columns_Type(optor.table_name);
	int begin=partition[1].find('(');
	int end=partition[1].find(')');
	if(begin==string::npos||end==string::npos)
		throw Table_Insertion_Error("Couldn't find parentheses");
	vector<string> values=Split(partition[1].substr(begin+1, end-begin-1),  ",");
	//判断要插入的值的类型是否与表相符
	if(col_in_table.size()>values.size())
		throw Table_Insertion_Error("Too few values");
	if(col_in_table.size()<values.size())
		throw Table_Insertion_Error("Too many values");
	for(int i=0; i<values.size(); ++i){
		int type=Recoed_And_Get_Type(values[i]);
		int column_type=col_type.at(col_in_table[i]);
		if(type<2&&type!=column_type)
			throw Table_Insertion_Error("Type doesn't match : "+values[i]);
		if(type>2&&column_type<2)
			throw Table_Insertion_Error("Type doesn't match : "+values[i]);
		if(type>2&&column_type<type)
			throw Table_Insertion_Error("Too long string : "+values[i]);
		string value=optor.values.back();
//整数，前补零
		if(type==0){
			int i=value.size();
			while(i>INT_LEN){
				value.pop_back();
				--i;
			}
			while(i<INT_LEN){
				value.insert(value.begin(), '0');
				++i;
			}
		}
//浮点数，前补零
		else if(type==1){
			int i=value.size();
			while(i>FLOAT_LEN){
				value.pop_back();
				--i;
			}
			while(i<FLOAT_LEN){
				value.insert(value.begin(), '0');
				++i;
			}

		}
//字符串，前补空格
		else{
			string col_name=TableRecord::Table_Columns_Name(optor.table_name)[i];
			int char_type=TableRecord::Table_Columns_Type(optor.table_name).at(col_name);
			while(type<char_type){
				value.insert(value.begin(), ' ');
				type+=4;
			}
		}
		optor.values.pop_back();
		optor.values.push_back(value);
	}
	return true;
}

bool Parser::Get_Table_Delete_Info(){
	vector<string> partition=Split(statement, "where");
	if(partition.size()>2)
		throw Table_Deletion_Error("Too many \' where \' clauses");
	optor.table_name=lstrip(rstrip(partition[0].substr(11, partition[0].size()-11), " "), " ");
	if(!TableRecord::Table_Exists(optor.table_name))
		throw Table_Deletion_Error("No such table exists : "+optor.table_name);
	if(partition.size()==2)
		Get_Where_Conditions(partition[1]);
	return true;
}

void Parser::Get_Where_Conditions(const std::string &in_where){
	vector<string> conditions=Split(in_where, "and");
	//col,value分别存列名和值
	string col, value;
	//name和type分别存表中所有列的名字和每列对应的类型
	vector<string> column_name=TableRecord::Table_Columns_Name(optor.table_name);
	map<string, int> column_type=TableRecord::Table_Columns_Type(optor.table_name);
	//opt存条件符
	int opt;
	for(int i=0; i<conditions.size(); ++i){
		int pos=conditions[i].find(">=");
		if(pos!=string::npos){
			col=conditions[i].substr(0, pos);
			value=conditions[i].substr(pos+2, conditions[i].size()-pos-1);
			opt=GREATER_THAN_OR_EQUAL_TO;
		}
		else{
			pos=conditions[i].find("<="); 
			if(pos!=string::npos){
				col=conditions[i].substr(0, pos);
				value=conditions[i].substr(pos+2, conditions[i].size()-pos-1);
				opt=LESS_THAN_OR_EQUAL_TO;
			}
			else{
				pos=conditions[i].find("!=");
				if(pos!=string::npos){
					col=conditions[i].substr(0, pos);
					value=conditions[i].substr(pos+2, conditions[i].size()-pos-1);
					opt=NOT_EQUAL;
				}
				else{
					pos=conditions[i].find("=");
					if(pos!=string::npos){
						col=conditions[i].substr(0, pos);
						value=conditions[i].substr(pos+1, conditions[i].size()-pos);
						opt=EQUAL;
					}
					else{
						pos=conditions[i].find("<");
						if(pos!=string::npos){
							col=conditions[i].substr(0, pos);
							value=conditions[i].substr(pos+1, conditions[i].size()-pos);
							opt=LESS_THAN;
						}
						else{
							pos=conditions[i].find(">");
							if(pos!=string::npos){
								col=conditions[i].substr(0, pos);
								value=conditions[i].substr(pos+1, conditions[i].size()-pos);
								opt=GREATER_THAN;
							}
							else
								throw Table_Selection_Error("Unknown operator : "+conditions[i]);
						}
					}
				}
			}
		}
		optor.condition_in_where.push_back(opt);
//检查表中是否存在该列
		col=rstrip(lstrip(col, " "), " "); value=rstrip(lstrip(value, " "), " "); int j;
		for(j=0; j<column_name.size(); ++j)
			if(column_name[j]==col)
				break;
		if(j==column_name.size())
			throw Table_Selection_Error("No such column exists in table "+optor.table_name+" : "+col);
		optor.col_in_where.push_back(col);
		if(isInt(value)){
			if(column_type.at(col)!=INTEGER)
				throw Table_Selection_Error("Type mismatch at \' "+value+" \'");
			int i=value.size();
			while(i>INT_LEN){
				value.pop_back();
				--i;
			}
			while(i<INT_LEN){
				value.insert(value.begin(), '0');
				++i;
			}
			optor.values.push_back(value);
		}
		else if(isFloat(value)){
			if(column_type.at(col)!=FLOATNUM)
				throw Table_Selection_Error("Type mismatch at \' "+value+" \'");
			int i=value.size();
			while(i>FLOAT_LEN){
				value.pop_back();
				--i;
			}
			while(i<FLOAT_LEN){
				value.insert(value.begin(), '0');
				++i;
			}
			optor.values.push_back(value);
		}
		else if(value[0]=='\''&&value[value.size()-1]=='\''){
			value=value.substr(1, value.size()-2);
			int new_len=value.size();
			int old_len=column_type.at(col)/4;
			if(new_len<=old_len){
				while(new_len<old_len){
					value.insert(value.begin(), ' ');
					++new_len;
				}
				optor.values.push_back(value);
			}
			else
				throw Table_Selection_Error("Too long string : \' "+value+" \'");
		}
		else
			throw Table_Selection_Error("Unknown type : \' "+value+" \'");
	}
}

int Parser::Recoed_And_Get_Type(std::string &in_value){
	string value=lstrip(rstrip(in_value, " "), " ");
	int value_type;
	//获取值的类型
	if(value[0]=='\''&&value[value.size()-1]=='\''){
		value_type=CHARACTERS+((value.size()-2)*4);
		value=value.substr(1, value.size()-2);
	}
	else if(isInt(value))
		value_type=INTEGER;
	else if(isFloat(value))
		value_type=FLOATNUM;
	else
		throw Table_Selection_Error("Unknown data type : "+value);
	optor.values.push_back(value);
	return value_type;
}

bool Parser::isInt(const string &str) const{
	for(int i=0; i<str.size(); ++i)
		if(str[i]>'9'||str[i]<'0')
			return false;
	return true;
}

bool Parser::isFloat(const std::string &str) const{
	for(int i=1; i<str.size(); ++i)
		if(str[i]=='.')
			return true;
	return false;
}

void Operator::Clean(){
	opt=-1;
	table_attr_name.clear();
	table_attr_type.clear();
	table_uniqueness.clear();
	primary_key="";
	table_name="";
	selected_col_of_table.clear();
	col_in_where.clear();
	condition_in_where.clear();
	values.clear();
	return ;
}

void Parser::Execute_File(){
	string fname=statement.substr(9, statement.size()-9);
	infile.open(fname);
	if(infile.good())
		flag=INPUT_FROM_FILE;
	else
		throw Execution_File_Error(fname);
}

bool Parser::Parse(){
	Clean();
	Normalize();
	try{
		Syntax_Check();
#ifdef TIMER
	extern  time_t mystart;
	extern time_t lyfstart;
	extern time_t myend;
	extern time_t lyfend;
	switch(optor.opt){
		case TABLE_SELECT:
		case TABLE_INSERT:
		case TABLE_DELETE:
			mystart=clock();
	}
#endif
	
		switch(optor.opt){
		case TABLE_CREATE: Get_Table_Create_Info(); break;
		case TABLE_DROP: Get_Table_Drop_Info(); break;
		case TABLE_SELECT: Get_Table_Select_Info(); break;
		case TABLE_INSERT: Get_Table_Insert_Info(); break;
		case TABLE_DELETE: Get_Table_Delete_Info(); break;
		case EXEC_FILE: Execute_File(); break;
		case INDEX_CREATE: Get_Index_Create_Info(); break;
		case INDEX_DROP: Get_Index_Drop_Info(); break;
		case QUIT: goon=false; break;
		default: throw Parser_Error();
		}
	}catch (Exception &e){
		e.Print();
		cout<<"\n\n";
		return false;
	}
	return true;
}