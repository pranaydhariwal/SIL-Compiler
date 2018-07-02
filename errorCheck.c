#include "y.tab.h"
extern int lineNo;


int checkDataType(tnode *node){
    if (strcmp(node->name, "integer") && strcmp(node->name, "boolean")){
        if(tSearch(node->name) == NULL){
            printf("Line : %d :: %s data Type does not exist\n", lineNo, node->name);
            exit(0);
        }
    }
    return 0;
}


/*What happens if it is declared as not pass by reference but defined as passByRef = True*/

int checkArrayDeclaration(tnode *node){
    if (node->gEntry->size == 0){
        if (node->left != NULL){
            printf("Line: %d :: Array has not been declared\n", lineNo);
            exit(0);
        }
    }
    else{
        if (node->left == NULL){
            printf("Line: %d :: Array has been declared but a variable has been used\n", lineNo);
            exit(0);
        }
        else{
            if (node->left->dataType != integer){
                printf("Line: %d :: Cannot be boolean index inside an array\n", lineNo);
                exit(0);
            }
        }
    }
}

int idDeclarationCheck(tnode *r){
    if (r->nodeType == ID){
        if (r->lEntry == NULL){
            if (r->gEntry == NULL){
    			printf("Line: %d :: '%s' not declared\n", lineNo, r->name);
    			exit(1);
    		}
            else
                checkArrayDeclaration(r);
        }
	}
    return 0;
}

int dataTypeCheck(tnode *l, tnode *r, int type){
    if (type == 0){ //This is meant for Assignment operation
        if (r->dataType != ALLOC){
            if (l->dataType != r->dataType){
                printf("Line: %d :: sInvalid data type declarations\n", lineNo);
                exit(1);
            }
            if (l->nodeType != ID && l->nodeType != USERDEF){
                printf("Line: %d :: Left side of ASSG has to be a variable.\n", lineNo);
        		exit(1);
            }
        }
    }
    else if (type == VOID){
        if (l->dataType <= BASIC_OPS){
            printf("Line: %d :: NULL has been assigned to basic data types\n", lineNo);
            exit(0);
        }
    }
    else{
        if (l != NULL){
            //printf("%d %d\n", l->dataType, type);
            if (l->dataType != type){
        		printf("Line: %d :: pInvalid data type declarations\n", lineNo);
        		exit(1);
        	}
        }
        if (r != NULL){
            if (r->dataType != type){
        		printf("Line: %d :: qInvalid data type declarations\n", lineNo);
        		exit(1);
        	}
        }
    }
    return 0;
}

int checkReturnType(int type1, int type2){
    if (type1 != type2){
        printf("Line: %d :: Return type mismatch\n", lineNo);
        exit(0);
    }
    return 0;
}

int checkArgumentType(tnode *node){ //check if arguments declared to the functions (or in local decl) are not arrays or functions
    int flag = 0;
    tnode *temp1 = node;
    while (temp1 != NULL){
        tnode *temp = temp1;
        while(temp != NULL){
            if (temp->nodeType == STMT && temp->val == 0)
                flag = 0;
            else
                flag = 1;
            temp = temp->left;
        }
        temp1 = temp1->right;
    }
    if (flag == 1){
        printf("Line : %d :: Argument type mismatch.\n", lineNo);
        exit(0);
    }
    return 0;
}

int searchArgList(argList *arg, tnode *argument){
    argList *arglist = makeArgList(argument);
    while(arglist != NULL){
        if (strcmp(arglist->name, arg->name) != 0 || arg->type != arglist->type || arg->passByRef != arglist->passByRef){
            printf("Line : %d :: Arguments in declaration do not match in the definitions\n", lineNo);
            exit(0);
        }
        arglist = arglist->next;
        arg = arg->next;
    }
    if (arg != NULL){
        printf("Line : %d :: Number of arguments in function declaration and definition do not match\n", lineNo);
        exit(0);
    }
    return 0;
}

int checkFunctionDecl(char *name, int type, tnode *argument){
    if (strcmp(name, "main") == 0){
        if (type != tSearch("integer")->index){
            printf("Line : %d :: Return type of main is integer\n", lineNo);
            exit(0);
        }
        if (argument != NULL){
            printf("Line : %d :: Main Function cannot have arguments\n", lineNo);
            exit(0);
        }
        return 0;
    }
    gTable *temp = gSearch(name);
    if (temp == NULL){
        printf("Line: %d :: Function %s has not been declared\n", lineNo, name);
        exit(0);
    }
    if (temp->type != type){
        printf("Line : %d :: Function Declaration and function definition types are different\n", lineNo);
        exit(0);
    }
    searchArgList(temp->arg, argument); //check if arguments have been declared properly
    return 0;
}

int checkPassedArgument(tnode *id, tnode *exprList){
    gTable *gTemp = gSearch(id->name);
    if (gTemp == NULL){
        printf("Line : %d :: The function %s has not been declared.\n", lineNo, id->name);
        exit(0);
    }
    argList *arg = gTemp->arg;
    while(exprList != NULL && arg != NULL){
        if (exprList->dataType != arg->type){
            printf("Line : %d :: Argument type mismatch in %s function.\n", lineNo, id->name);
            exit(0);
        }
        exprList = exprList->expr;
        arg = arg->next;
    }
    if (arg != NULL || exprList != NULL){
        printf("Line : %d :: Number of arguments is not same as declared in '%s' function\n", lineNo, id->name);
        exit(0);
    }
    return 0;
}
