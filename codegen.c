#include "y.tab.h"

int getRegNo(){
    if (regNo >= 7){
        printf("Registers out of bound\n");
        exit(0);
    }
    int reg= regNo;
    regNo++;
    return reg;
}

int freeReg(){
    if (regNo == 0){
        printf("Register has not yet been allocated\n");
    }
    regNo--;
}

int generateLabel(){
    label++;
    return label;
}

int getAddress(tnode *t){
    int address, r1;
    if (t->lEntry == NULL){ //for global variables
        address = t->gEntry->binding;
        r1 = getRegNo();
        fprintf(filePtr, "MOV R%d, %d\n", r1, address);
        if (t->left != NULL){//For function arguments, it's going here. Only meant for arrays
            int offset = opCodeGen(t->left);
            fprintf(filePtr, "ADD R%d, R%d\n", r1, offset);
            freeReg();
        }
    }
    else{ //for local variables
        address = t->lEntry->binding;
        r1 = getRegNo();
        int r = getRegNo();
        fprintf(filePtr, "MOV R%d, BP\n", r1);
        fprintf(filePtr, "MOV R%d, %d\n", r, address);
        fprintf(filePtr, "ADD R%d, R%d\n", r1, r);
        if (t->lEntry->bindingType == 1){
            fprintf(filePtr, "MOV R%d, [R%d]\n", r1, r1);
        }
        freeReg();
    }
    return r1;
}

int getAddUserDefined(tnode *t){
    fprintf(filePtr, "//Get address for - %s %d\n", t->name, t->nodeType);
    int r1 = opCodeGen(t);
    typeTable *tab = tSearchByIndex(t->dataType);
    fprintf(filePtr, "//Opcode over for %s\n", t->name);
    t = t->expr;
    fprintf(filePtr, "//Part of userdefined\n");
    while (t != NULL){
        fprintf(filePtr, "//t->name - %s\n", t->name);
        FieldList *f = fSearch(t->name, tab);
        if (f != NULL){
            int offset = f->bindingVal;
            printf("%d ", offset);
            int r2 = getRegNo();
            fprintf(filePtr, "MOV R%d, %d\n", r2, offset);
            fprintf(filePtr, "ADD R%d, R%d\n", r1, r2);
            freeReg();
            if (t->left != NULL)
                fprintf(filePtr, "MOV R%d, [R%d]\n", r1, r1);
        }
        t = t->left;
    }
    printf("\n");
    return r1;
}

