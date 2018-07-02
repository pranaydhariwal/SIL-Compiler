typeTable *tTable = NULL;
int indexForTable = 3;
extern int lineNo;

void createTypeTable(){
    typeTable *temp;
    temp = (typeTable*)malloc(sizeof(typeTable));
    temp->name = "boolean";
    temp->index = 1;
    temp->field = NULL;
    temp->next = tTable;
    tTable = temp;
    temp = (typeTable*)malloc(sizeof(typeTable));
    temp->name = "integer";
    temp->index = 2;
    temp->field = NULL;
    temp->next = tTable;
    tTable = temp;
    printf("Type table created\n");
    return;
}

void printTypeTable(){
    typeTable *t = tTable;
    printf("Printing the type table\n");
    while(t != NULL){
        FieldList *f = t->field;
        if (f != NULL){
            printf("%s %d ", t->name, t->index);
            while (f != NULL){
                printf("%s ", f->name);
                f = f->next;
            }
            printf("\n");
        }
        else
            printf("%s %d\n", t->name, t->index);
        t = t->next;
    }
    printf("\n");
    return;
}

FieldList *makeFieldList(tnode *node){
    FieldList *f = NULL, *temp;
    int memValue = 1;
    while(node != NULL){
        temp = (FieldList*)malloc(sizeof(FieldList));
        temp->bindingVal = memValue;
        memValue++;
        temp->name = node->name;
        typeTable *t = tTable;
        printf("Inside Fields %s %d\n",temp->name, temp->bindingVal);
        while(t != NULL){
            //printf("%s %s\n", t->name, node->expr->name);
            if (strcmp(t->name, node->expr->name) == 0){
                temp->type = t;
                break;
            }
            else
                temp->type = NULL;
            t = t->next;
        }
        if (temp->type == NULL){
            printf("'%s' user data type not declared\n", node->name);
            exit(0);
        }
        temp->next = f;
        f = temp;
        node = node->left;
    }
    return f;
}

typeTable *tSearch(char *name){
    typeTable *temp = tTable;
    while(temp != NULL){
        if(!strcmp(name, temp->name)){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

typeTable *tSearchByIndex(int index){
    typeTable *temp = tTable;
    while(temp != NULL){
        if(temp->index == index)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void *groupTInstall(tnode *node){
    tnode *temp = node;
    while (temp != NULL){
        if (temp->expr == NULL){
            printf("Nothing in expr of %s\n", temp->name);
            exit(0);
        }
        FieldList *f = makeFieldList(temp->expr);
        if (f == NULL){
            printf("Nothing in field of %s\n", temp->name);
            exit(0);
        }
        tInstall(temp->name, f);
        temp = temp->right;
    }
}

void *tInstall(char *name, FieldList *field){
    if (tSearch(name) != NULL){
        printf("'%s' type already exists\n", name);
        exit(0);
    }
    typeTable *temp;
    temp = (typeTable*)malloc(sizeof(typeTable));
    temp->index = indexForTable;
    indexForTable++;
    temp->name = name;
    temp->field = field;
    temp->next = tTable;
    tTable = temp;
}

FieldList *fSearch(char *name, typeTable *table){
    FieldList *f = table->field;
    printf("Strings in field compared\n");
    while(f != NULL){
        if (!strcmp(name, f->name))
            return f;
        f = f->next;
    }
    return NULL;
}
