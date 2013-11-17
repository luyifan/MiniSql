//
//  Attribute.h
//  MiniSql
//
//  Created by luyifan on 13-10-19.
//  Copyright (c) 2013年 luyifan. All rights reserved.
//

#ifndef __MiniSql__Attribute__
#define __MiniSql__Attribute__
#include <string>
#include "Macro.h"

using namespace std ;

class Attribute
{
public:
    string attributeName ;// 属性名
    AttributeType type ;
    bool isPrimeryKey ;
    bool isUnique ;
    int len ; //这个属性的长度
    Attribute();
    Attribute( string name  , AttributeType Type , int Len , bool Prime , bool Unique ):attributeName(name), type(Type), len(Len), isPrimeryKey(Prime), isUnique(Unique){}
    
};
#endif /* defined(__MiniSql__Attribute__) */
