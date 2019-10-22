#include "semantic.h"
#include "grammarTree.h"
#include "log.h"
int anonymous = 0;
extern HashSet *symbolTable;
void printTotalGrammarTree(Morpheme *root, int depth)
{
    if (root == NULL)
    {
        return;
    }

    for (int k = 0; k < depth; k++)
    {
        printf("  ");
    }
    if (root->type >= 28)
    {
        printf("%s (%d)\n", typeToString(root->type), root->lineNumber);
    }
    else if (root->type <= 27 && root->type >= 1)
    {
        if (root->type == _ID)
        {
            printf(typeToString(root->type), root->idName);
        }
        else if (root->type == _INT)
        {
            printf(typeToString(root->type), root->intValue);
        }
        else if (root->type == _FLOAT)
        {
            printf("%s: %f\n", typeToString(root->type), root->floatValue);
        }
        else if (root->type == _TYPE)
        {
            printf("%s: %s\n", typeToString(root->type), root->idName);
        }
        else
        {
            printf("%s\n", typeToString(root->type));
        }
    }
    else
    {
        printf("%s\n", typeToString(root->type));
    }

    Morpheme *c = root->child;
    while (c != NULL)
    {
        printTotalGrammarTree(c, depth + 1);
        c = c->siblings;
    }
}

/****handlers****/

bool handleProgram(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Program, this ExtDefList is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _Program)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Program, this node is not ExtDefList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Program.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling program, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _ExtDefList)
    {
        return handleExtDefList(c);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Program, this Program has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleExtDefList(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, this ExtDefList is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _ExtDefList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, this node is not ExtDefList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling ExtDefList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _BLANK)
    {
        addLogInfo(SemanticAnalysisLog, "\033[32mwhen handling ExtDefList, this ExtDefList is empty .\n\033[0m");
        return true;
    }
    while (c != NULL)
    {
        if (c->type == _ExtDefList)
        {
            handleExtDefList(c);
        }
        else if (c->type == _ExtDef)
        {
            handleExtDef(c);
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, this ExtDefList has a wrong child .\n\033[0m");
        }
        c = c->siblings;
    }
    return true;
}

bool handleExtDef(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDef, this ExtDef is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _ExtDef)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDef, this node is not ExtDef.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling ExtDef.\n\033[0m");

    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _SEMI)
    {
        //ExtDef := Specifier SEMI
        ValueTypes *type = (ValueTypes *)malloc(sizeof(ValueTypes));
        char **name = (char **)malloc(sizeof(char *));
        handleSpecifier(c, type, name);
        return true;
    }
    else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _ExtDecList && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI)
    {
        //ExtDef := Specifier ExtDecList SEMI
        ValueTypes *type = (ValueTypes *)malloc(sizeof(ValueTypes));
        char **name = (char **)malloc(sizeof(char *));
        handleSpecifier(c, type, name);
        return handleExtDecList(c->siblings, type, name);
    }
    else if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _FunDec && c->siblings->siblings != NULL && c->siblings->siblings->type == _CompSt)
    {
        //ExtDef := Specifier FunDec CompSt
        ValueTypes *type = (ValueTypes *)malloc(sizeof(ValueTypes));
        char **name = (char **)malloc(sizeof(char *));
        handleSpecifier(c, type, name);
        Symbol *s = createSymbol();
        setSymbolType(s, FUNC_SYMBOL);
        setFuncReturnValue(s, *type, *name);
        handleFunDec(c->siblings, s);
        if (!insert(symbolTable, s))
        {
            reportError(SemanticError, 4, c->siblings->lineNumber, "Duplicate definition of function.");
        }
        return handleCompSt(c->siblings->siblings);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDef, this ExtDef has a wrong child .\n\033[0m");
        return false;
    }

    return true;
}