int opCodeGen(tnode *t){
    //printf("Entering for %d\n", t->nodeType);
    int r1, r2;
    //int r2 = getRegNo();
    //printf("Register no: %d %d\n", r1, r2);
    switch (t->nodeType) {
        case ALLOC:
            r1 = alloc(); // defined in heapManagement.c
            break;
        case FREEM:{
            printf("Entered here\n");
            r1 = getAddUserDefined(t->expr);
            printf("Add for free: %d\n", r1);
            int dummy = freeMem(r1); //defined in heapManagement.c
            fprintf(filePtr, "MOV R%d, %d\n", r1, dummy);
            break;
        }
        case VOID:
            r1 = getRegNo();
            fprintf(filePtr, "MOV R%d, -1\n", r1);
            break;
        case USERDEF:{
            printf("userdef\n");
            r1 = getAddUserDefined(t->left);
            fprintf(filePtr, "MOV R%d, [R%d]\n", r1, r1);
            printf("opcode over\n");
            break;
        }
        case NUM:{
            int value;
            if (t->dataType == (tSearch("integer")->index))
                value = t->val;
            else
                value = t->boolVal;
            r1 = getRegNo();
            fprintf(filePtr, "MOV R%d, %d\n", r1, value);
            break;
        }
        case ID:{ //change in code here because now you might need to put in heap
            printf("Enter ID\n");
            r1 = getAddress(t);
            printf("Val of r1: %d\n", r1);
            fprintf(filePtr, "MOV R%d, [R%d]\n", r1, r1);
            break;
        }
        case CALL:{
            t = t->left;
            //fprintf(filePtr, "//Called %s\n", t->name);
            int i = 0;
            while(i < regNo){
                fprintf(filePtr, "PUSH R%d\n", i);
                i++;
            }
            //Pushing the arguments
            tnode *temp = t->expr;
            int count = 0;
            fprintf(filePtr, "//Pushing the arguments\n");
            argList *arg = t->gEntry->arg;
            while (temp != NULL){
                printf("Arg should be %s\n", arg->name);
                if (arg->passByRef == 0){
                    printf("global arg for %s\n", temp->name);
                    //fprintf(filePtr, "//For arg %s\n", temp->name);
                    r1 = opCodeGen(temp);
                }
                else{
                    //printf("local arg for %s\n", temp->name);
                    fprintf(filePtr, "//For args %s\n", temp->name);
                    if (temp->nodeType == USERDEF)
                        r1 = getAddUserDefined(temp->left);
                    else
                        r1 = getAddress(temp);
                }
                fprintf(filePtr, "PUSH R%d\n", r1);
                freeReg();
                temp = temp->expr;
                arg = arg->next;
                count++;
            }
            fprintf(filePtr, "//Pushing arguments done\n");
            r1 = getRegNo(); //For return value
            fprintf(filePtr, "PUSH R%d\n", r1);
            fprintf(filePtr, "CALL %s\n", t->name);
            fprintf(filePtr, "POP R%d\n", r1); //Need to save this value somewhere
            //Popping off the arguments
            temp = t->expr;
            fprintf(filePtr, "//Popping off arguments\n");
            r2 = getRegNo();
            while(temp != NULL){
                fprintf(filePtr, "POP R%d\n", r2);
                temp = temp->expr;
            }
            freeReg();
            fprintf(filePtr, "//Popping arguments done\n");
            while (i > 0){
                i--;
                fprintf(filePtr, "POP R%d\n", i);
            }
            fprintf(filePtr, "//Calling done\n");
            break;
        }
        case PLUS:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "ADD R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case SUB:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "SUB R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case MUL:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "MUL R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case DIV:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "DIV R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case MOD:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "MOD R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case LT:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "LT R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case GT:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "GT R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case LE:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "LE R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case GE:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "GE R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case EQ:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "EQ R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case NE:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "NE R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case AND:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "MUL R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case OR:
            r1 = opCodeGen(t->left);
            r2 = opCodeGen(t->right);
            fprintf(filePtr, "ADD R%d, R%d\n", r1, r2);
            freeReg();
            break;
        case NOT:
            r1 = opCodeGen(t->left);
            fprintf(filePtr, "EQ R%d, 0\n", r1);
            break;
        default:
            printf("Error in codegen for %d type\n", t->nodeType);
            exit(0);
    }
    return r1;
}

