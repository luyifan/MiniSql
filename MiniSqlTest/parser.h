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

class Operator{//��Ҫִ�еĲ���
public:
	int opt;//��������
	std::string index_name;//��������
	std::string table_name;//����
	std::vector<std::string> table_attr_name;//����
	std::map<std::string, int> table_attr_type;//������
	std::unordered_set<std::string> table_uniqueness;//���Ƿ�Ψһ
	std::string primary_key;//����
	std::string table_of_index;//�������ڱ���
	std::string name_of_table_of_index;//��������������
	std::vector<std::string> selected_col_of_table;//ѡ�����ʣ�����
	std::vector<std::string>  col_in_where;//where�������漰����
	std::vector<int> condition_in_where;//where�������漰���ж�
	std::vector<std::string> values;//where�����ж����漰��ֵ�Լ�insert�е�ֵ
	bool select_all;//ѡ������Ƿ�ѡ��������
	void Clean();//���
};

class Parser{//������������
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
	void Clean();//����ϴβ���
	bool Parse_Statement();//�������
	void Normalize();//���������
	bool Syntax_Check();//����﷨�����Լ�ȷ��������
	std::vector<std::string> Split(const std::string &input, const std::string &delimit);//�ַ����ָ����
	bool Find_Col_Name(const std::vector<std::string> &names, const std::string &name);//Ѱ���Ƿ��Ѵ�������
	const std::string lstrip(const std::string &input, const std::string &elim) const;
	const std::string rstrip(const std::string &input, const std::string &elim) const;
	void Get_Where_Conditions(const std::string &in_where);//��where�Ӿ��л�ȡ�������洢
	int Recoed_And_Get_Type(std::string &value);//��ȡҪ����ֵ�Լ�����
	bool isInt(const std::string &str) const;//�ж��ַ����ǲ�������
	bool isFloat(const std::string &str) const;//�ж��ַ����ǲ��Ǹ�����
	bool Get_Table_Create_Info();//��ȡ������������Ϣ
	bool Get_Table_Drop_Info();//��ȡ���ٱ�������Ϣ
	bool Get_Index_Create_Info();//��ȡ��������������Ϣ
	bool Get_Index_Drop_Info();//��ȡ��������������Ϣ
	bool Get_Table_Insert_Info();//��ȡ�����������Ϣ
	bool Get_Table_Delete_Info();//��ȡɾ����������Ϣ
	bool Get_Table_Select_Info();//��ȡѡ���������Ϣ
	void Execute_File();//ִ���������ļ�
};

#endif