bool handleSpecifier(Morpheme *root, ValueTypes *type, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Specifier, this Specifier is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _Specifier)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Specifier, this node is not Specifier.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Specifier.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Specifier, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _TYPE)
    {
        return handleTYPE(c, type);
    }
    else if (c->type == _StructSpecifier)
    {
        //Specifier := StructSpecifier
        return handleStructSpecifier(c, type, name);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Specifier, this Specifier has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleStructSpecifier(Morpheme *root, ValueTypes *type, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StructSpecifier, this StructSpecifier is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _StructSpecifier)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StructSpecifier, this node is not StructSpecifier.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling StructSpecifier.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StructSpecifier, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _STRUCT && c->siblings != NULL && c->siblings->type == _OptTag && c->siblings->siblings != NULL && c->siblings->siblings->type == _LC && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _DefList && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _RC)
    {
        //StructSpecifier := STRUCT OptTag LC DefList RC
        *type = _STRUCT_TYPE_;
        handleOptTag(c->siblings, name);
        Symbol *s = createSymbol();
        setSymbolName(s, *name); // TODO handle epsilon
        setSymbolType(s, STRUCT_TYPE_SYMBOL);
        if (!insert(symbolTable, s))
        {
            reportError(SemanticError, 16, c->lineNumber, "This struct type name has been used");
        }
        return handleDefList(c->siblings->siblings->siblings, s);
    }
    else if (c->type == _STRUCT && c->siblings != NULL && c->siblings->type == _Tag)
    {
        //StructSpecifier := STRUCT Tag
        *type = _STRUCT_TYPE_;
        return handleTag(c->siblings, name);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StructSpecifier, this Specifier has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleOptTag(Morpheme *root, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling OptTag, this OptTag is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _OptTag)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling OtTag, this node is not OptTag.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling OptTag.\n\033[0m");

    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling OptTag, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _ID)
    {
        if (c->idName != NULL)
        {
            *name = (char *)malloc(strlen(c->idName) + 1);
            strcpy(*name, c->idName);
            return true;
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling OptTag, the idName is NULL.\n\033[0m");
            return false;
        }
    }
    else if (c->type == _BLANK)
    {
        char str[80];
        sprintf(str, "$%d", anonymous);
        anonymous++;
        *name = (char *)malloc(strlen(str) + 1);
        strcpy(*name, str);
        return true;
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling OptTag, child node is wrong .\n\033[0m");
        return false;
    }

    return true;
}

bool handleTag(Morpheme *root, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Tag, this Tag is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _Tag)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Tag, this node is not Tag.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Tag.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Tag, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _ID)
    {
        if (c->idName != NULL)
        {
            *name = (char *)malloc(strlen(c->idName) + 1);
            strcpy(*name, c->idName);
            if (!isContain(symbolTable, c->idName))
            {
                reportError(SemanticError, 17, c->lineNumber, "This struct type is undefined");
            }
            else if (get(symbolTable, c->idName)->symbol_type != STRUCT_TYPE_SYMBOL)
            {
                reportError(SemanticError, 17, c->lineNumber, "This struct type is undefined");
            }
            return true;
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Tag, the idName is NULL.\n\033[0m");
            return false;
        }
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Tag, child node is wrong .\n\033[0m");
        return false;
    }

    return true;
}

bool handleTYPE(Morpheme *root, ValueTypes *type)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling TYPE, this TYPE is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _TYPE)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling TYPE, this node is not TYPE.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling TYPE.\n\033[0m");
    if (root->idName != NULL)
    {
        if (strcmp("int", root->idName) == 0)
        {
            *type = _INT_TYPE_;
        }
        else if (strcmp("float", root->idName) == 0)
        {
            *type = _FLOAT_TYPE_;
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling TYPE, the type name is wrong.\n\033[0m");
            return false;
        }
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling TYPE, the idName is NULL.\n\033[0m");
        return false;
    }

    return true;
}

bool handleVarDec(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarDec, this VarDec is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _VarDec)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarDec, this node is not VarDec.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling VarDec.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarDec, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _ID)
    {
        setSymbolName(s, c->idName);
        return true;
    }
    else if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _LB && c->siblings->siblings != NULL && c->siblings->siblings->type == _INT && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RB)
    {
        if (s->symbol_type == 0)
        {
            setSymbolType(s, ARRAY_SYMBOL);
        }
        addArrayDimension(s, c->siblings->siblings->intValue);
        return handleVarDec(c, s);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarDec, this VarDec has a wrong child .\n\033[0m");
        return false;
    }

    return true;
}

bool handleExtDecList(Morpheme *root, ValueTypes *type, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, this ExtDecList is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _ExtDecList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, this node is not ExtDecList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling ExtDecList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _ExtDecList)
    {
        Symbol *s = createSymbol();
        handleVarDec(c, s);
        if (s->symbol_type == ARRAY_SYMBOL)
        {
            setArrayType(s, *type, *name);
            //insert(symbolTable, s);
        }
        else
        {
            if (*type == _INT_TYPE_)
            {
                setSymbolType(s, INT_SYMBOL);
                //insert(symbolTable, s);
            }
            else if (*type == _FLOAT_TYPE_)
            {
                setSymbolType(s, FLOAT_SYMBOL);
                //insert(symbolTable, s);
            }
            else if (*type == _STRUCT_TYPE_)
            {
                setSymbolType(s, STRUCT_VAL_SYMBOL);
                setStructValueType(s, *name);
                //insert(symbolTable, s);
            }
            else
            {
                addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, VarDec type is wrong .\n\033[0m");
                return false;
            }
        }
        if (!insert(symbolTable, s))
        {
            reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
        }
        return handleExtDecList(c->siblings->siblings, type, name);
    }
    else if (c->type == _VarDec)
    {
        Symbol *s = createSymbol();
        handleVarDec(c, s);
        if (s->symbol_type == ARRAY_SYMBOL)
        {
            setArrayType(s, *type, *name);
            insert(symbolTable, s);
        }
        else
        {
            if (*type == _INT_TYPE_)
            {
                setSymbolType(s, INT_SYMBOL);
                insert(symbolTable, s);
            }
            else if (*type == _FLOAT_TYPE_)
            {
                setSymbolType(s, FLOAT_SYMBOL);
                insert(symbolTable, s);
            }
            else if (*type == _STRUCT_TYPE_)
            {
                setSymbolType(s, STRUCT_VAL_SYMBOL);
                setStructValueType(s, *name);
                insert(symbolTable, s);
            }
        }
        return true;
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, this ExtDecList has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleDefList(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DefList, this DefList is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _DefList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DefList, this node is not DefList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling DefList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDefList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _BLANK)
    {
        addLogInfo(SemanticAnalysisLog, "\033[32mwhen handling DefList, this DefList is empty .\n\033[0m");
        return true;
    }
    while (c != NULL)
    {
        if (c->type == _DefList)
        {
            handleDefList(c, s);
        }
        else if (c->type == _Def)
        {
            handleDef(c, s);
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DefList, this DefList has a wrong child .\n\033[0m");
        }
        c = c->siblings;
    }
    return true;
}

bool handleDef(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Def, this Def is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _Def)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Def, this node is not Def.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Def.\n\033[0m");

    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DefList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _DecList && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI)
    {
        //Def := Specifier DecList SEMI
        ValueTypes *type = (ValueTypes *)malloc(sizeof(ValueTypes));
        char **name = (char **)malloc(sizeof(char *));
        handleSpecifier(c, type, name);
        return handleDecList(c->siblings, s, type, name);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Def, this Def has a wrong child .\n\033[0m");
        return false;
    }

    return true;
}

bool handleDecList(Morpheme *root, Symbol *s, ValueTypes *type, char **name)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DecList, this DecList is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _DecList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DecList, this node is not DecList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling DecList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling DecList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _Dec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _DecList)
    {
        Symbol *ss = createSymbol();
        handleDec(c, s, ss);
        if (ss->symbol_type == ARRAY_SYMBOL)
        {
            setArrayType(ss, *type, *name);
            //insert(symbolTable, ss);
        }
        else
        {
            if (*type == _INT_TYPE_)
            {
                setSymbolType(ss, INT_SYMBOL);
                //insert(symbolTable, ss);
            }
            else if (*type == _FLOAT_TYPE_)
            {
                setSymbolType(ss, FLOAT_SYMBOL);
                //insert(symbolTable, ss);
            }
            else if (*type == _STRUCT_TYPE_)
            {
                setSymbolType(ss, STRUCT_VAL_SYMBOL);
                setStructValueType(ss, *name);
                //insert(symbolTable, ss);
            }
        }
        if (s != NULL && s->symbol_type == STRUCT_TYPE_SYMBOL)
        {
            if (insert(symbolTable, ss))
            {
                addStructTypeField(s, ss->name);
            }
            else
            {
                reportError(SemanticError, 15, c->lineNumber, "Wrong field in struct definition.");
            }
        }
        else
        {
            if (!insert(symbolTable, ss))
            {
                reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
            }
        }
        return handleDecList(c->siblings->siblings, s, type, name);
    }
    else if (c->type == _Dec)
    {
        Symbol *ss = createSymbol();
        handleDec(c, s, ss);
        if (ss->symbol_type == ARRAY_SYMBOL)
        {
            setArrayType(ss, *type, *name);
            //insert(symbolTable, ss);
        }
        else
        {
            if (*type == _INT_TYPE_)
            {
                setSymbolType(ss, INT_SYMBOL);
                //insert(symbolTable, ss);
            }
            else if (*type == _FLOAT_TYPE_)
            {
                setSymbolType(ss, FLOAT_SYMBOL);
                //insert(symbolTable, ss);
            }
            else if (*type == _STRUCT_TYPE_)
            {
                setSymbolType(ss, STRUCT_VAL_SYMBOL);
                setStructValueType(ss, *name);
                //insert(symbolTable, ss);
            }
        }
        if (s != NULL && s->symbol_type == STRUCT_TYPE_SYMBOL)
        {
            if (insert(symbolTable, ss))
            {
                addStructTypeField(s, ss->name);
            }
            else
            {
                reportError(SemanticError, 15, c->lineNumber, "Wrong field in struct definition.");
            }
        }
        else
        {
            if (!insert(symbolTable, ss))
            {
                reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
            }
        }
        return true;
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ExtDecList, this ExtDecList has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleDec(Morpheme *root, Symbol *s, Symbol *field)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Dec, this Dec is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _Dec)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Dec, this node is not Dec.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Dec.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Dec, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _VarDec && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp)
    {
        addLogInfo(SemanticAnalysisLog, "Going to handle Exp!!!.\n");
        if (s != NULL)
        {
            reportError(SemanticError, 15, c->lineNumber, "Cannot initialize a field in definition of a struct");
            return handleVarDec(c, field);
        }
        else
        {
            handleVarDec(c, field);
        }
    }
    else if (c->type == _VarDec && c->siblings == NULL)
    {
        return handleVarDec(c, field);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Dec, this Dec has a wrong child .\n\033[0m");
        return false;
    }

    return true;
}

bool handleFunDec(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling FunDec, this FunDec is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _FunDec)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling FunDec, this node is not FunDec.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling FunDec.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling FunDec, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _VarList && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP)
    {
        if (c->idName != NULL)
        {
            setSymbolName(s, c->idName);
            return handleVarList(c->siblings->siblings, s);
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling FunDec, the idName is NULL.\n\033[0m");
            return false;
        }
    }
    else if (c->type == _ID && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP)
    {
        if (c->idName != NULL)
        {
            setSymbolName(s, c->idName);
            return true;
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling FunDec, the idName is NULL.\n\033[0m");
            return false;
        }
    }
    return true;
}

bool handleVarList(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarList, this VarList is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _VarList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarList, this node is not VarList.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling VarList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarList, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _ParamDec && c->siblings != NULL && c->siblings->type == _COMMA && c->siblings->siblings != NULL && c->siblings->siblings->type == _VarList)
    {
        Symbol *argument = createSymbol();
        handleParamDec(c, argument);
        insert(symbolTable, argument);
        addFuncArgument(s, argument->name);
        return handleVarList(c->siblings->siblings, s);
    }
    else if (c->type == _ParamDec)
    {
        Symbol *argument = createSymbol();
        handleParamDec(c, argument);
        insert(symbolTable, argument);
        addFuncArgument(s, argument->name);
        return true;
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling VarList, this VarDecList has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleParamDec(Morpheme *root, Symbol *s)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ParamDec, this ParamDec is NULL.\n\033[0m");
        return false;
    }
    if (root->type != _ParamDec)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ParamDec, this node is not ParamDec.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling ParamDec.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ParamDec, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _Specifier && c->siblings != NULL && c->siblings->type == _VarDec)
    {
        ValueTypes *type = (ValueTypes *)malloc(sizeof(ValueTypes));
        char **name = (char **)malloc(sizeof(char *));
        handleSpecifier(c, type, name);
        handleVarDec(c->siblings, s);
        if (s->symbol_type == ARRAY_SYMBOL)
        {
            setArrayType(s, *type, *name);
        }
        else
        {
            if (*type == _INT_TYPE_)
            {
                setSymbolType(s, INT_SYMBOL);
            }
            else if (*type == _FLOAT_TYPE_)
            {
                setSymbolType(s, FLOAT_SYMBOL);
            }
            else if (*type == _STRUCT_TYPE_)
            {
                setSymbolType(s, STRUCT_VAL_SYMBOL);
                setStructValueType(s, *name);
            }
        }
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling ParamDec, this ParamDec has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleCompSt(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling CompSt, this CompSt is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _CompSt)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling CompSt, this node is not CompSt.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling CompSt.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling CompSt, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _LC && c->siblings != NULL && c->siblings->type == _DefList && c->siblings->siblings != NULL && c->siblings->siblings->type == _StmtList && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RC)
    {
        handleDefList(c->siblings, NULL);
        handleStmtList(c->siblings->siblings);
        return true;
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling CompSt, this CompSt has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleStmtList(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StmtList, this StmtList is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _StmtList)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StmtList, this node is not StmtList. \n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling StmtList.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StmtList, child node is NULL .\n\033[0m");
        return false;
    }
    if (c->type == _BLANK)
    {
        addLogInfo(SemanticAnalysisLog, "\033[32mwhen handling StmtList, this StmtList is empty .\n\033[0m");
        return true;
    }
    while (c != NULL)
    {
        if (c->type == _StmtList)
        {
            handleStmtList(c);
        }
        else if (c->type == _Stmt)
        {
            handleStmt(c);
        }
        else
        {
            addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling StmtList, this StmtList has a wrong child .\n\033[0m");
        }
        c = c->siblings;
    }
    return true;
}

bool handleStmt(Morpheme *root)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Stmt, this Stmt is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _Stmt)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Stmt, this node is not Stmt.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Stmt.\n\033[0m");
    Morpheme *c = root->child;
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Stmt, child node is NULL .\n\033[0m");
        return false;
    }

    if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _SEMI)
    {
        //Stmt := Exp SEMI
        //TODO : 确定Exp的类型
        addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
        ExpType *expType = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, expType);
        return true;
    }
    else if (c->type == _CompSt)
    {
        return handleCompSt(c);
    }
    else if (c->type == _RETURN && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI)
    {
        //Stmt := RETURN Exp SEMI
        // TODO
        addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
        ExpType *expType = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c->siblings, expType);
        return true;
    }
    //注意条件表达式只能是INT
    else if (c->type == _WHILE && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt)
    {
        return handleStmt(c->siblings->siblings->siblings->siblings);
    }
    else if (c->type == _IF && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt && c->siblings->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->siblings->type == _ELSE && c->siblings->siblings->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->siblings->siblings->type == _Stmt)
    {
        handleStmt(c->siblings->siblings->siblings->siblings);
        handleStmt(c->siblings->siblings->siblings->siblings->siblings->siblings);
        return true;
    }
    else if (c->type == _IF && c->siblings != NULL && c->siblings->type == _LP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->type == _RP && c->siblings->siblings->siblings->siblings != NULL && c->siblings->siblings->siblings->siblings->type == _Stmt)
    {
        return handleStmt(c->siblings->siblings->siblings->siblings);
    }
    else
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Stmt, this Stmt has a wrong child .\n\033[0m");
        //printf("\033[31mwhen handling Stmt, this Stmt has a wrong child .\n\033[0m");
        return false;
    }
    return true;
}

