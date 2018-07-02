#define STMT 0
#define FUNC 1
#define CALL 2
#define USERDEF 3
#define VOID -1
#define VAR 0
#define ARR 2
#define TRUE 1
#define FALSE 0

#define BASIC_OPS 2

//extern int lineNo = 1; //For printing the line number. Incremented in lex when \n is encountered

/* The node for AST will have these features and can simply be called as tnode */
typedef struct tnode{
	int dataType; //either an int or bool; also denotes if an ID is passs by ref or val
	int nodeType; //can be PLUS, SUB, IF, WHILE, STMT, etc.
	int val; //if the node is integer node, to store the value
	int boolVal; //if the node is boolean node, to store the value
	char *name; //if the node is a variable, to store the variable name
	int passByRef; //TRUE if pass by reference and FALSE if pass by value
	struct tnode *expr; //pointer to another expression (used in IF, WHILE, STMT, READ, WRITE).. Also for function args
						//.. Also for storing argument lists while calling function
	struct tnode *left; //pointer to left node(used in IF, WHILE, boolOP, operator, ASSG, STMT)
	struct tnode *right; //pointer to right node (used in IF, boolOP, operator, ASSG). Also used for return exp for Slist
	struct gTable *gEntry; //for the location in the symbol table entry if node is a variable
	struct lTable *lEntry;
}tnode;

// For making nodes for user defined types
struct tnode *makeUserDefined(tnode *node);

struct tnode *assignType(char *name);

// Function node
// For nodes like Main function
struct tnode *makeFunctionNode(tnode *id, int type, tnode *argList, tnode *body);

// For the nodes that will call the function nodes
struct tnode *makeFunctionCall(tnode *id, tnode *exprList);

// Creates a node for Statement with tnode->expr containing node and tnode->left containing next
// Called from expl.y when a statement is completed
// Error checking not required
struct tnode *makeStatement(struct tnode *node, struct tnode *next);

// Creates a node for leaf node. Can be either integer or boolean type
// Called from expl.l when a integer value or boolean value is found. Also, in expl.y when creating node for (-expr)
// Error checking not required
struct tnode *makeLeaf(int n, int type);

//Creates a node for all operations that deals with integer operands (+, -, *, /, %, <, >, <=, >=, ==, !=)
//tnode->left and tnode->right is used
//Called from expl.y after an expression is formed
//Error-checking is necessary
struct tnode *makeOperatorNode (int op, struct tnode *left, struct tnode *right);

//Creates a node for boolean operations with boolean operands (AND, OR, NOT)
//tnode->left and tnode->right is used
//Called from expl.y after boolean expression is formed
struct tnode *makeBooleanNode (int op, struct tnode *left, struct tnode *right);

//Creates a node for a variable/identifier whenever encountered
//Called from expl.l when a variable is found
struct tnode *makeID(char *id);

//Creates a node for assignment operator where tnode->left and tnode->right are used
//Type checking is to be done
//Called from expl.y when assignment statement is encountered
struct tnode *makeAssgNode(struct tnode *left, struct tnode *right);

//For READ and WRITE statement
//tnode->expr used for arg
//Type checking to see that input is a integer in READ, expression not allowed. Also check boolean is not printed
//Called from expl.y
struct tnode *makeIONode(int op, struct tnode *arg);

//For IF statement
//tnode->expr has expr, tnode->left has thenPart and tnode->right has elsePart
//expr should be boolean, thenPart and elsePart should be STMT or NULL
//Called from expl.y
struct tnode *makeConditionalNode(tnode *expr, tnode *thenPart, tnode *elsePart);

//For WHILE statement
//tnode->expr has expr, tnode->left has slist
//expr should be boolean and slist should be STMT or NULL
//Called from expl.y
struct tnode *makeIterativeNode(tnode *expr, tnode *slist);

//Used while interpreting the code. Not useful after machine code is generated.
//Direct evaluation using Syntax-Directed Translation mechanism
void evaluate(struct tnode *t);
