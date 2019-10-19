#include "semantic.h"
#include "grammarTree.h"
int anonymous = 0;
extern HashSet* symbolTable;
void printTotalGrammarTree(Morpheme* root, int depth) {
    if (root == NULL) {
        return;
    }
    
    for (int k = 0; k < depth; k++) {
        printf("  ");
    }
    if (root->type >= 28) {
            printf("%s (%d)\n", typeToString(root->type), root->lineNumber);
    } else if (root->type <= 27 && root->type >= 1) {
        if (root->type == _ID) {
            printf("%s: %s\n", typeToString(root->type), root->idName);
        } else if (root->type == _INT) {
            printf("%s: %d\n", typeToString(root->type), root->intValue);
        } else if (root->type == _FLOAT) {
            printf("%s: %f\n", typeToString(root->type), root->floatValue);
        } else if (root->type == _TYPE) {
            printf("%s: %s\n", typeToString(root->type), root->idName);
        } else {
            printf("%s\n", typeToString(root->type));
        }
    } else {
	printf("%s\n", typeToString(root->type));
    }
    
    
    Morpheme* c = root->child;
    while (c != NULL) {
        printTotalGrammarTree(c, depth+1);
        c = c->siblings;
    }
       
}


/****handlers****/

bool handleProgram(Morpheme* root) {
    if (root == NULL) {
	printf("\033[31mwhen handling Program, this ExtDefList is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _Program) {
	printf("\033[31mwhen handling Program, this node is not ExtDefList.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling Program.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling program, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _ExtDefList) {
	return handleExtDefList(c);
    } else {
	printf("\033[31mwhen handling Program, this Program has a wrong child .\n\033[0m");
	return false;
    }
    return true;
}


bool handleExtDefList(Morpheme* root) {
    if (root == NULL) {
	printf("\033[31mwhen handling ExtDefList, this ExtDefList is NULL.\n\033[0m");
	return false;
    }

    if (root->type != _ExtDefList) {
	printf("\033[31mwhen handling ExtDefList, this node is not ExtDefList.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling ExtDefList.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _BLANK) {
 	printf("\033[32mwhen handling ExtDefList, this ExtDefList is empty .\n\033[0m");
	return true;
    }
    while (c != NULL) {
        if (c->type == _ExtDefList) {
	    handleExtDefList(c);
	} else if (c->type == _ExtDef) {
	    handleExtDef(c);
	} else {
	    printf("\033[31mwhen handling ExtDefList, this ExtDefList has a wrong child .\n\033[0m");
	    return false;
  	}
        c = c->siblings;
    }
    return true;
}

bool handleExtDef(Morpheme* root) {
    if (root == NULL) {
	printf("\033[31mwhen handling ExtDef, this ExtDef is NULL.\n\033[0m");
	return false;
    }

    if (root->type != _ExtDef) {
	printf("\033[31mwhen handling ExtDef, this node is not ExtDef.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling ExtDef.\n\033[0m");

    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _SEMI) {
	//ExtDef := Specifier SEMI
	ValueTypes* type = (ValueTypes*) malloc(sizeof(ValueTypes));
        char** name = (char**) malloc(sizeof(char*));
        handleSpecifier(c, type, name);
        return true;
    } else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _ExtDecList && c->siblings->siblings != NULL
            && c->siblings->siblings->type == _SEMI) {
        //ExtDef := Specifier ExtDecList SEMI
	printf("ExtDef := Specifier ExtDecList SEMI\n");
        ValueTypes* type = (ValueTypes*) malloc(sizeof(ValueTypes));
        char** name = (char**) malloc(sizeof(char*));
        handleSpecifier(c, type, name);
        return handleExtDecList(c->siblings, type, name);
    } else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _FunDec && c->siblings->siblings != NULL
            && c->siblings->siblings->type == _CompSt) {
        //ExtDef := Specifier FunDec CompSt
	printf("ExtDef := Specifier FunDec CompSt\n");
    } else {
	printf("\033[31mwhen handling ExtDef, this ExtDef has a wrong child .\n\033[0m");
	return false;
    }

    return true;
}

bool handleSpecifier(Morpheme* root, ValueTypes* type, char** name) {
    if (root == NULL) {
	printf("when handling Specifier, this Specifier is NULL.\n");
	return false;
    }
    if (root->type != _Specifier) {
	printf("when handling Specifier, this node is not Specifier.\n");
	return false;
    }
    printf("\033[32mStart handling Specifier.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("when handling Specifier, child node is NULL .\n");
	return false;
    }
    if (c->type == _TYPE) {
	return handleTYPE(c, type);
    } else if (c->type == _StructSpecifier) {
	//Specifier := StructSpecifier
	return handleStructSpecifier(c, type, name);
    } else {
        printf("when handling Specifier, this Specifier has a wrong child .\n");
	return false;
    }
    return true;
}

bool handleStructSpecifier(Morpheme* root, ValueTypes* type, char** name) {
    if (root == NULL) {
	printf("when handling StructSpecifier, this StructSpecifier is NULL.\n");
	return false;
    }
    if (root->type != _StructSpecifier) {
	printf("when handling StructSpecifier, this node is not StructSpecifier.\n");
	return false;
    }
    printf("\033[32mStart handling StructSpecifier.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("when handling StructSpecifier, child node is NULL .\n");
	return false;
    }
    if (c->type == _STRUCT && c->siblings != NULL && c->siblings->type == _OptTag && c->siblings->siblings != NULL
        && c->siblings->siblings->type == _LC && c->siblings->siblings->siblings != NULL  
        && c->siblings->siblings->siblings->type == _DefList && c->siblings->siblings->siblings->siblings != NULL
        && c->siblings->siblings->siblings->siblings->type == _RC) {
	//StructSpecifier := STRUCT OptTag LC DefList RC
        *type = _STRUCT_TYPE_;
        handleOptTag(c->siblings, name);
        Symbol* s = createSymbol();
        setSymbolName(s, *name); // TODO handle epsilon
        setSymbolType(s, STRUCT_TYPE_SYMBOL);
        insert(symbolTable, s);
        return handleDefList(c->siblings->siblings->siblings, s);
    } else if (c->type == _STRUCT && c->siblings != NULL && c->siblings->type == _Tag) {
        //StructSpecifier := STRUCT Tag
	*type = _STRUCT_TYPE_;
        return handleTag(c->siblings, name);
    } else {
        printf("when handling StructSpecifier, this Specifier has a wrong child .\n");
	return false;
    }
    return true;
}

bool handleOptTag(Morpheme* root, char** name) {
    if (root == NULL) {
	printf("\033[31mwhen handling OptTag, this OptTag is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _OptTag) {
	printf("\033[31mwhen handling OtTag, this node is not OptTag, this is %d.\n\033[0m", root->type);
	return false;
    }
    printf("\033[32mStart handling OptTag.\n\033[0m");
    
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling OptTag, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _ID) {
    	if (c->idName != NULL) {
	    *name = (char*) malloc(strlen(c->idName) + 1);
	    strcpy(*name, c->idName);
            return true;
    	} else {
    	    printf("\033[31mwhen handling OptTag, the idName is NULL.\n\033[0m");
	    return false;
   	 }
    } else if (c->type == _BLANK) {
        char str[80];
        sprintf(str, "$%d", anonymous);
        anonymous++;
        *name = (char*) malloc(strlen(str) + 1);
	strcpy(*name, str);
        return true;
    }else {
        printf("\033[31mwhen handling OptTag, child node is wrong .\n\033[0m");
	return false;
    }

    return true;
}

bool handleTag(Morpheme* root, char** name) {
    if (root == NULL) {
	printf("\033[31mwhen handling Tag, this Tag is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _Tag) {
	printf("\033[31mwhen handling Tag, this node is not Tag.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling Tag.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling Tag, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _ID) {
    	if (c->idName != NULL) {
	    *name = (char*) malloc(strlen(c->idName) + 1);
	    strcpy(*name, c->idName);
            return true;
    	} else {
    	    printf("\033[31mwhen handling Tag, the idName is NULL.\n\033[0m");
	    return false;
   	 }
    } else {
        printf("\033[31mwhen handling Tag, child node is wrong .\n\033[0m");
	return false;
    }

    return true;

}

bool handleTYPE(Morpheme* root, ValueTypes* type) {
    if (root == NULL) {
	printf("when handling TYPE, this TYPE is NULL.\n");
	return false;
    }
    if (root->type != _TYPE) {
	printf("when handling TYPE, this node is not TYPE.\n");
	return false;
    }
    printf("\033[32mStart handling TYPE.\n\033[0m");
    if (root->idName != NULL) {
	if (strcmp("int", root->idName) == 0) {
	    *type = _INT_TYPE_;
	} else if (strcmp("float", root->idName) == 0) {
	    *type = _FLOAT_TYPE_;
	} else {
	    printf("when handling TYPE, the type name is wrong.\n");
	    return false;
	}
    } else {
    	printf("when handling TYPE, the idName is NULL.\n");
	return false;
    }

    return true;
}

bool handleVarDec(Morpheme* root, Symbol* s) {
    if (root == NULL) {
	printf("\033[31mwhen handling VarDec, this VarDec is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _VarDec) {
	printf("\033[31mwhen handling VarDec, this node is not VarDec.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling VarDec.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling VarDec, child node is NULL .\n\033[0m");
	return false;
    }

    if (c->type == _ID) {
        setSymbolName(s, c->idName);
        return true;
    } else if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _LB && c->siblings->siblings != NULL
                && c->siblings->siblings->type == _INT && c->siblings->siblings->siblings != NULL 
                && c->siblings->siblings->siblings->type == _RB) {
        if (s->symbol_type == 0) {
            setSymbolType(s, ARRAY_SYMBOL);
        } 
        addArrayDimension(s, c->siblings->siblings->intValue);
        return handleVarDec(c, s);
    } else {
        printf("\033[31mwhen handling VarDec, this VarDec has a wrong child .\n\033[0m");
	return false;
    }

    return true;
}

bool handleExtDecList(Morpheme* root, ValueTypes* type, char** name) {
    if (root == NULL) {
	printf("\033[31mwhen handling ExtDecList, this ExtDecList is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _ExtDecList) {
	printf("\033[31mwhen handling ExtDecList, this node is not ExtDecList.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling ExtDecList.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling ExtDecList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL
        && c->siblings->siblings->type == _ExtDecList) {
        Symbol* s = createSymbol();
        handleVarDec(c, s);
	if (s->symbol_type == ARRAY_SYMBOL) {
            setArrayType(s, *type, *name);
	    insert(symbolTable, s);
        } else {
	    if (*type == _INT_TYPE_) {
                setSymbolType(s, INT_SYMBOL);
                insert(symbolTable, s);
            } else if (*type == _FLOAT_TYPE_) {
                setSymbolType(s, FLOAT_SYMBOL);
                insert(symbolTable, s);
            } else if (*type == _STRUCT_TYPE_) {
                setSymbolType(s, STRUCT_VAL_SYMBOL);
                setStructValueType(s, *name);
                insert(symbolTable, s);
            }
        }
        return handleExtDecList(c->siblings->siblings, type, name);
    } else if (c->type == _VarDec) {
	Symbol* s = createSymbol();
        handleVarDec(c, s);
	if (s->symbol_type == ARRAY_SYMBOL) {
            setArrayType(s, *type, *name);
	    insert(symbolTable, s);
        } else {
	    if (*type == _INT_TYPE_) {
                setSymbolType(s, INT_SYMBOL);
                insert(symbolTable, s);
            } else if (*type == _FLOAT_TYPE_) {
                setSymbolType(s, FLOAT_SYMBOL);
                insert(symbolTable, s);
            } else if (*type == _STRUCT_TYPE_) {
                setSymbolType(s, STRUCT_VAL_SYMBOL);
                setStructValueType(s, *name);
                insert(symbolTable, s);
            }
        }
        return true;
    } else {
	printf("\033[31mwhen handling ExtDecList, this ExtDecList has a wrong child .\n\033[0m");
	return false;
    }
    return true;
}

bool handleDefList(Morpheme* root, Symbol* s) {
    if (root == NULL) {
	printf("\033[31mwhen handling DefList, this DefList is NULL.\n\033[0m");
	return false;
    }

    if (root->type != _DefList) {
	printf("\033[31mwhen handling DefList, this node is not DefList.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling DefList.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _BLANK) {
 	printf("\033[32mwhen handling DefList, this DefList is empty .\n\033[0m");
	return true;
    }
    while (c != NULL) {
        if (c->type == _DefList) {
	    handleDefList(c, s);
	} else if (c->type == _Def) {
	    handleDef(c, s);
	} else {
	    printf("\033[31mwhen handling DefList, this DefList has a wrong child .\n\033[0m");
	    return false;
  	}
        c = c->siblings;
    }
    return true;
}

bool handleDef(Morpheme* root, Symbol* s) {
    if (root == NULL) {
	printf("\033[31mwhen handling Def, this Def is NULL.\n\033[0m");
	return false;
    }

    if (root->type != _Def) {
	printf("\033[31mwhen handling Def, this node is not Def.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling Def.\n\033[0m");

    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling DefList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _DecList && c->siblings->siblings != NULL
            && c->siblings->siblings->type == _SEMI) {
        //Def := Specifier DecList SEMI
	printf("Def := Specifier DecList SEMI\n");
        ValueTypes* type = (ValueTypes*) malloc(sizeof(ValueTypes));
        char** name = (char**) malloc(sizeof(char*));
        handleSpecifier(c, type, name);
        return handleDecList(c->siblings, s, type, name);
    } else {
	printf("\033[31mwhen handling Def, this Def has a wrong child .\n\033[0m");
	return false;
    }

    return true;
}

bool handleDecList(Morpheme* root, Symbol* s, ValueTypes* type, char** name) {
    if (root == NULL) {
	printf("\033[31mwhen handling DecList, this DecList is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _DecList) {
	printf("\033[31mwhen handling DecList, this node is not DecList.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling DecList.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling DecList, child node is NULL .\n\033[0m");
	return false;
    }
    if (c->type == _Dec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL
        && c->siblings->siblings->type == _DecList) {
        Symbol* ss = createSymbol();
        handleDec(c, ss);
	if (ss->symbol_type == ARRAY_SYMBOL) {
            setArrayType(ss, *type, *name);
	    insert(symbolTable, ss);
        } else {
	    if (*type == _INT_TYPE_) {
                setSymbolType(ss, INT_SYMBOL);
                insert(symbolTable, ss);
            } else if (*type == _FLOAT_TYPE_) {
                setSymbolType(ss, FLOAT_SYMBOL);
                insert(symbolTable, ss);
            } else if (*type == _STRUCT_TYPE_) {
                setSymbolType(ss, STRUCT_VAL_SYMBOL);
                setStructValueType(ss, *name);
                insert(symbolTable, ss);
            }
        }
        if (s->symbol_type == STRUCT_TYPE_SYMBOL) {
	    addStructTypeField(s, ss->name);
        }
        return handleDecList(c->siblings->siblings, s, type, name);
    } else if (c->type == _Dec) {
	Symbol* ss = createSymbol();
        handleDec(c, ss);
	if (ss->symbol_type == ARRAY_SYMBOL) {
            setArrayType(ss, *type, *name);
	    insert(symbolTable, ss);
        } else {
	    if (*type == _INT_TYPE_) {
                setSymbolType(ss, INT_SYMBOL);
                insert(symbolTable, ss);
            } else if (*type == _FLOAT_TYPE_) {
                setSymbolType(ss, FLOAT_SYMBOL);
                insert(symbolTable, ss);
            } else if (*type == _STRUCT_TYPE_) {
                setSymbolType(ss, STRUCT_VAL_SYMBOL);
                setStructValueType(ss, *name);
                insert(symbolTable, ss);
            }
        }
        if (s->symbol_type == STRUCT_TYPE_SYMBOL) {
	    addStructTypeField(s, ss->name);
        }
        return true;
    } else {
	printf("\033[31mwhen handling ExtDecList, this ExtDecList has a wrong child .\n\033[0m");
	return false;
    }
    return true;
}

bool handleDec(Morpheme* root, Symbol* s) {
    if (root == NULL) {
	printf("\033[31mwhen handling Dec, this Dec is NULL.\n\033[0m");
	return false;
    }
    if (root->type != _Dec) {
	printf("\033[31mwhen handling Dec, this node is not Dec.\n\033[0m");
	return false;
    }
    printf("\033[32mStart handling Dec.\n\033[0m");
    Morpheme* c = root->child;
    if (c == NULL) {
 	printf("\033[31mwhen handling Dec, child node is NULL .\n\033[0m");
	return false;
    }

    if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL 
        && c->siblings->siblings->type == _Exp) {
        return handleVarDec(c, s);
    } else if (c->type == _VarDec) {
        return handleVarDec(c, s);
    } else {
        printf("\033[31mwhen handling Dec, this Dec has a wrong child .\n\033[0m");
	return false;
    }

    return true;
}

