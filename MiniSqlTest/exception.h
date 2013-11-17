#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <iostream>

class Exception{

public:
	virtual void Print() const{
		std::cout<<"An error as follow had occurred :\n";	
	}
};

class Parser_Error: public Exception{
public:
	void Print() const{
		Exception::Print();
		std::cout<<"Unknown instruction"<<std::endl;
	}
};

class Syntax_Error: public Exception{
public:
	void Print() const{ 
		Exception::Print();
		std::cout << "Syntax Error"<<std::endl; 
	}
};

class Table_Creation_Error: public Exception{
	std::string reason;
public:
	Table_Creation_Error(const std::string &r):reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Execution_File_Error: public Exception{
	std::string fname;
public:
	Execution_File_Error(const std::string &f): fname(f){}
	void Print() const{
		Exception::Print();
		std::cout<<"Can't execute file \'"+fname+"\'"<<std::endl;
	}
};

class Table_Drop_Error: public Exception{
	std::string reason;
public:
	Table_Drop_Error(const std::string &r): reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Index_Creation_Error: public Exception{
	std::string reason;
public:
	Index_Creation_Error(const std::string &r):reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Index_Drop_Error: public Exception{
	std::string reason;
public:
	Index_Drop_Error(const std::string &r): reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Table_Selection_Error: public Exception{
	std::string reason;
public:
	Table_Selection_Error(const std::string &r):reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Table_Insertion_Error: public Exception{
	std::string reason;
public:
	Table_Insertion_Error(const std::string &r):reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

class Table_Deletion_Error: public Exception{
	std::string reason;
public:
	Table_Deletion_Error(const std::string &r):reason(r){}
	void Print() const{
		Exception::Print();
		std::cout<<reason<<std::endl;
	}
};

#endif