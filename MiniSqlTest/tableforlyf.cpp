#include "tableforlyf.h"
#include "TableRecord.h"
#include "IndexRecord.h"
#include "parser.h"
using namespace std;

std::vector<Table> tableforlyf::tableinfo;

const Attribute tableforlyf::myattrtolyfattr(const std::string &tname, const std::string &aname){
	std::map<std::string, int> col_type=TableRecord::Table_Columns_Type(tname);
	std::unordered_set<std::string> col_unique=TableRecord::Table_Uniquenesses(tname);
	bool un=col_unique.find(aname)==col_unique.end()?false:true;
	bool pk=TableRecord::Table_Primary_Key(tname)==aname?true:false;
		
	AttributeType type;
	int len;
	switch(col_type.at(aname)){
		case INTEGER: type=INT; len=INT_LEN;break;
		case FLOATNUM: type=FLOAT; len=FLOAT_LEN;break;
		default: type=CHAR; len=col_type.at(aname)/4;break;
	}

	return Attribute(aname,	type, len, pk, un);
}

const Table tableforlyf::mytabletolyftable(const std::string &tname){
	 std::vector<std::string> col_name=TableRecord::Table_Columns_Name(tname);
	 Table t(tname);
	 int tuplelen=0; int atnum=0;
	 for(int i=0; i<col_name.size(); ++i){
		 ++atnum;
		t.attributes.push_back(myattrtolyfattr(tname, col_name[i])); 
		tuplelen+=t.attributes.back().len;
	 }
	 t.attributeNum=atnum;
	 t.tupleLength=tuplelen+1;
	 vector<string> in=IndexRecord::All_Indices();
	 for(int i=0; i<in.size(); ++i)
		 if(IndexRecord::Table_Of_Index(in[i])==tname)
			 t.index.push_back(in[i]);
	 return t;
}

int tableforlyf::myselectedtolyfselected(const string &tname, const string &cname){
	vector<string> cnames;
	cnames=TableRecord::Table_Columns_Name(tname);
	for(int i=0; i<cnames.size(); ++i)
		if(cnames[i]==cname)
			return i;
}

const constraint tableforlyf::myconstrainttolyfconstraint(const std::string &tname, const std::string &cname, int con, const std::string value){
	vector<string> col_name=TableRecord::Table_Columns_Name(tname);
	int no;
	for(no=0; no<col_name.size(); ++no)
		if(col_name[no]==cname)
			break;
	no+=START_FROM;
	CONSTRAINTTYPE ctype;
	switch(con){
		case EQUAL:ctype=Eq;break;
		case NOT_EQUAL:ctype=Ne;break;
		case LESS_THAN:ctype=Lt; break;
		case LESS_THAN_OR_EQUAL_TO:ctype=Le; break;
		case GREATER_THAN:ctype=Gt; break;
		case GREATER_THAN_OR_EQUAL_TO:ctype=Ge; break;
	}
	constraint temp(no, value);
	temp.conType=ctype;
	return temp;
}

vector<Table>::iterator tableforlyf::Find_Position(const std::string &tname){
	vector<Table>::iterator pos;
	for(pos=tableinfo.begin(); pos<tableinfo.end(); ++pos)
		if((*pos).tableName==tname)
			return pos;
    return pos;
}

int tableforlyf::Find_Position2(const std::string &tname){
	int pos;
	for(pos=0; pos<tableinfo.size(); ++pos)
		if(((tableinfo[pos]).tableName)==tname)
			return pos;
    return pos;
}

bool tableforlyf::Delete_Table(const std::string &tname){
	tableinfo.erase(Find_Position(tname));
	fstream test;
	bool exist;
	test.open(tname+".tableforlyf.info");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((tname+".tableforlyf.info").c_str());
	test.open(tname+".tableforlyf.index");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((tname+".tableforlyf.index").c_str());
	test.open(tname+".tableforlyf.attribute");
	if(test)
		exist=true;
	else
		exist=false;
	test.close();
	if(exist)
		remove((tname+".tableforlyf.attribute").c_str());
	return true;
}

bool tableforlyf::Add_Index(const std::string &tname, const std::string &iname){
	(*Find_Position(tname)).index.push_back(iname);
	return true;
}

