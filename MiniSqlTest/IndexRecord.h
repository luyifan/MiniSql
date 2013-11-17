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
	static std::vector<std::string> index_name;//������
	static std::map<std::string, std::string>  index_on_table;//�������ڱ���
	static std::map<std::string, std::string> index_on_column;//������������
	static std::map<std::string, int> index_counts;//��Index���ȫ�����뻺��������ô����Ҫ���ٿ�
    static std::map<std::string, std::vector<int>> blockNums ;//ÿһ������������ڻ�������λ�õĿ���
    static std::map<std::string, std::vector<int>> remain ; //����黹�ж�����Ŀ�ǿ��е�
    static std::map<std::string, int> tupleLength ; //ÿһ����Ŀ�Ŀ���

//����������������
	static const std::vector<std::string> &All_Indices(){ return index_name; }
//�����Ƿ���ڸ�����
	static bool Index_Exists(const std::string &iname);
//�����������ڱ���
	static const std::string &Table_Of_Index(const std::string &iname){ return index_on_table.at(iname); }
//����������������
	static const std::string &Column_Of_Index(const std::string &iname){ return index_on_column.at(iname); }
//����һ��������
	static bool CreateIndex(const std::string &iname, const std::string &tname, const std::string &cname);
//ɾ��һ������
	static bool DropIndex(const std::string &iname);
//���ظ�Index���ȫ�����뻺��������ô����Ҫ���ٿ�
	static int Index_Counts(const std::string &iname){ return index_counts.at(iname); }
//���ظ�Indexÿһ������������ڻ�������λ�õĿ���
	static const std::vector<int> &Block_Nums(const std::string &iname){ return blockNums.at(iname); }
//���ظ�Index����黹�ж�����Ŀ�ǿ��е�
	static const std::vector<int> &Remain(const std::string &iname){ return remain.at(iname); }
//���ظ�Indexÿһ����Ŀ�Ŀ���
	static int Tuple_Length(const std::string &iname){ return tupleLength.at(iname); }
//�����ļ�
	static bool Store_Into_Disk();
//���ļ�����
	static bool Read_From_Disk();
};
#endif