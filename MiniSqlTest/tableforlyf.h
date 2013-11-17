#ifndef _TABLEFORLYF_H_
#define _TABLEFORLYF_H_
#include <string>
#include "Table.h"
#include <cstdlib>
#include <fstream>
#include "title.h"

const int INT_LEN=10;
const int FLOAT_LEN=15;
const int START_FROM=0;

class tableforlyf{
public:
//表的信息
	static std::vector<Table> tableinfo;
//get single table
	static const Table &Get_Table(const string &tname){
		for(int i=0; i<tableinfo.size(); ++i)
			if(tableinfo[i].tableName==tname)
				return tableinfo[i];
	}
//转换列表示
	static const Attribute myattrtolyfattr(const std::string &tname, const std::string &aname);
//转换表表示
	static const Table mytabletolyftable(const std::string &tname);
//转换约束条件表示
	static const constraint myconstrainttolyfconstraint(const std::string &tname, const std::string &cname, int con, const std::string value);
//转换列的表示
	static int myselectedtolyfselected(const string &tname, const string &cname);
//找到对应表名的表位置
	static std::vector<Table>::iterator Find_Position(const std::string &tname);
	static int Find_Position2(const std::string &tname);
//删除表
	static bool Delete_Table(const std::string &tname);
//为表增加索引
	static bool Add_Index(const std::string &tname, const std::string &iname);
//为表删除索引
	static bool Delete_Index(const std::string &tname, const std::string &iname);
//存入文件
	static bool Store_Into_Disk();
//从文件读入
	static bool Read_From_Disk();

};

#endif