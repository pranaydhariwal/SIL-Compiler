int regNo = 0; //For numbering the registers being used for Machine code generation
int label = 0; //For numbering the labels used during branching

FILE *filePtr; //File pointer for printing the Machine code after its generation

int getRegNo(); //Increments reg no and sends it back

int freeReg(); //Decrements reg no, so the highest register number is always deallocated

int generateLabel(); //Increments label number. In a program, same label number would probably not be used

int opCodeGen(tnode *node); //Code Generation for all operations and expressions commands

int stCodeGen(tnode *node); //Code Generation for all statements(IF, WHILE, ASSG, READ, WRITE, STMT)

void funcCodeGen(tnode *node);

void codeGen(tnode *node); //Driving function for code generation. Opens up the filePtr and calls stCodeGen then closes the File
