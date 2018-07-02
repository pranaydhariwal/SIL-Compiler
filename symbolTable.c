gTable *gStart = NULL;
lTable *lStart = NULL;
extern int lineNo;

void provideMemoryToGlobal(){
	gTable *temp;
	temp = gStart;
	while(temp != NULL){
        if (temp->nodeType == VAR){
            temp->binding = memory;
            if (temp->size == 0)
                memory += 1;
    		memory += temp->size;
        }
		temp = temp->next;
	}
}

int searchArg(argList *arg, char *name){
	while(arg != NULL){
		if (!strcmp(arg->name, name))
			return 1;
		arg = arg->next;
	}
	return 0;
}

void provideMemoryToLocal(char *name, lTable *table){
	printf("Providing memory to %s\n", name);
	if (!strcmp(name, "main")){
		lTable *temp;
		temp = table;
	  	int mem = 1;
		while(temp != NULL){
			temp->binding = mem;
			mem++;
			temp = temp->next;
		}
		return;
	}
	gTable *global = gSearch(name);
	argList *arg = global->arg;
	lTable *temp;
	temp = table;
  	int mem = 1;
	int mem2 = -3;
	while (temp != NULL){
		if (searchArg(arg, temp->name)){
			temp->binding = mem2;
			mem2--;
		}
		else{
			temp->binding = mem;
			mem++;
		}
		temp = temp->next;
	}
	printf("Exiting memory\n");
	return;
}

void cleanLocalMemory(lTable *table){
	lTable *temp;
	temp = table;
	while (temp != NULL){
		memory -= 1;
		temp = temp->next;
	}
}

void printSymbolTable(){
    gTable *temp = gStart;
    printf("Global Symbol table\n");
    while (temp != NULL){
        printf("%s %d %d\n", temp->name, temp->type, temp->size);
        temp = temp->next;
    }
    printf("\n");
}

void printLocalSymTable(char *name){
    lTable *temp = lStart;
    printf("Local Symbol table for %s function\n", name);
    while (temp != NULL){
        printf("%s %d %d\n", temp->name, temp->type, temp->bindingType);
        temp = temp->next;
    }
    printf("\n");
}

tnode *addDataType(int type, tnode *node){
	tnode *temp = node;
	while(node != NULL){
		node->dataType = type;
		node = node->left;
	}
	return temp;
}

struct argList *makeArgList(tnode *node){
    argList *temp, *start;
    tnode *temp1;
    start = NULL;
    while(node != NULL){
        temp1 = node;
        while(temp1 != NULL){
            temp = (argList*)malloc(sizeof(argList));
            temp->name = temp1->name;
            temp->type = temp1->dataType;
			temp->passByRef = temp1->passByRef;
            temp->next = start;
            start = temp;
            temp1 = temp1->left;
        }
        node = node->right;
    }
	argList *t = NULL, *t2 = start, *t3; //Reversing this list
    while(t2 != NULL){
        t3 = t2->next;
        t2->next = t;
        t = t2;
        t2 = t3;
    }
    return t;
}

void groupGInstall(tnode *node, int type){
  tnode *temp;
  temp = node;
  while (temp != NULL){
    if (temp->nodeType == FUNC){
        fInstall(temp->name, type, temp->expr);
    }
    else
        gInstall(temp->name, type, temp->val);
    temp = temp->left;
  }
}

void fInstall(char *name, int type, tnode *arguments){
    gTable *tempPointer = gSearch(name);
    if (tempPointer != NULL){
			printf("Line: %d :: Variable %s already declared\n", lineNo, name);
			exit(1);
	}
    gTable *temp;
    temp = (gTable*)malloc(sizeof(gTable));
    temp->name = name;
    temp->type = type;
	temp->nodeType = FUNC;
    temp->arg = makeArgList(arguments);
    temp->next = gStart;
    gStart = temp;
}

void gInstall(char *name, int type, int size){
  //printf("Declaring Variable %s\n", name);
  gTable *tempPointer = gSearch(name);
	if (tempPointer != NULL){
			printf("Line: %d :: Variable %s already declared\n", lineNo, name);
			exit(1);
	}
  gTable *temp;
  temp = (gTable*)malloc(sizeof(gTable));
  temp->name = name;
  temp->type = type;
  temp->nodeType = VAR;
  temp->size = size;
  temp->arg = NULL;
  temp->next = gStart;
  gStart = temp;                //gStart is the start of the symbol table
}

gTable *gSearch(char *name){
  gTable *temp;
  temp = gStart;
  while(temp != NULL){
    if (strcmp(temp->name, name) == 0){
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

void *argLInstall(tnode *node){
    tnode *temp = node;
    while (temp != NULL){
        groupLInstall(temp, temp->dataType);
        temp = temp->right;
    }
}

void *groupLInstall(tnode *node, int type){
    tnode *temp = node;
    while (temp != NULL){
        lTable *temp2 = lInstall(temp->name, type, temp->passByRef);
        temp2->next = lStart;
        lStart = temp2;
        temp = temp->left;
    }
}

lTable *lSearch(char *name){
    lTable *temp = lStart;
    while(temp != NULL){
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

lTable *lInstall(char *name, int type, int passByRef){
    lTable *tempPointer = lSearch(name);
  	if (tempPointer != NULL){
  			printf("Line: %d :: Variable %s already declared in local scope\n", lineNo, name);
  			exit(1);
  	}
    lTable *temp;
    temp = (lTable*)malloc(sizeof(lTable));
    temp->name = name;
    temp->type = type;
    temp->bindingType = passByRef;
    return temp;
}
