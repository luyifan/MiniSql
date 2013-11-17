#include "TableRecord.h"
using namespace std;

std::vector<std::string> TableRecord::table_names;//表名
std::map<std::string, std::vector<std::string>> TableRecord::table_columns_name;//每个表每列的名称
std::map<std::string, std::map<std::string, int>> TableRecord::table_columns_type;//每个表每列的类型
std::map<std::string, std::unordered_set<std::string>> TableRecord::table_uniquenesses;//哪些列名是唯一的
std::map<std::string, std::string> TableRecord::table_primary_keys;

bool TableRecord::Table_Exists(const std::string &tname){
	for(int i=0; i<table_names.size(); ++i)
		if(table_names[i]==tname)
			return true;
	return false;
}

bool TableRecord::Drop_Table(const std::string &tname){
	vector<string>::const_iterator pos;
	for(pos=table_names.begin(); pos<table_names.end(); ++pos)
		if(*pos==tname)
			break;
	table_names.erase(pos);
	table_columns_name.erase(tname);
	table_columns_type.erase(tname);
	table_uniquenesses.erase(tname);
	table_primary_keys.erase(tname);
	return true;
}

bool TableRecord::Create_Table(const std::string &tname, const std::string &pkey,  const std::vector<std::string> &cname, const std::map<std::string, int> &ctype, const std::unordered_set<std::string> &uinfo){
	table_names.push_back(tname);
	table_primary_keys[tname]=pkey;
	table_columns_name[tname]=cname;
	table_columns_type[tname]=ctype;
	table_uniquenesses[tname]=uinfo;
	return true;
}

bool TableRecord::Store_Into_Disk(){
//	system("cd tablerecord");
	ofstream tablename(TITLE+"tablename");

	for(int i=0; i<table_names.size(); ++i){
//写表名和主键
		tablename<<table_names[i]<<'\n'<<table_primary_keys.at(table_names[i])<<'\n';
//写列名和类型
		ofstream info(TITLE+table_names[i]+".tablerecord.column");
		const vector<string> &col_name=table_columns_name.at(table_names[i]);
		const map<string, int> &col_type=table_columns_type.at(table_names[i]);
		for(int j=0; j<col_name.size(); ++j)
			info<<col_name[j]<<'\n'<<col_type.at(col_name[j])<<'\n';
		info.close();
//写唯一的列
		ofstream uni(TITLE+table_names[i]+".tablerecord.unique");
		const unordered_set <string> unique=table_uniquenesses.at(table_names[i]);
		unordered_set<string>::const_iterator ite;
		for(ite=unique.begin(); ite!=unique.end(); ++ite)
			uni<<*ite<<'\n';
		uni.close();
	}

	tablename.close();
//	system("cd ..");
	return true;
}

bool TableRecord::Read_From_Disk(){
//	system("cd tablerecord");
	ifstream tablename(TITLE+"tablename");

	while(!tablename.eof()){

//读入表名和主键
		string tname;
		string temp;
		getline(tablename, tname);
		if(tablename.eof())
			break;
		table_names.push_back(tname);
		getline(tablename, temp);
		table_primary_keys[tname]=temp;

		table_columns_name[tname]=vector<string>();
		table_columns_type[tname]=map<string, int>();
		table_uniquenesses[tname]=unordered_set<string>();

//读入列名和类型
		ifstream column(TITLE+tname+".tablerecord.column");
		while(!column.eof()){
			string cname;
			int type;
			getline(column, cname);
			if(column.eof())
				break;
			column>>type;
			column.ignore(1);//忽略末尾回车
			table_columns_name[tname].push_back(cname);
			table_columns_type[tname][cname]=type;
		}
		column.close();

//读入唯一的列
		ifstream unique(TITLE+tname+".tablerecord.unique");
		while(!unique.eof()){
			string cname;
			getline(unique, cname);
			if(unique.eof())
				break;
			table_uniquenesses[tname].insert(cname);
		}
		unique.close();
	}

	tablename.close();
//	system("cd ..");
	return true;

}
