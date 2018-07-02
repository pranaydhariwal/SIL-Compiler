#include "y.tab.h"
extern int lineNo;

struct tnode *makeUserDefined(tnode *node){
	tnode *temp = (tnode*)malloc(sizeof(tnode));
	temp->nodeType = USERDEF;
	temp->dataType = node->dataType;
	typeTable *tab = tTable;
	while (tab != NULL){
		if (node->dataType == tab->index)
			break;
		tab = tab->next;
	}
	tnode *t = node->expr;
	while(t != NULL){
		FieldList *f = tab->field;
		int flag = 0;
		while (f != NULL){
			if (strcmp(t->name, f->name) == 0){
				temp->dataType = f->type->index;
				flag = 1;
				break;
			}
			f = f->next;
		}
		if (flag == 0){
			printf("Line: %d :: The next var does not exist for %s\n", lineNo, t->name);
			exit(0);
		}
		t = t->left;
		if (temp->dataType <= BASIC_OPS && t != NULL){
			printf("Line : %d :: For a primitive data type, concatenation not allowed\n", lineNo);
			exit(0);
		}
	}
	temp->left = node;
	return temp;
}

struct tnode *assignType(char *name){
	tnode *temp = (tnode*)malloc(sizeof(tnode));
	if(tSearch(name) != NULL)
		temp->dataType = tSearch(name)->index;
	temp->name = name;
	return temp;
}

struct tnode *makeFunctionNode(tnode *id, int type, tnode *argList, tnode *body){
	if (strcmp("main", id->name))
		idDeclarationCheck(id);
	checkArgumentType(argList); //see if arguments does not contain arrays or functions
	checkFunctionDecl(id->name, type, argList);
	tnode *temp;
	temp = (tnode*)malloc(sizeof(tnode));
	temp->name = id->name;
	temp->nodeType = FUNC;
	temp->dataType = type;
	temp->expr = argList;
	temp->left = body;
	temp->gEntry = gSearch(id->name);
	return temp;
}

struct tnode *makeFunctionCall(tnode *id, tnode *exprList){
	tnode *temp;
	temp = (tnode*)malloc(sizeof(tnode));
	id->expr = exprList;
	temp->name = "CALL";
	temp->expr = NULL;
	temp->left = id;
	temp->right = NULL;
	temp->nodeType = CALL;
	temp->gEntry = gSearch(id->name);
	if (temp->gEntry == NULL){
		printf("Line : %d :: Function %s has not been declared.\n", lineNo, id->name);
		exit(0);
	}
	else
		temp->dataType = temp->gEntry->type;
	return temp;
}

struct tnode *makeStatement(struct tnode *node, struct tnode *next){
	tnode *temp;
	temp = (tnode*)malloc(sizeof(tnode));
	temp->nodeType = STMT;
	temp->dataType = VOID;
	temp->expr = node;
	temp->left = next;
	return temp;
}

struct tnode *makeLeaf(int n, int type){
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->name = NULL;
	temp->dataType = type;
	if (type == tSearch("integer")->index){
		temp->val = n;
		temp->boolVal = -1;
	}
	else{
		temp->boolVal = n;
		temp->val = -1;
	}
	temp->nodeType = NUM;
	temp->expr = NULL;
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}

struct tnode *makeOperatorNode(int op, struct tnode *l, struct tnode *r){
	//idDeclarationCheck(l);
	//idDeclarationCheck(r);
	//dataTypeCheck(l, r, tSearch("integer")->index);
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	if (op == GT | op == LT | op == LE | op == GE | op == EQ | op == NE)
		temp->dataType = tSearch("boolean")->index;
	else
		temp->dataType = tSearch("integer")->index;
	temp->name = NULL;
	temp->nodeType = op;
	temp->val = -1;
	temp->expr = NULL;
	temp->left = l;
	temp->right = r;
	return temp;
}

struct tnode *makeID(char *id){
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->name = (char*)malloc(sizeof(char));
	strcpy(temp->name, id);
	temp->nodeType = ID;
	temp->expr = NULL;
	temp->left = NULL;
	temp->right = NULL;
	temp->lEntry = lSearch(id);
	if (temp->lEntry != NULL){
		temp->dataType = temp->lEntry->type;
		temp->passByRef = temp->lEntry->bindingType;
		temp->gEntry = NULL;
	}
	else{
		temp->gEntry = gSearch(id);
		if (temp->gEntry != NULL){
			temp->dataType = temp->gEntry->type;
		}
	}
	return temp;
}

struct tnode *makeAssgNode(struct tnode *l, struct tnode *r){
	if (r->dataType == VOID)
		dataTypeCheck(l, r, VOID);
	else
		dataTypeCheck(l, r, 0);
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->dataType = VOID;
	temp->nodeType = ASSG;
	temp->name = NULL;
	temp->left = l;
	temp->right = r;
	return temp;
}

struct tnode *makeBooleanNode(int op, struct tnode *l, struct tnode *r){
	if (op != NOT)
		dataTypeCheck(l, r, tSearch("boolean")->index);
	else
		dataTypeCheck(l, NULL, tSearch("boolean")->index);
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->dataType = tSearch("boolean")->index;
	temp->nodeType = op;
	temp->boolVal = -1;
	temp->left = l;
	temp->right = r;
	return temp;
}

