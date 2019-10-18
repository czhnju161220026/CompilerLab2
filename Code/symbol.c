#include "symbol.h"
//创建一个数组信息 
//基类型 + argc + 类型名（当类型为struct时需要) + 每维长度
ArrayContent* createArrayContent(ValueTypes type, int argc, ...) {
    ArrayContent* arrayContent = (ArrayContent*)malloc(sizeof(ArrayContent));
    arrayContent->type = type;
    int dimensions = argc;
    va_list argp;
    va_start(argp, argc);
    arrayContent->typeName = NULL;
    if(type == _STRUCT_TYPE_) {
        arrayContent->typeName = va_arg(argp, char*);
        dimensions--;
    }
    arrayContent->size = (int*)malloc(dimensions * sizeof(int));
    for(int i = 0; i < dimensions; i++) {
        arrayContent->size[i] = va_arg(argp, int);
    }
    return arrayContent;
}
//创建结构体中某个域的信息
//field名 + 类型 + 类型名（当类型为struct时需要) / 数组信息(当类型是array时需要)
Field* createField(char* name, ValueTypes type, ...) {
    Field* field = (Field*)malloc(sizeof(Field));
    field->type = type;
    field->next = NULL;
    field->typeName = NULL;
    field->arrayContent = NULL;
    field->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(field->name, name);
    //如果是结构体
    if(type == _STRUCT_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        field->typeName = va_arg(argp, char*);
    }
    //如果是数组
    if(type == _ARRAY_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        field->arrayContent = va_arg(argp, ArrayContent*);
    }
    return field;
}
//创建函数返回值的信息
//类型 + 类型名（当类型为struct时需要）/ 数组信息(当类型是array时需要)
RetValue* createRetValue(ValueTypes type, ...) {
    RetValue* ret = (RetValue*)malloc(sizeof(RetValue));
    ret->type = type;
    ret->typeName = NULL;
    if(type == _STRUCT_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        ret->typeName = va_arg(argp, char*);
    }
    //如果是数组
    if(type == _ARRAY_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        ret->arrayContent = va_arg(argp, ArrayContent*);
    }
    return ret;
}

//创建函数参数的信息
//参数名 + 类型 + 类型名（当类型为struct时需要）/ 数组信息(当类型是array时需要)
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
    if(type == _ARRAY_TYPE_) {
        va_list argp;
        va_start(argp, 1);
        arg->arrayContent = va_arg(argp, ArrayContent*);
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
            IntContent* content = (IntContent*)malloc(sizeof(IntContent));
            content->val = va_arg(argp, int);
            s->int_content = content;
            break;
        }
        case FLOAT_SYMBOL: {
            FloatContent* content = (FloatContent*)malloc(sizeof(FloatContent));
            content->val =(float)va_arg(argp, double);
            s->float_content = content;
            break;
        }
        case ARRAY_SYMBOL: {
            ArrayContent* content = (ArrayContent*)malloc(sizeof(ArrayContent));
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
            StructTypeContent* content = (StructTypeContent*)malloc(sizeof(StructTypeContent));
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
            StructValueContent* content = (StructValueContent*)malloc(sizeof(StructValueContent));
            char* typeName = va_arg(argp, char*);
            content->typeName = (char*)malloc(sizeof(char) * strlen(typeName));
            strcpy(content->typeName, typeName);
            s->struct_value = content;
            break;
        }
        case FUNC_SYMBOL: {
            FuncContent* content = (FuncContent*)malloc(sizeof(FuncContent));
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
    if(symbol == NULL) {
        printf("Null symbol\n");
    }
    else {
        printf("Symbol name : %s \n", symbol->name);
        switch(symbol->symbol_type) {
            case INT_SYMBOL:  printf("Symbol type: int\n");printf("Symbol val: %d\n", symbol->int_content->val); break;
            case FLOAT_SYMBOL :printf("Symbol type: float\n"); printf("Symbol val: %f\n", symbol->float_content->val); break;
            case ARRAY_SYMBOL: {
                printf("Symbol type: array\n");
                ArrayContent* content = symbol->array_content;
                switch(content->type) {
                    case _INT_TYPE_: printf("Array basic type : int\n");break;
                    case _FLOAT_TYPE_: printf("Array basic type : float\n"); break;
                    case _STRUCT_TYPE_: printf("Array basic type: struct %s.\n", content->typeName);
                    case _ARRAY_TYPE_:  printf("array "); break;
                }
                printf("Array shape: [");
                for(int i = 0;i < content->dimensions; i++) {
                    printf("%d, ", content->size[i]);
                }
                printf("]\n");
                break;
            }
            case STRUCT_TYPE_SYMBOL: {
                printf("Symbol type: struct type\n");
                Field* p = symbol->struct_def->fields;
                printf("Symbol fields:[");
                while(p != NULL) {
                    switch(p->type) {
                        case _INT_TYPE_: printf("int ");break;
                        case _FLOAT_TYPE_: printf("float "); break;
                        case _STRUCT_TYPE_: printf("struct %s ", p->typeName); break;
                        case _ARRAY_TYPE_:  printf("array "); break;
                    }
                    printf("%s, ", p->name);
                    p = p->next;
                }
                printf("]\n");
                break;
            }
            case STRUCT_VAL_SYMBOL: {
                printf("Symbol type: struct value\n");
                printf("Type: %s\n", symbol->struct_value->typeName);
                break;
            }
            case FUNC_SYMBOL: {
                printf("Symbol type: function\n");
                RetValue* ret = symbol->func_content->ret;
                Argument* p = symbol->func_content->arguments;
                printf("Ret value : ");
                switch(ret->type) {
                    case _VOID_TYPE_: printf("void\n"); break;
                    case _INT_TYPE_: printf("int\n");break;
                    case _FLOAT_TYPE_: printf("float\n"); break;
                    case _STRUCT_TYPE_: printf("struct %s\n", ret->typeName);
                    case _ARRAY_TYPE_:  printf("array "); break;
                }
                printf("Arguments:[");
                while(p!=NULL) {
                    switch(p->type) {
                        case _INT_TYPE_: printf("int ");break;
                        case _FLOAT_TYPE_: printf("float "); break;
                        case _STRUCT_TYPE_: printf("struct %s ", p->typeName); break;
                        case _ARRAY_TYPE_:  printf("array "); break;
                    }
                    printf("%s, ", p->name);
                    p = p->next;
                }
                printf("]\n");
                break;
            }
        }
    }
    printf("-------------------------------------------\n");
}

