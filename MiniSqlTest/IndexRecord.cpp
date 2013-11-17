#include "IndexRecord.h"
using namespace std;

std::vector<std::string> IndexRecord::index_name;//索引名
std::map<std::string, std::string>  IndexRecord::index_on_table;//索引所在表名
std::map<std::string, std::string> IndexRecord::index_on_column;//索引所在列名
std::map<std::string, int> IndexRecord::index_counts;//该Index如果全部存入缓存区，那么则需要多少块
std::map<std::string, vector<int>> IndexRecord::blockNums ;//每一块如果存在则在缓存区的位置的块编号
std::map<std::string, vector<int>> IndexRecord::remain ; //这个块还有多少条目是空闲的
std::map<std::string, int> IndexRecord::tupleLength ; //每一个条目的块数

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

//写索引名，所在表名，所在列名，要多少块，每一条目块数
		indexname<<index_name[i]<<'\n'<<index_on_table.at(index_name[i])<<'\n'
						<<index_on_column.at(index_name[i])<<'\n'<<index_counts.at(index_name[i])<<'\n'
						<<tupleLength.at(index_name[i])<<'\n';

//写块编号和空闲数
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

//读入索引名，所在表名，所在列名，要多少块，每一条目块数
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
		indexname.ignore(1);//忽略末尾回车
		blockNums[iname]=vector<int>();
		remain[iname]=vector<int>();

//读入块编号和空闲数
		ifstream info1(TITLE+iname+".index.column1");
		while(!info1.eof()){
			int temp;
			info1>>temp;
			if(info1.eof())
				break;
			blockNums[iname].push_back(temp);
			info1.ignore(1);//忽略末尾回车
		}
		info1.close();

		ifstream info2(TITLE+iname+".index.column2");
		while(!info2.eof()){
			int temp;
			info2>>temp;
			if(info2.eof())
				break;
			remain[iname].push_back(temp);
			info2.ignore(1);//忽略末尾回车
		}
		info2.close();
	}

	indexname.close();
//	system("cd ..");
	return true;

}
