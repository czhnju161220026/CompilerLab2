#include "symbol.h"

Field* createField(char* name, ValueTypes type, ...) {
    Field* field = (Field*)malloc(sizeof(Field));
    field->type = type;
    field->next = NULL;
    field->typeName = NULL;
    if(type == _STRUCT_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        field->typeName = va_arg(argp, char*);
    }
    return field;
}

RetValue* createRetValue(ValueTypes type, ...) {
    RetValue* ret = (RetValue*)malloc(sizeof(RetValue));
    ret->type = type;
    ret->typeName = NULL;
    if(type == _STRUCT_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        ret->typeName = va_arg(argp, char*);
    }
    return ret;
}


Argument* createArgument(char* name, ValueTypes type, ...) {
    Argument* arg = (Argument*)malloc(sizeof(Argument));
    arg->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(arg->name, name);
    arg->type = type;
    arg->typeName = NULL;
    if(type == _STRUCT_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        arg->typeName = va_arg(argp, char*);
    }
    return arg;
}


/*
创建符号例子：
1. 创建int型符号： createSymbol("symbol", INT_SYMBOL, 1, 123);
2. 创建float型符号: createSymbol("symbol", FLOAT_SYMBOL, 1, 1.23);
3. 创建array类型的：
    1) 若基类型不是struct，例如是int a[4][5]: createSymbol("a", ARRAY_SYMBOL, 3, _INT_TYPE_, 4, 5);
    2) 基类型是struct，例如struct S s[4][5]: createSymbol("s", ARRAY_SYMBOL, 4, _STRUCT_TYPE_, "S", 4, 5);
4. 创建struct type 类型的：createSymbol("symbol", STRUCT_TYPE_SYMBOL, 2, Field* f1, Field* f2);
5. 创建struct value类型的：createSymbol("symbol", STRUCT_VALUE_SYMBOL, 1, "typeName")
6. 创建func类型的：createSymbol("symbol", FUNC_SYMBOL, 3, RetValue*, Argument* 1, Argument2*);
*/
Symbol* createSymbol(char* name, SymbolTypes symbol_type, int argc, ...) {
    //设置symbol的名称,类型
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    s->next = NULL;
    s->name = (char*) malloc(sizeof(char) * strlen(name));
    s->symbol_type = symbol_type;
    strcpy(s->name, name);
    //处理可选参数
    va_list argp;
    va_start(argp, argc); //读入所有可选参数
    switch(symbol_type){
        case INT_SYMBOL: {
            IntSymbol* content = (IntSymbol*)malloc(sizeof(IntSymbol));
            content->val = va_arg(argp, int);
            s->int_content = content;
            break;
        }
        case FLOAT_SYMBOL: {
            FloatSymbol* content = (FloatSymbol*)malloc(sizeof(FloatSymbol));
            content->val =(float)va_arg(argp, double);
            s->float_content = content;
            break;
        }
        case ARRAY_SYMBOL: {
            ArraySymbol* content = (ArraySymbol*)malloc(sizeof(ArraySymbol));
            content->type = va_arg(argp, ValueTypes);//基类型
            int dimensions = argc - 1;               //维数
            if(content->type == _STRUCT_TYPE_) {     //如果是struct，要设置typeName
                dimensions --;
                char* typeName = va_arg(argp, char*);
                content->typeName = (char*)malloc(sizeof(char) * strlen(typeName));
                strcpy(content->typeName, typeName);
            }
            content->dimensions = dimensions;
            //设置各个维数的长度
            content->size = (int*)malloc(sizeof(int) * dimensions);
            for(int i = 0; i < dimensions; i++) {
                content->size[i] = va_arg(argp, int);
            }
            s->array_content = content;
            break;
        }
        case STRUCT_TYPE_SYMBOL: {
            StructTypeSymbol* content = (StructTypeSymbol*)malloc(sizeof(StructTypeSymbol));
            Field* head = NULL;
            Field* tail = NULL;
            int num_of_fields = argc;
            for(int i = 0; i < num_of_fields; i++) {
                if(head == NULL) {
                    head = va_arg(argp, Field*); 
                    tail = head;
                }
                else {
                    tail->next = va_arg(argp, Field*);
                    tail = tail->next;
                }
            }
            content->fields = head;
            s->struct_def = content;
            break;
        }
        case STRUCT_VAL_SYMBOL: {
            StructValueSymbol* content = (StructValueSymbol*)malloc(sizeof(StructValueSymbol));
            char* typeName = va_arg(argp, char*);
            content->typeName = (char*)malloc(sizeof(char) * strlen(typeName));
            strcpy(content->typeName, typeName);
            s->struct_value = content;
            break;
        }
        case FUNC_SYMBOL: {
            FuncSymbol* content = (FuncSymbol*)malloc(sizeof(FuncSymbol));
            content->ret = va_arg(argp, RetValue*);
            Argument* head = NULL;
            Argument* tail = NULL;
            for(int i = 0; i < argc - 1; i++) {
                if(head == NULL) {
                    head = va_arg(argp, Argument*);
                    tail = head;
                }
                else {
                    tail->next = va_arg(argp, Argument*);
                    tail = tail->next;
                }
            }
            content->arguments = head;
            s->func_content = content;
            break;
        }
        default: printf("Symbol type error"); break;
    }
    return s;
}

void outputSymbol(Symbol* symbol) {

}