struct tnode *makeIONode(int op, struct tnode *node){ //Using t->expr not t->left or t->right
	//dataTypeCheck(node, NULL, tSearch("integer")->index);
	tnode *temp;
	temp = (struct tnode*)malloc(sizeof(tnode));
	temp->nodeType = op;
	temp->expr = node;
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}

struct tnode *makeConditionalNode(tnode *expr, tnode *thenPart, tnode *elsePart){
	dataTypeCheck(expr, NULL, tSearch("boolean")->index);
	tnode *temp;
	temp = (tnode*)malloc(sizeof(tnode));
	temp->nodeType = IF;
	temp->expr = expr;
	temp->left = thenPart;
	temp->right = elsePart;
	return temp;
}

struct tnode *makeIterativeNode(tnode *expr, tnode *slist){
	dataTypeCheck(expr, NULL, tSearch("boolean")->index);
	tnode *temp;
	temp = (tnode*)malloc(sizeof(tnode));
	temp->nodeType = WHILE;
	temp->expr = expr;
	temp->left = slist;
	temp->right = NULL;
	return temp;
}
/*
void evaluate(struct tnode *t){
	if (t->nodeType == NUM){ 														//For numbers
		return;
	}
	else if(t->nodeType == ID){ 												//For identifiers
		if (t->gEntry == NULL){
			printf("Line: %d :: Variable %s is not declared\n", lineNo, t->name);
			exit(1);
		}
		int offset = 0;
		if (t->expr != NULL){
			evaluate(t->expr);
			offset = t->expr->val;
		}
		if (t->dataType == boolean)
			t->boolVal = memory[t->gEntry->binding+offset];
		else
			t->val = memory[t->gEntry->binding+offset];
	}
	else if (t->nodeType == ASSG){ 											//For Assignments
		int offset = 0;
		evaluate(t->left);
		if (t->left->expr != NULL){ //Checking if ID is array or variable
			evaluate(t->left->expr);
			offset = t->left->expr->val;
		}
		evaluate(t->right);
		if (t->left->dataType == boolean)
			memory[t->left->gEntry->binding+offset] = t->right->boolVal;
		else
			memory[t->left->gEntry->binding+offset] = t->right->val;
	}
	else if (t->nodeType == READ){											//For Read Statement
		int offset = 0;
		if (t->expr->expr != NULL){
			evaluate(t->expr->expr);
			offset = t->expr->expr->val;
		}
		scanf("%d", &memory[t->expr->gEntry->binding + offset]);
	}
	else if (t->nodeType == WRITE){											//For Write Statement
		evaluate(t->expr);
		printf("%d\n", t->expr->val);
	}
	else if (t->nodeType == IF){												//For IF Statement
		evaluate(t->expr);
		if (t->expr->boolVal == TRUE){
			evaluate(t->left);
		}
		else{
			if (t->right != NULL)
				evaluate(t->right);
		}
	}
	else if (t->nodeType == WHILE){											//For While Statement
		evaluate(t->expr);
		while(t->expr->boolVal == TRUE){
			evaluate(t->left);
			evaluate(t->expr);
		}
	}
	else if (t->nodeType == STMT){											//For handling Statement List
		while (t != NULL){
			evaluate(t->expr);
			t = t->left;
		}
	}
	else{																								//For arithmetic and relational operators
		int k, p, q;
		switch (t->nodeType){
			case PLUS:
				evaluate(t->left);
        		evaluate(t->right);
        		t->val = t->left->val + t->right->val;
				break;
			case SUB:
		        evaluate(t->left);
		        evaluate(t->right);
		        t->val = t->left->val - t->right->val;
		        break;
			case MUL:
		        evaluate(t->left);
		        evaluate(t->right);
		        t->val = t->left->val * t->right->val;
		        break;
			case DIV:
		        evaluate(t->left);
		        evaluate(t->right);
		        t->val = t->left->val / t->right->val;
		        break;
			case LT:
		        evaluate(t->left);
		        evaluate(t->right);
		        k = t->left->val < t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case GT:
		        evaluate(t->left);
		        evaluate(t->right);
		        k = t->left->val > t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case LE:
				evaluate(t->left);
				evaluate(t->right);
				k = t->left->val <= t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
				break;
			case GE:
		        evaluate(t->left);
		        evaluate(t->right);
		        k = t->left->val >= t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case EQ:
		        evaluate(t->left);
		        evaluate(t->right);
		        k = t->left->val == t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case NE:
		        evaluate(t->left);
		        evaluate(t->right);
		        k = t->left->val != t->right->val;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case AND:
		        evaluate(t->left);
		        evaluate(t->right);
		        if (t->left->boolVal == TRUE)
					p = 1;
				else
					p = 0;
				if (t->right->boolVal == TRUE)
					q = 1;
				else
					q = 0;
				k = p && q;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
		        break;
			case OR:
		        evaluate(t->left);
		        evaluate(t->right);
				if (t->left->boolVal == TRUE)
					p = 1;
				else
					p = 0;
				if (t->right->boolVal == TRUE)
					q = 1;
				else
					q = 0;
				k = p || q;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
        		break;
			case NOT:
        		evaluate(t->left);
				if (t->left->boolVal == TRUE)
					p = 1;
				else
					p = 0;
				k = !p;
				if (k == 1)
					t->boolVal = TRUE;
				else
					t->boolVal = FALSE;
        		break;
			default:
				printf("Error in evaluation\n");
				exit(0);
		}
	}
	return;
}
*/
