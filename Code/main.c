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
    printf("%d\n", insert(set, s));

}