bool tableforlyf::Delete_Index(const std::string &tname, const std::string &iname){
	vector<Table>::iterator tpos=Find_Position(tname);
	vector<string>::iterator ipos;
	if(tpos==tableinfo.end())
		return true;
	for(ipos=(*tpos).index.begin(); ipos<(*tpos).index.end(); ++ipos)
		if((*ipos)==iname){
			(*tpos).index.erase(ipos);
			break;
		}
	return true;
}

bool tableforlyf::Store_Into_Disk(){
//	system("cd tableforlyf");
	ofstream tflname(TITLE+"tableforlyfname");

	for(int i=0; i<tableinfo.size(); ++i){

//写表名，counts，attrNum，tupleLen
		tflname<<tableinfo[i].tableName<<'\n'<<tableinfo[i].counts<<'\n'
						<<tableinfo[i].attributeNum<<'\n'<<tableinfo[i].tupleLength<<'\n';

//写bNums，remain
		ofstream info(TITLE+tableinfo[i].tableName+".tableforlyf.info");
		const vector<int> &bnums=tableinfo[i].blockNums;
		const vector< int> &rem=tableinfo[i].remain;
		for(int j=0; j<bnums.size(); ++j)
			info<<bnums[j]<<'\n'<<rem[j]<<'\n';
		info.close();

//写建立在该表上的索引
		ofstream index(TITLE+tableinfo[i].tableName+".tableforlyf.index");
		const vector<string> &iname=tableinfo[i].index;
		for(int j=0; j<iname.size(); ++j)
			index<<iname[j]<<'\n';
		index.close();

//写列属性
		ofstream attr(TITLE+tableinfo[i].tableName+".tableforlyf.attribute");
		const vector<Attribute> &attribute=tableinfo[i].attributes;
		for(int j=0; j<attribute.size(); ++j){
			attr<<attribute[j].attributeName<<'\n';
			switch(attribute[j].type){
				case INT: attr<<0<<'\n'; break;
				case FLOAT: attr<<1<<'\n'; break;
				case CHAR: attr<<2<<'\n'; break;
			}
			attr<<(attribute[j].isPrimeryKey?1:0)<<'\n'<<(attribute[j].isUnique?1:0)<<'\n'<<attribute[j].len<<'\n';
		}
		attr.close();

	}

	tflname.close();
//	system("cd ..");
	return true;
}

bool tableforlyf::Read_From_Disk(){
//	system("cd tableforlyf");
	ifstream tflname(TITLE+"tableforlyfname");

	while(!tflname.eof()){
//读入表名，counts，attrNum，tupleLen
		string tname;
		getline(tflname, tname);
		if(tflname.eof())
			break;
		Table t(tname);
		tflname>>t.counts>>t.attributeNum>>t.tupleLength;
		tflname.ignore(1);//忽略末尾回车

//读入bNums，remain
		ifstream info(TITLE+tname+".tableforlyf.info");
		while(!info.eof()){
			int temp;
			info>>temp;
			if(info.eof())
				break;
			t.blockNums.push_back(temp);
			info>>temp;
			t.remain.push_back(temp);
		}
		info.close();

//读入建立在该表上的索引名
		ifstream index(TITLE+tname+".tableforlyf.index");
		while(!index.eof()){
			string iname;
			getline(index, iname);
			if(index.eof())
				break;
			t.index.push_back(iname);
		}
		index.close();

//读入列属性
		ifstream attribute(TITLE+tname+".tableforlyf.attribute");
		while(!attribute.eof()){
			string temp;
			getline(attribute, temp);
			if(attribute.eof())
				break;
			Attribute attr;
			attr.attributeName=temp;
			int tempint;
			attribute>>tempint;
			switch(tempint){
				case 0: attr.type=INT; break;
				case 1: attr.type=FLOAT; break;
				case 2: attr.type=CHAR; break;
			}
			attribute>>tempint;
			attr.isPrimeryKey=tempint?true:false;
			attribute>>tempint;
			attr.isUnique=tempint?true:false;
			attribute>>attr.len;
			attribute.ignore(1);//忽略末尾回车
			t.attributes.push_back(attr);
		}
		attribute.close();

		tableinfo.push_back(t);
	}

	tflname.close();
//	system("cd ..");
	return true;
}
