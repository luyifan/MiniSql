#ifndef _TABLERECORD_H_
#define _TABLERECORD_H_
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <cstdlib>
#include <fstream>
#include "title.h"

class TableRecord{
	static std::vector<std::string> table_names;//表名
	static std::map<std::string, std::vector<std::string>> table_columns_name;//每个表每列的名称
	static std::map<std::string, std::map<std::string, int>> table_columns_type;//每个表每列的类型
	static std::map<std::string, std::unordered_set<std::string>> table_uniquenesses;//哪些列名是唯一的
	static std::map<std::string, std::string> table_primary_keys;
public:
	//是否存在表
	static bool Table_Exists(const std::string &tname);
	//返回一个表的所有列名
	static const std::vector<std::string> &Table_Columns_Name(const std::string &tname) { return table_columns_name.at(tname); }
	//返回一个表每列的类型
	static const std::map<std::string, int> &Table_Columns_Type(const std::string &tname) { return table_columns_type.at(tname); }
	//返回一个表每列是否唯一
	static const std::unordered_set<std::string> &Table_Uniquenesses(const std::string &tname) { return table_uniquenesses.at(tname); }
	//返回一个表的主键
	static const std::string &Table_Primary_Key(const std::string &tname){ return table_primary_keys.at(tname); }
	//销毁一个表
	static bool Drop_Table(const std::string &tname);
	//新创建一条表记录
	static bool Create_Table(const std::string &tname, const std::string &pkey,  const std::vector<std::string> &cname, const std::map<std::string, int> &ctype, const std::unordered_set<std::string> &uinfo);
	//存入文件
	static bool Store_Into_Disk();
	//从文件读入
	static bool Read_From_Disk();
};

#endif