int memory = 513;

typedef struct argList{
    char *name;
    int type;
    int passByRef;
    struct argList *next;
} argList;

/* For the Global Symbol Table for global declaraions */
typedef struct gTable{
  char *name;
  int type; //Data type
  int nodeType; //for saying if its a variable, array or a function
  int size;
  int binding;
  struct argList *arg;
  struct gTable *next;
  struct lTable *link;
} gTable;

/* For the Local Symbol Table for local declarations in functions */
typedef struct lTable{
    char *name;
    int type;
    int bindingType; //either TRUE or FALSE for pass by reference
    int binding;
    struct lTable *next;
} lTable;

void provideMemoryToGlobal(); //Providing binding location during runtime

void provideMemoryToLocal(char *name, lTable *table);

void cleanLocalMemory(lTable *table);

void printSymbolTable(); //Function to print symbol table (For debugging purposes)

void printLocalSymTable(char *name); //Function to print the local symbol table of name function (For debugging purposes)

tnode *addDataType(int type, tnode *node); //Function to add datatypes in node->dataType. Used in expl.y during arg: integer argInput

argList *makeArgList(tnode *node);

/* Functions for the Global Symbol Table */

void groupGInstall(tnode *node, int type);

gTable *gSearch(char *name);

void fInstall(char *name, int type, tnode *arguments);

void gInstall(char *name, int type, int size);

/*Functions for the Local Symbol Table */

void *argLInstall(tnode *node);

void *groupLInstall(tnode *node, int type);

lTable *lSearch(char *name);

lTable *lInstall(char *name, int type, int passByRef);