bool handleExp(Morpheme *root, ExpType *expType)
{
    if (root == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Exp, this Exp is NULL.\n\033[0m");
        return false;
    }

    if (root->type != _Exp)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Exp, this node is not Exp.\n\033[0m");
        return false;
    }
    addLogInfo(SemanticAnalysisLog, "\033[32mStart handling Exp.\n\033[0m");

    Morpheme *c = root->child;
    //错误处理
    if (c == NULL)
    {
        addLogInfo(SemanticAnalysisLog, "\033[31mwhen handling Exp, child node is NULL .\n\033[0m");
        return false;
    }
    // case : EXP->ID
    else if (c->type == _ID && c->siblings == NULL)
    {
        //printf("Exp->id\n");
        char *idName = c->idName;
        if (!isContain(symbolTable, idName))
        {
            reportError(SemanticError, 1, c->lineNumber, "Undefined ID");
            return false;
        }
        else
        {
            Symbol *s = get(symbolTable, idName);
            switch (s->symbol_type)
            {
            case INT_SYMBOL:
                expType->type = _INT_TYPE_;
                expType->leftValue = true;
                break;
            case FLOAT_SYMBOL:
                expType->type = _FLOAT_TYPE_;
                expType->leftValue = true;
                break;
            case ARRAY_SYMBOL:
                expType->type = _ARRAY_TYPE_;
                expType->leftValue = true;
                expType->arrayContent = s->array_content;
                break;
            case STRUCT_VAL_SYMBOL:
                expType->type = _STRUCT_TYPE_;
                expType->leftValue = true;
                expType->typeName = s->struct_value->typeName;
                break;
            case STRUCT_TYPE_SYMBOL:
                reportError(SemanticError, 1, c->lineNumber, "Illegal ID");
                return false;
            }
            return true;
        }
    }
    // case : EXP->INT
    else if (c->type == _INT && c->siblings == NULL)
    {
        //printf("Exp -> int\n");
        expType->type = _INT_TYPE_;
        expType->leftValue = false;
        return true;
    }
    // case : EXP->FLOAT
    else if (c->type == _FLOAT && c->siblings == NULL)
    {
        expType->type = _FLOAT_TYPE_;
        expType->leftValue = false;
        return true;
    }
    // case : EXP->EXP ASSIGNOP EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _ASSIGNOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("Exp -> Exp = Exp\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        if (!type1->leftValue)
        {
            reportError(SemanticError, 6, c->lineNumber, "Expression cannot be right value");
            return false;
        }
        //check type
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 5, c->lineNumber, "Wrong use of array");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 5, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        expType->typeName = type1->typeName;
        return true;
    }
    // case : EXP->EXP PLUS EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _PLUS && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP -> EXP + EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //加法算式中，加法两边不能是数组和结构体
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
            return false;
        }
        if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        return true;
    }
    // case : EXP->EXP MINUS EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _MINUS && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP -> EXP - EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //减法算式中，加法两边不能是数组和结构体
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
            return false;
        }
        if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        return true;
    }
    // case : EXP->EXP STAR EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _STAR && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP -> EXP * EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //乘法算式中，加法两边不能是数组和结构体
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
            return false;
        }
        if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        return true;
    }
    // case : EXP->EXP DIV EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _DIV && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP->EXP - EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //除法算式中，加法两边不能是数组和结构体
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
            return false;
        }
        if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        return true;
    }
    // case : EXP->EXP RELOP EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _RELOP && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP->EXP relop EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //relop算式中，加法两边不能是数组和结构体
        if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
            return false;
        }
        if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
            return false;
        }
        else if (!expTpyeEqual(type1, type2))
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = type1->type;
        return true;
    }
    // case : EXP-> EXP AND EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _AND && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP->EXP && EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //‘与’运算中，两个操作数都要是INT
        if (type1->type != _INT_TYPE_ || type2->type != _INT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = _INT_TYPE_;
        return true;
    }
    // case : EXP-> EXP OR EXP
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _OR && c->siblings->siblings != NULL && c->siblings->siblings->type == _Exp && c->siblings->siblings->siblings == NULL)
    {
        //printf("EXP->EXP || EXP\n");
        ExpType *type1 = (ExpType *)malloc(sizeof(ExpType));
        ExpType *type2 = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, type1);
        handleExp(c->siblings->siblings, type2);
        //check type
        //‘或’运算中，两个操作数都要是INT
        if (type1->type != _INT_TYPE_ || type2->type != _INT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
            return false;
        }
        //pass
        expType->leftValue = false;
        expType->type = _INT_TYPE_;
        return true;
    }
    // case : EXP -> LP EXP RP
    else if (c->type == _LP && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _RP && c->siblings->siblings->siblings == NULL)
    {
        return handleExp(c->siblings, expType);
    }
    // case : EXP -> MINUS EXP
    else if (c->type == _MINUS && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings == NULL)
    {
        ExpType *childType = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c->siblings, childType);
        if (childType->type != _INT_TYPE_ || childType->type != _FLOAT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch"); //取负操作符的操作数只能是int或者float
            return false;
        }
        //pass
        expType->type = childType->type;
        expType->leftValue = false;
        return true;
    }
    // case : EXP -> NOT EXP
    else if (c->type == _NOT && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings == NULL)
    {
        ExpType *childType = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c->siblings, childType);
        if (childType->type != _INT_TYPE_)
        {
            reportError(SemanticError, 7, c->lineNumber, "Type mismatch"); //NOT操作符的操作数只能是int
            return false;
        }
        //pass
        expType->type = _INT_TYPE_;
        expType->leftValue = false;
        return true;
    }
    // case : EXP -> EXP DOT ID
    else if (c->type == _Exp && c->siblings != NULL && c->siblings->type == _DOT && c->siblings->siblings != NULL && c->siblings->siblings->type == _ID && c->siblings->siblings->siblings == NULL)
    {
        ExpType *childType = (ExpType *)malloc(sizeof(ExpType));
        handleExp(c, childType);
        if(childType->type != _STRUCT_TYPE_) {
            reportError(SemanticError, 13, c->lineNumber, "Type mismatch"); //DOT操作符的左操作数只能是struct
            return false;
        }
        // check whether id is the field of struct
        StructTypeContent* s = get(symbolTable, childType->typeName)->struct_def;
        char* fieldName = c->siblings->siblings->idName;
        if(!isField(s, fieldName)) {
            reportError(SemanticError, 14, c->lineNumber, "Undefined field"); //DOT操作符的左操作数只能是struct
            return false;
        }
        //pass
        //int,float,struct可以作为左值, array不行
        

    }
}
