#ifndef _INDEXRECORD_H_
#define _INDEXRECORD_H_
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstdlib>
#include "title.h"

class IndexRecord{
public:
	static std::vector<std::string> index_name;//索引名
	static std::map<std::string, std::string>  index_on_table;//索引所在表名
	static std::map<std::string, std::string> index_on_column;//索引所在列名
	static std::map<std::string, int> index_counts;//该Index如果全部存入缓存区，那么则需要多少块
    static std::map<std::string, std::vector<int>> blockNums ;//每一块如果存在则在缓存区的位置的块编号
    static std::map<std::string, std::vector<int>> remain ; //这个块还有多少条目是空闲的
    static std::map<std::string, int> tupleLength ; //每一个条目的块数

//返回所有索引名称
	static const std::vector<std::string> &All_Indices(){ return index_name; }
//返回是否存在该索引
	static bool Index_Exists(const std::string &iname);
//返回索引所在表名
	static const std::string &Table_Of_Index(const std::string &iname){ return index_on_table.at(iname); }
//返回索引所在列名
	static const std::string &Column_Of_Index(const std::string &iname){ return index_on_column.at(iname); }
//加入一个新索引
	static bool CreateIndex(const std::string &iname, const std::string &tname, const std::string &cname);
//删除一个索引
	static bool DropIndex(const std::string &iname);
//返回该Index如果全部存入缓存区，那么则需要多少块
	static int Index_Counts(const std::string &iname){ return index_counts.at(iname); }
//返回该Index每一块如果存在则在缓存区的位置的块编号
	static const std::vector<int> &Block_Nums(const std::string &iname){ return blockNums.at(iname); }
//返回该Index这个块还有多少条目是空闲的
	static const std::vector<int> &Remain(const std::string &iname){ return remain.at(iname); }
//返回该Index每一个条目的块数
	static int Tuple_Length(const std::string &iname){ return tupleLength.at(iname); }
//存入文件
	static bool Store_Into_Disk();
//从文件读入
	static bool Read_From_Disk();
};
#endif