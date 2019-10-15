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
    //_POINTER_TYPE_, 没有定义指针运算符，不需要指针类型
} ValueTypes;

//INT_SYMBOL的内容
typedef struct IntSymbol {
    int val;
} IntSymbol;

//FLOAT_SYMBOL的内容
typedef struct FloatSymbol {
    float val;
} FloatSymbol;

//ARRAY_SYMBOL的内容
typedef struct ArraySymbol {
    ValueTypes type; //基类型 
    int dimensions; //维数
    int * size; //每一维的大小
    char * typeName; //如果基类型是一个struct，那么这个字段指明struct的名字
} ArraySymbol;

//FUNC_SYMBOL的内容
typedef struct RetValue {
    ValueTypes type; //返回值类型
    char * typeName; //如果返回值类型是一个struct，那么这个字段指明struct的名字
} RetValue;

typedef struct Argument {
    ValueTypes type; //参数类型
    char* typeName; //如果参数是一个struct，那么typeName指明它的名称
    char* name;      //参数名
    struct Argument* next; //下一个参数
    
} Argument;

typedef struct FuncSymbol {
    RetValue* ret;       //返回值
    Argument* arguments; //参数列表
} FuncSymbol;

//STRUCT_DEF_SYMBOL的内容
typedef struct Field {
    ValueTypes type; //类型
    char* name;//参数名
    struct Field* next; //下一个域
    char* typeName; //如果域是一个struct， 那么这个字段指明struct的名字
} Field;

typedef struct StructTypeSymbol {
    Field* fields;
} StructTypeSymbol;

//STRUCT_VAL_SYMBOL的内容
typedef struct StructValueSymbol {
    char* typeName; //指明这个value的类型是哪个struct
} StructValueSymbol;

/*符号的定义
* 1. 符号名
* 2. 符号类型
* 3. 符号内容*/
typedef struct Symbol {
    char* name;
    SymbolTypes symbol_type;
    union {
        IntSymbol* int_content;
        FloatSymbol* float_content;
        FuncSymbol* func_content;
        ArraySymbol* array_content;
        StructTypeSymbol* struct_def;
        StructValueSymbol* struct_value;
    };
    struct Symbol* next;
} Symbol;

Field* createField(char* name, ValueTypes type,...);
RetValue* createRetValue(ValueTypes type,...);
Argument* createArgument(char* name, ValueTypes type, ...);
Symbol* createSymbol(char* name, SymbolTypes symbol_type, int argc, ...);

void outputSymbol(Symbol* symbol);
#endif