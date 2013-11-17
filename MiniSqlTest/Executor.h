#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_
#include "parser.h"
#include "exception.h"
#include "test.h"

const int PLACE_HOLDER=2;
class Executor{
public:
	static bool Select(const Operator &o);
	static bool Delete(const Operator &o);
	static bool Insert(const Operator &o);
	static bool DropTable(Operator &o);
	static bool CreateTable(const Operator &o);
	static bool DropIndex(const Operator &o);
	static bool CreateIndex(const Operator &o);
	static bool Select_By_Tree(const Operator &o, std::string &bcol, std::string &bvalue);
	static bool Delete_By_Tree(const Operator &o);
	static bool Print_Select_Result(const std::vector<std::string> &opt, const std::string &tname, const Data &result);
	static bool Print_Row(const std::vector<std::string> &values, const std::vector<int> &length, const std::vector<std::string> &selected_col_name, const std::string &tname);
	static string Print_Value(int type, const string &value);
	static bool Select_By_Tree_Insert(const string &tname, const string &colname, string &Iname);
};

#endif