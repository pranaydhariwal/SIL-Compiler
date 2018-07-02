#define HB_SIZE 16

int freeBlock;
FILE *filePtr;

//Code for Intialize
void initialize(FILE *filePtr){
    filePtr = filePtr;
    fprintf(filePtr, "initialize:\n");
    fprintf(filePtr, "MOV [512], 0\n");
    int i = 0;
    while (i < 512){
        fprintf(filePtr, "MOV [%d], %d\n", i, i+HB_SIZE);
        i = i + HB_SIZE;
    }
    fprintf(filePtr, "MOV [496], -1\n");
    fprintf(filePtr, "RET\n\n");
    return;
}

/* How to do things?
    Check if free memory is left to be allocated
    Check if all memory is freed to be freed more.. Should present error
    Check if memory has been allocated or not to a variable.. If not and then used then it should be error
*/

//Code for alloc()
int alloc(){
    int r1 = getRegNo();
    fprintf(filePtr, "//Alloc statement\nMOV R%d, [512]\nMOV [512], [R%d]\n\n", r1, r1);
    return r1;
}

//Code for free()
int freeMem(int address){
    fprintf(filePtr, "//Free statement\nMOV [R%d], [512]\nMOV [512], R%d\n\n", address, address);
    return 0;
}
