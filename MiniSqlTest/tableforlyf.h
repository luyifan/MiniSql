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
//�����Ϣ
	static std::vector<Table> tableinfo;
//get single table
	static const Table &Get_Table(const string &tname){
		for(int i=0; i<tableinfo.size(); ++i)
			if(tableinfo[i].tableName==tname)
				return tableinfo[i];
	}
//ת���б�ʾ
	static const Attribute myattrtolyfattr(const std::string &tname, const std::string &aname);
//ת�����ʾ
	static const Table mytabletolyftable(const std::string &tname);
//ת��Լ��������ʾ
	static const constraint myconstrainttolyfconstraint(const std::string &tname, const std::string &cname, int con, const std::string value);
//ת���еı�ʾ
	static int myselectedtolyfselected(const string &tname, const string &cname);
//�ҵ���Ӧ�����ı�λ��
	static std::vector<Table>::iterator Find_Position(const std::string &tname);
	static int Find_Position2(const std::string &tname);
//ɾ����
	static bool Delete_Table(const std::string &tname);
//Ϊ����������
	static bool Add_Index(const std::string &tname, const std::string &iname);
//Ϊ��ɾ������
	static bool Delete_Index(const std::string &tname, const std::string &iname);
//�����ļ�
	static bool Store_Into_Disk();
//���ļ�����
	static bool Read_From_Disk();

};

#endif