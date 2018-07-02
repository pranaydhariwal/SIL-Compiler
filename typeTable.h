typedef struct typeTable {
    char *name;
    int index;
    struct FieldList *field;
    struct typeTable *next;
} typeTable;

typedef struct FieldList{
    char *name;
    int bindingVal;
    struct typeTable *type;
    struct FieldList *next;
} FieldList;

void createTypeTable();

void printTypeTable(); //for debugging purposes

FieldList *makeFieldList(tnode *node);

typeTable *tSearch(char *name);

typeTable *tSearchByIndex(int index);

void *groupTInstall(tnode *node);

void *tInstall(char *name, FieldList *field);

FieldList *fSearch(char *name, typeTable *table);
