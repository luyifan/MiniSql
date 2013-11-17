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
	static std::vector<std::string> table_names;//����
	static std::map<std::string, std::vector<std::string>> table_columns_name;//ÿ����ÿ�е�����
	static std::map<std::string, std::map<std::string, int>> table_columns_type;//ÿ����ÿ�е�����
	static std::map<std::string, std::unordered_set<std::string>> table_uniquenesses;//��Щ������Ψһ��
	static std::map<std::string, std::string> table_primary_keys;
public:
	//�Ƿ���ڱ�
	static bool Table_Exists(const std::string &tname);
	//����һ�������������
	static const std::vector<std::string> &Table_Columns_Name(const std::string &tname) { return table_columns_name.at(tname); }
	//����һ����ÿ�е�����
	static const std::map<std::string, int> &Table_Columns_Type(const std::string &tname) { return table_columns_type.at(tname); }
	//����һ����ÿ���Ƿ�Ψһ
	static const std::unordered_set<std::string> &Table_Uniquenesses(const std::string &tname) { return table_uniquenesses.at(tname); }
	//����һ���������
	static const std::string &Table_Primary_Key(const std::string &tname){ return table_primary_keys.at(tname); }
	//����һ����
	static bool Drop_Table(const std::string &tname);
	//�´���һ�����¼
	static bool Create_Table(const std::string &tname, const std::string &pkey,  const std::vector<std::string> &cname, const std::map<std::string, int> &ctype, const std::unordered_set<std::string> &uinfo);
	//�����ļ�
	static bool Store_Into_Disk();
	//���ļ�����
	static bool Read_From_Disk();
};

#endif