# Compile Lab 2: semantic analysis
## HASH SET
### HASH_SIZE
* 定义了哈希集的大小
### HashSet* initializeHashSet(int size)
* 初始化一个大小为size的哈希表，并返回这个哈希表的指针
* 如果创建失败，返回NULL
### unsigned int pjwHash(char* str)
* 输入为一个字符串，输出为一个哈希出来的整数值
### bool isContain(HashSet* hashSet, char* name)
* 如果哈希表里存在名为name的符号，返回true；否则返回false。
### bool insert(HashSet* hashSet, Symbol* symbol)
* 向哈希表里插入一个符号symbol
* 如果成功插入，返回true，否则返回false。
* 因为insert函数实现里已包含了contain的检查，所以建议在插入前不要做额外的存在性检查。
### Symbol* get(HashSet* hashSet, char* name)
* 从哈希集里获取一个名为name的符号
* 如果存在该符号，返回对应指针；如果不存在返回NULL。
* 建议在get前不要使用isContain检查符号是否在哈希表里，应通过返回的指针是否为NULL来判断。
### Symbol* createSymbol(char* name)
* 创建一个名为name的符号，并返回其指针。

