#ifndef _PARSER_H_
#define _PARSER_H_

#include "test.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include "Macro.h"
#include "tableforlyf.h"

const int TABLE_CREATE = 0;
const int TABLE_DROP = 1;
const int TABLE_SELECT = 2;
const int TABLE_INSERT = 3;
const int TABLE_DELETE = 4;
const int INDEX_CREATE = 5;
const int INDEX_DROP = 6;
const int EXEC_FILE = 7;
const int QUIT = 8;

const int INPUT_FROM_CONSOLE = 0;
const int INPUT_FROM_FILE = 1;

const int INTEGER = 0;
const int FLOATNUM = 1;
const int CHARACTERS = 2;

const int EQUAL = 0;
const int NOT_EQUAL=1;
const int LESS_THAN=2;
const int LESS_THAN_OR_EQUAL_TO=3;
const int GREATER_THAN=4;
const int GREATER_THAN_OR_EQUAL_TO=5;

class Query{
	std::string query;
public:
	friend std::istream &operator >>(std::istream &in, Query &q);
	friend std::ifstream &operator >>(std::ifstream &infile, Query &q);
	const std::string &getContent() const;
};

class Operator{//所要执行的操作
public:
	int opt;//操作类型
	std::string index_name;//索引名称
	std::string table_name;//表名
	std::vector<std::string> table_attr_name;//列名
	std::map<std::string, int> table_attr_type;//列属性
	std::unordered_set<std::string> table_uniqueness;//列是否唯一
	std::string primary_key;//主键
	std::string table_of_index;//索引所在表名
	std::string name_of_table_of_index;//索引依据列名称
	std::vector<std::string> selected_col_of_table;//选择操作剩余的列
	std::vector<std::string>  col_in_where;//where条件中涉及的列
	std::vector<int> condition_in_where;//where条件中涉及的判断
	std::vector<std::string> values;//where条件判断中涉及的值以及insert中的值
	bool select_all;//选择操作是否选择所有列
	void Clean();//清空
};

class Parser{//输入语句分析器
	std::string statement;
	Operator optor;
	static std::vector<std::string> opts;
public:
	static int flag;
	static std::ifstream infile;
	bool goon;
	static void opts_initial(){
		opts.push_back("create table");
		opts.push_back("drop table");
		opts.push_back("select");
		opts.push_back("insert into");
		opts.push_back("delete from");
		opts.push_back("create index");
		opts.push_back("drop index");
		opts.push_back("execfile");
		opts.push_back("quit");
	}
public:
	Parser(std::string input="");
	~Parser(){}
	Operator &Get_Operator();
	void Input_Statement(std::string input);
	bool Parse();
private:
	void Clean();//清除上次操作
	bool Parse_Statement();//分析语句
	void Normalize();//输入语句规格化
	bool Syntax_Check();//检查语法错误以及确认主操作
	std::vector<std::string> Split(const std::string &input, const std::string &delimit);//字符串分割程序
	bool Find_Col_Name(const std::vector<std::string> &names, const std::string &name);//寻找是否已存在列名
	const std::string lstrip(const std::string &input, const std::string &elim) const;
	const std::string rstrip(const std::string &input, const std::string &elim) const;
	void Get_Where_Conditions(const std::string &in_where);//从where子句中获取条件并存储
	int Recoed_And_Get_Type(std::string &value);//获取要赋的值以及类型
	bool isInt(const std::string &str) const;//判断字符串是不是整数
	bool isFloat(const std::string &str) const;//判断字符串是不是浮点数
	bool Get_Table_Create_Info();//获取创建表所需信息
	bool Get_Table_Drop_Info();//获取销毁表所需信息
	bool Get_Index_Create_Info();//获取创建索引所需信息
	bool Get_Index_Drop_Info();//获取销毁索引所需信息
	bool Get_Table_Insert_Info();//获取插入表所需信息
	bool Get_Table_Delete_Info();//获取删除表所需信息
	bool Get_Table_Select_Info();//获取选择表所需信息
	void Execute_File();//执行批处理文件
};

#endif