int stCodeGen(tnode *t){
    printf("Entering for st: %d\n", t->nodeType);
    int r1;
    switch (t->nodeType) {
        case STMT:
            while(t != NULL){
                if (t->expr != NULL)
                    stCodeGen(t->expr);
                t = t->left;
            }
            break;
        case ASSG:{
            printf("\nNodetype: %d\n\n", t->left->nodeType);
            if (t->left->nodeType == USERDEF)
                r1 = getAddUserDefined(t->left->left);
            else
                r1 = getAddress(t->left);
            printf("Val of r1 in ASSG: %d\n", r1);
            fprintf(filePtr, "//Right side\n");
            int r2 = opCodeGen(t->right);
            printf("After opCodeGen\n");
            fprintf(filePtr, "MOV [R%d], R%d\n", r1, r2);
            freeReg();
            freeReg();
            break;
        }
        case READ:{
            if (t->expr->nodeType == USERDEF)
                r1 = getAddUserDefined(t->expr->left);
            else
                r1 = getAddress(t->expr);
            printf("Register address in READ\n");
            int r2 = getRegNo();
            fprintf(filePtr, "IN R%d\n", r2);
            fprintf(filePtr, "MOV [R%d], R%d\n", r1, r2);
            freeReg();
            freeReg();
            break;
        }
        case WRITE:
            fprintf(filePtr, "//Code for WRITE\n");
            printf("Entered Write\n");
            r1 = opCodeGen(t->expr);
            printf("Finished opcode in WRITE\n");
            fprintf(filePtr, "OUT R%d\n", r1);
            freeReg();
            break;
        case IF:{
            int l1 = generateLabel();
            r1 = opCodeGen(t->expr);
            fprintf(filePtr, "JZ R%d, L%d\n", r1, l1);
            freeReg();
            int l2 = generateLabel();
            stCodeGen(t->left);
            fprintf(filePtr, "JMP L%d\n", l2);
            fprintf(filePtr, "L%d:\n", l1);
            if (t->right != NULL){
                stCodeGen(t->right);
            }
            fprintf(filePtr, "L%d:\n", l2);
            break;
        }
        case WHILE:{
            int l1 = generateLabel();
            int l2 = generateLabel();
            fprintf(filePtr, "L%d:\n", l1);
            r1 = opCodeGen(t->expr);
            fprintf(filePtr, "JZ R%d, L%d\n", r1, l2);
            freeReg();
            stCodeGen(t->left);
            fprintf(filePtr, "JMP L%d\n", l1);
            fprintf(filePtr, "L%d:\n", l2);
            break;
        }
        default:
            break;
    }
    return 0;
}

void returnCodeGen(tnode *node){
    printf("Going for return\n");
    fprintf(filePtr, "//Printing for return\n");
    int r1 = getRegNo();
    int r2 = opCodeGen(node);
    int r3 = getRegNo();
    fprintf(filePtr, "MOV R%d, BP\n", r1);
    fprintf(filePtr, "MOV R%d, 2\n", r3);
    fprintf(filePtr, "SUB R%d, R%d\n", r1, r3);
    fprintf(filePtr, "MOV [R%d], R%d\n", r1, r2);
    freeReg();
    freeReg();
    freeReg();
    return;
}

void funcCodeGen(tnode *node){
    while(node != NULL){
        printf("Printing for %s\n", node->name);
        lTable *table = node->left->lEntry;
        provideMemoryToLocal(node->name, node->left->lEntry);
        fprintf(filePtr, "//Code Gen for %s function\n", node->name);
        fprintf(filePtr, "%s:\n", node->name);
        fprintf(filePtr, "PUSH BP\n");
        fprintf(filePtr, "MOV BP, SP\n");
        if (table == NULL){
            fprintf(filePtr, "//Nothing inside of Local SymTable\n");
        }
        while(table != NULL){
            int r1 = getRegNo();
            fprintf(filePtr, "MOV R%d, %d\n", r1, table->binding);
            fprintf(filePtr, "PUSH R%d\n", r1);
            freeReg();
            table = table->next;
        }
        if (node->left != NULL)
            stCodeGen(node->left);
        returnCodeGen(node->left->right);
        printf("Return code completed\n");
        table = node->left->lEntry;
        while(table != NULL){
            int r1 = getRegNo();
            fprintf(filePtr, "POP R%d\n", r1);
            freeReg();
            table = table->next;
        }
        fprintf(filePtr, "POP BP\n");
        fprintf(filePtr, "RET\n");
        fprintf(filePtr, "\n");
        node = node->right;
    }
    return;
}

void codeGen(tnode *t){
    filePtr = fopen("simulator/asmCode.asm", "w+");
    fprintf(filePtr, "START\n");
    fprintf(filePtr, "MOV BP, 513\n");
    fprintf(filePtr, "MOV SP, %d\n", memory);
    int i = 0;
    while (i < regNo){
        fprintf(filePtr, "PUSH R%d\n", i);
        i++;
    }
    fprintf(filePtr, "CALL initialize\n");
    fprintf(filePtr, "CALL main\n");
    while (i > 0){
        fprintf(filePtr, "POP R%d\n", i);
        i--;
    }
    fprintf(filePtr, "HALT\n");
    fprintf(filePtr, "\n");
    initialize(filePtr);
    funcCodeGen(t);
    fclose(filePtr);
    return;
}
