#include <stdio.h>
#include "grammarTree.h"
#include "semantic.h"
extern void yyrestart(FILE*);
extern void yyparse(void);
extern Morpheme* root;
extern int syntax_correct;
extern int lexical_correct;

int main(int argc, char** argv) {
    /*if(argc <= 1) {
        printf("pass filename to scanner\n");
        return -1;
    }
    else {
        FILE* f = fopen(argv[1], "r");
        if(!f) {
            printf("fopen failed:%s\n",argv[1]);
            return -1;
        }
        yyrestart(f);
        yyparse();
        if(syntax_correct && lexical_correct) 
            printGrammarTree(root, 0);
        destructMorpheme(root);
        return 0;
    }*/
    /*
    char s1[] = "asadb";
    char s2[] = "abc";
    char s3[] = "1234";
    printf("%u\n", pjwHash(s1));
    printf("%u\n", pjwHash(s2));
    printf("%u\n", pjwHash(s3));
    HashSet* set = initializeHashSet(HASH_SIZE);
    printf("%d\n", isContain(set, s1));
    Symbol* s = createSymbol(s1);
    printf("%d\n", insert(set, s));
    printf("%d\n", isContain(set, s1));
    Symbol* p = get(set, "asadb");
    printf("%s\n", p->name);
    printf("%d\n", insert(set, s));*/

    HashSet* symbolTable = initializeHashSet(HASH_SIZE);
    //创建一个代表struct类型的符号 Student(int age, float score);
    Symbol* s1 = createSymbol("Student", 
                            STRUCT_TYPE_SYMBOL, 
                            2, 
                            createField("age", _INT_TYPE_), 
                            createField("score", _FLOAT_TYPE_));
    //创建一个返回struct Student, 接受int的函数f
    Symbol* s2 = createSymbol("f",
                            FUNC_SYMBOL,
                            2,
                            createRetValue(_STRUCT_TYPE_, "Student"),
                            createArgument("arg1", _INT_TYPE_));
    //创建一个嵌套定义的struct School
    Symbol* s3 = createSymbol("School",
                            STRUCT_TYPE_SYMBOL,
                            3,
                            createField("student", _STRUCT_TYPE_, "Student"),
                            createField("a", _INT_TYPE_),
                            createField("b", _FLOAT_TYPE_));
    //创建一个Student数组
    Symbol* s4 = createSymbol("students",
                            ARRAY_SYMBOL,
                            4,
                            _STRUCT_TYPE_,
                            "Student",
                            12,
                            34);
    outputSymbol(s1);
    outputSymbol(s2);
    outputSymbol(s3);
    outputSymbol(s4);

}