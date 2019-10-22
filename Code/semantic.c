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
        }
        return handleVarDec(c, field);
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
        addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
        return true;
    }
    else if (c->type == _CompSt)
    {
        return handleCompSt(c);
    }
    else if (c->type == _RETURN && c->siblings != NULL && c->siblings->type == _Exp && c->siblings->siblings != NULL && c->siblings->siblings->type == _SEMI)
    {
        //Stmt := RETURN Exp SEMI
        addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
        return true;
    }
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
