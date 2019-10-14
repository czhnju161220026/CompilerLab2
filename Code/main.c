#include<stdio.h>
#include "grammarTree.h"
extern void yyrestart(FILE*);
extern void yyparse(void);
extern Morpheme* root;
extern int syntax_correct;
extern int lexical_correct;

int main(int argc, char** argv) {
    if(argc <= 1) {
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
    }
}