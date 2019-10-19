#include "symbol.h"
#include "hashset.h"

extern HashSet* symbolTable;

Symbol* createSymbol() {
    //为symbol分配空间
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    if (s == NULL) {
  	printf("When creating symbol, memory allocation error.\n");
        return NULL;
    }
    s->next = NULL;
    s->name = NULL;
    s->symbol_type = 0;
    return s;
}

bool setSymbolName(Symbol* s, char* name) {
    //设置symbol的名称
    if (s == NULL) {
	printf("When setting symbol name, this symbol pointer is NULL.\n");
	return false;
    }
    /*if (isContain(symbolTable, name)) {
	printf("When setting symbol name, this name already exists.\n");
	return false;
    }*/
    if (s->name != NULL) {
	printf("When setting symbol name, this symbol already has a name.\n");
        return false;
    }
    s->name = (char*) malloc(sizeof(char) * strlen(name) + 1);
    if (s->name == NULL) {
	printf("When setting symbol name, memory allocation error.\n");
        return false;
    }
    strcpy(s->name, name);
    return true;
}

bool setSymbolType(Symbol* s, SymbolTypes type) {
    //设置symbol的类型
    if (s == NULL) {
	printf("When setting symbol type, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->symbol_type != 0) {
	printf("When setting symbol(%s) type, this symbol already has a type %d.\n", s->name, s->symbol_type);
        return false;
    }
    s->symbol_type = type;
    switch (s->symbol_type) {
        case INT_SYMBOL: s->int_content = (IntContent*) malloc(sizeof(IntContent)); break;
        case FLOAT_SYMBOL: s->float_content = (FloatContent*) malloc(sizeof(FloatContent)); break;
        case FUNC_SYMBOL: s->func_content = (FuncContent*) malloc(sizeof(FuncContent)); 
            s->func_content->retType = 0; s->func_content->arguments = NULL;s->func_content->typeName = NULL;
            break;
        case ARRAY_SYMBOL: s->array_content = (ArrayContent*) malloc(sizeof(ArrayContent)); 
            s->array_content->type = 0; s->array_content->dimensions = 0; s->array_content->typeName = NULL; s->array_content->size = NULL;
            break;
        case STRUCT_TYPE_SYMBOL: s->struct_def = (StructTypeContent*) malloc(sizeof(StructTypeContent)); 
            s->struct_def->fields = NULL;
            break;
        case STRUCT_VAL_SYMBOL: s->struct_value = (StructValueContent*) malloc(sizeof(StructValueContent)); 
            s->struct_value->typeName = NULL;
            break;
    }
    return true;
}

bool addArrayDimension(Symbol* s, int size) {
    if (s == NULL) {
	printf("When adding array dimension, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->array_content == NULL) {
	printf("When sadding array dimension, this symbol content pointer is NULL.\n");
	return false;
    }
    if (s->array_content->dimensions == 0 && s->array_content->size == NULL) {
	s->array_content->dimensions++;
	int* temp = (int*) malloc(sizeof(int) * s->array_content->dimensions);
	temp[s->array_content->dimensions-1] = size;
	s->array_content->size = temp;
	return true;
    } else if (s->array_content->dimensions > 0 && s->array_content->size != NULL){
	s->array_content->dimensions++;
  	int* temp = (int*) malloc(sizeof(int) * s->array_content->dimensions);
	temp[0] = size;
	memcpy(&temp[1], s->array_content->size, s->array_content->dimensions * sizeof(int) - sizeof(int));
	free(s->array_content->size);
	s->array_content->size = temp;
	return true;
    } else {
	printf("When adding array dimension, dimension doesn't match with size.\n");
	return false;
    }
}

bool setArrayType(Symbol* s, ValueTypes type, char* name) {
    if (s == NULL) {
	printf("When setting array type, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->array_content == NULL) {
	printf("When setting array type, this symbol content pointer is NULL.\n");
	return false;
    }
    if (type != _STRUCT_TYPE_) {
	s->array_content->type = type;
    } else {
	s->array_content->type = type;
        Symbol* struct_def = get(symbolTable, name);
	if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
	    printf("When setting array type, this type doesn't exist.\n");
	    return false;
	}
	s->array_content->typeName = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(s->array_content->typeName, name);
    }
    return true;
}

bool setFuncReturnValue(Symbol* s, ValueTypes type, char* name) {
    if (s == NULL) {
	printf("When setting func return type, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->func_content == NULL) {
	printf("When setting func return type, this symbol content pointer is NULL.\n");
	return false;
    }
    if (type != _STRUCT_TYPE_) {
	s->func_content->retType = type;
    } else {
	s->func_content->retType = type;
	Symbol* struct_def = get(symbolTable, name);
	if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
	    printf("When setting func return value, this type doesn't exist.\n");
	    return false;
	}
	s->func_content->typeName = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(s->func_content->typeName, name);
    }
    return true;
}

bool addFuncArgument(Symbol* s, char* name) {
    if (s == NULL) {
	printf("When adding func argument, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->func_content == NULL) {
	printf("When adding func argument, this symbol content pointer is NULL.\n");
	return false;
    }
    if (!isContain(symbolTable, name)) {
	printf("When adding func argument, this argument doesn't exist.\n");
	return false;
    }
    if (s->func_content->arguments == NULL) {
	s->func_content->arguments = (Argument*) malloc(sizeof(Argument));
	s->func_content->arguments->name = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(s->func_content->arguments->name,name);
	s->func_content->arguments->next = NULL;
    } else {
	Argument* p = s->func_content->arguments;
	while (p->next != NULL) {
	    p = p->next;
	}
        Argument* temp = (Argument*) malloc(sizeof(Argument));
	temp->name = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(temp->name,name);
	temp->next = NULL;
	p->next = temp;
    }
    return true;
}

bool setStructValueType(Symbol* s, char* name) {
    if (s == NULL) {
	printf("When setting struct value type, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->struct_value == NULL) {
	printf("When setting struct value type, this symbol content pointer is NULL.\n");
	return false;
    }
    Symbol* struct_def = get(symbolTable, name);
    if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
	printf("When setting struct value, this type doesn't exist.\n");
	return false;
    }
    s->struct_value->typeName = (char*) malloc(sizeof(char) * strlen(name) + 1);
    strcpy(s->struct_value->typeName,name);
    return true;
}

bool addStructTypeField(Symbol* s, char* name) {
    if (s == NULL) {
	printf("When adding struct type field, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->struct_value == NULL) {
	printf("When adding struct type field, this symbol content pointer is NULL.\n");
	return false;
    }
    if (!isContain(symbolTable, name)) {
	printf("When adding struct type field, this field doesn't exist.\n");
	return false;
    }
    if (s->struct_def->fields == NULL) {
	s->struct_def->fields = (Field*) malloc(sizeof(Field));
	s->struct_def->fields->name = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(s->struct_def->fields->name,name);
	s->struct_def->fields->next = NULL;
    } else {
	Field* p = s->struct_def->fields;
	while (p->next != NULL) {
	    p = p->next;
	}
        Field* temp = (Field*) malloc(sizeof(Field));
	temp->name = (char*) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(temp->name,name);
	temp->next = NULL;
	p->next = temp;
    }
    return true;
}

char* valueTypesToString(ValueTypes type) {
    switch (type) {
	case _INT_TYPE_: return "INT";
      	case _FLOAT_TYPE_: return "FLOAT";
	case _ARRAY_TYPE_: return "ARRAY";
	case _STRUCT_TYPE_: return "STRUCT";
        default: return "NONE";
    }
}


bool outputSymbol(Symbol* s) {
    printf("----------------------------------\n");
    if (s == NULL) {
	printf("When outputting symbol, this symbol pointer is NULL.\n");
	return false;
    }
    if (s->name != NULL) {
	printf("Symbol name: %s\n", s->name);
    } else {
	printf("Symbol name is NULL\n");
	return false;
    }

    
    if (s->symbol_type == INT_SYMBOL) {
	printf("Symbol type: INT\n");
    } else if (s->symbol_type == FLOAT_SYMBOL) {
	printf("Symbol type: FLOAT\n");
    } else if (s->symbol_type == FUNC_SYMBOL) {
	printf("Symbol type: FUNC\n");
	if (s->func_content->retType == _STRUCT_TYPE_) {
	    printf("Return Type: %s %s\n", valueTypesToString(s->func_content->retType), s->func_content->typeName);
        } else {
	    printf("Return Type: %s\n", valueTypesToString(s->func_content->retType));
        }

    } else if (s->symbol_type == ARRAY_SYMBOL) {
	printf("Symbol type: ARRAY\n");
        if (s->array_content->type == _STRUCT_TYPE_) {
	    printf("Element Type: %s %s\n", valueTypesToString(s->array_content->type), s->array_content->typeName);
        } else {
	    printf("Element Type: %s\n", valueTypesToString(s->array_content->type));
        }
        printf("Array dimensions: \n");
        for (int i = 0; i < s->array_content->dimensions; i++) {
	    printf("%d ", s->array_content->size[i]);
        }
	printf("\n");
    } else if (s->symbol_type == STRUCT_TYPE_SYMBOL) {
        printf("Symbol type: STRUCT_TYPE\n");
        printf("Struct fields: \n");
        Field* f = s->struct_def->fields;
        while (f != NULL) {
            printf("\t%s\n", f->name);
            f= f->next;
        }
    } else if (s->symbol_type == STRUCT_VAL_SYMBOL) {
        printf("Symbol type: STRUCT_VAL\n");
        printf("Struct type: %s\n", s->struct_value->typeName);
    } else {
	printf("wrong type.\n"); 
	return false;
    }
    return true; 
}
