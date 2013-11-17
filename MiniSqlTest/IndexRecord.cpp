#include "IndexRecord.h"
using namespace std;

std::vector<std::string> IndexRecord::index_name;//������
std::map<std::string, std::string>  IndexRecord::index_on_table;//�������ڱ���
std::map<std::string, std::string> IndexRecord::index_on_column;//������������
std::map<std::string, int> IndexRecord::index_counts;//��Index���ȫ�����뻺��������ô����Ҫ���ٿ�
std::map<std::string, vector<int>> IndexRecord::blockNums ;//ÿһ������������ڻ�������λ�õĿ���
std::map<std::string, vector<int>> IndexRecord::remain ; //����黹�ж�����Ŀ�ǿ��е�
std::map<std::string, int> IndexRecord::tupleLength ; //ÿһ����Ŀ�Ŀ���

bool IndexRecord::Index_Exists(const std::string &iname){
	for(int i=0; i<index_name.size(); ++i)
		if(index_name[i]==iname)
			return true;
	return false;
}

bool IndexRecord::CreateIndex(const std::string &iname, const std::string &tname, const std::string &cname){
	index_name.push_back(iname);
	index_on_table[iname]=tname;
	index_on_column[iname]=cname;
	index_counts[iname]=0;
	blockNums[iname]=vector<int>();
	remain[iname]=vector<int>();
	tupleLength[iname]=0;
	return true;
}

bool IndexRecord::DropIndex(const std::string &iname){
	vector<string>::iterator pos;
	for(pos=index_name.begin(); pos<index_name.end(); ++pos)
		if(*pos==iname)
			break;
	index_name.erase(pos);
	index_on_table.erase(iname);
	index_on_column.erase(iname);
	index_counts.erase(iname);
	blockNums.erase(iname);
	remain.erase(iname);
	tupleLength.erase(iname);
	return true;
}

bool IndexRecord::Store_Into_Disk(){
//	system("cd indexrecord");
	ofstream indexname(TITLE+"indexname");

	for(int i=0; i<index_name.size(); ++i){

//д�����������ڱ���������������Ҫ���ٿ飬ÿһ��Ŀ����
		indexname<<index_name[i]<<'\n'<<index_on_table.at(index_name[i])<<'\n'
						<<index_on_column.at(index_name[i])<<'\n'<<index_counts.at(index_name[i])<<'\n'
						<<tupleLength.at(index_name[i])<<'\n';

//д���źͿ�����
		ofstream info1(TITLE+index_name[i]+".index.column1");
		const vector<int> &bnums=blockNums.at(index_name[i]);
		for(int j=0; j<bnums.size(); ++j)
			info1<<bnums[j]<<'\n';
		info1.close();
		ofstream info2(TITLE+index_name[i]+".index.column2");
		const vector< int> &rem=remain.at(index_name[i]);
		for(int j=0; j<rem.size(); ++j)
			info2<<rem[j]<<'\n';
		info2.close();
	}
	indexname.close();
//	system("cd ..");
	return true;
}

bool IndexRecord::Read_From_Disk(){
//	system("cd indexrecord");
	ifstream indexname(TITLE+"indexname");

	while(!indexname.eof()){

//���������������ڱ���������������Ҫ���ٿ飬ÿһ��Ŀ����
		string iname;
		string temp;
		int tempint;
		getline(indexname, iname);
		if(indexname.eof())
			break;
		index_name.push_back(iname);
		getline(indexname, temp);
		index_on_table[iname]=temp;
		getline(indexname, temp);
		index_on_column[iname]=temp;
		indexname>>tempint;
		index_counts[iname]=tempint;
		indexname>>tempint;
		tupleLength[iname]=tempint;
		indexname.ignore(1);//����ĩβ�س�
		blockNums[iname]=vector<int>();
		remain[iname]=vector<int>();

//������źͿ�����
		ifstream info1(TITLE+iname+".index.column1");
		while(!info1.eof()){
			int temp;
			info1>>temp;
			if(info1.eof())
				break;
			blockNums[iname].push_back(temp);
			info1.ignore(1);//����ĩβ�س�
		}
		info1.close();

		ifstream info2(TITLE+iname+".index.column2");
		while(!info2.eof()){
			int temp;
			info2>>temp;
			if(info2.eof())
				break;
			remain[iname].push_back(temp);
			info2.ignore(1);//����ĩβ�س�
		}
		info2.close();
	}

	indexname.close();
//	system("cd ..");
	return true;

}
