#ifndef __SYMBOL_H__
#define __SYMBOL_H__
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//定义一个符号的类型
typedef enum SymbolTypes {
    INT_SYMBOL,
    FLOAT_SYMBOL,
    ARRAY_SYMBOL,
    STRUCT_TYPE_SYMBOL, //该符号是一个struct定义
    STRUCT_VAL_SYMBOL, //该符号是一个struct实例
    FUNC_SYMBOL
} SymbolTypes;

//定义值可能的类型
typedef enum ValueTypes {
    _INT_TYPE_,
    _FLOAT_TYPE_,
    _VOID_TYPE_,
    _STRUCT_TYPE_,
    _ARRAY_TYPE_,
    //_POINTER_TYPE_, 没有定义指针运算符，不需要指针类型
} ValueTypes;

//INT_SYMBOL的内容
typedef struct IntContent {
    int val;
} IntContent;

//FLOAT_SYMBOL的内容
typedef struct FloatContent {
    float val;
} FloatContent;

//ARRAY_SYMBOL的内容
typedef struct ArrayContent {
    ValueTypes type; //基类型 
    int dimensions; //维数
    int * size; //每一维的大小
    char * typeName; //如果基类型是一个struct，那么这个字段指明struct的名字
} ArrayContent;

//FUNC_SYMBOL的内容
typedef struct RetValue {
    ValueTypes type; //返回值类型
    char * typeName; //如果返回值类型是一个struct，那么这个字段指明struct的名字
    ArrayContent* arrayContent; //如果返回值是一个array，那么这个字段指明array的信息（基类型，维数，每维长度)
} RetValue;

typedef struct Argument {
    ValueTypes type; //参数类型
    char* typeName; //如果参数是一个struct，那么typeName指明它的名称
    ArrayContent* arrayContent; //如果参数是一个array，那么这个字段指明array的信息（基类型，维数，每维长度)
    char* name;      //参数名
    struct Argument* next; //下一个参数
    
} Argument;

typedef struct FuncContent {
    RetValue* ret;       //返回值
    Argument* arguments; //参数列表
} FuncContent;

//STRUCT_DEF_SYMBOL的内容
typedef struct Field {
    ValueTypes type; //类型
    char* name;//参数名
    struct Field* next; //下一个域
    char* typeName; //如果域是一个struct， 那么这个字段指明struct的名字
    ArrayContent* arrayContent; //如果域是一个array，那么这个字段指明array的信息（基类型，维数，每维长度)
} Field;

typedef struct StructTypeContent {
    Field* fields;
} StructTypeContent;

//STRUCT_VAL_SYMBOL的内容
typedef struct StructValueContent {
    char* typeName; //指明这个value的类型是哪个struct
} StructValueContent;

/*符号的定义
* 1. 符号名
* 2. 符号类型
* 3. 符号内容*/
typedef struct Symbol {
    char* name;
    SymbolTypes symbol_type;
    union {
        IntContent* int_content;
        FloatContent* float_content;
        FuncContent* func_content;
        ArrayContent* array_content;
        StructTypeContent* struct_def;
        StructValueContent* struct_value;
    };
    struct Symbol* next;
} Symbol;

ArrayContent* createArrayContent(ValueTypes type, int argc, ...);
Field* createField(char* name, ValueTypes type,...);
RetValue* createRetValue(ValueTypes type,...);
Argument* createArgument(char* name, ValueTypes type, ...);
Symbol* createSymbol(char* name, SymbolTypes symbol_type, int argc, ...);

void outputSymbol(Symbol* symbol);
#endif