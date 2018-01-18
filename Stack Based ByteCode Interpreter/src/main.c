#include "System.h"
#include "Lexer.h"
#include "Assembler.h"
#include "Interpreter.h"

int main(void)
{
    FILE*   FilePtr;
    size_t  FileSize;

    uint8_t*    Text;
    size_t  ReadSize;

    s_lexer_lexer       Lexer;
    s_assembler_parser  Assembler;
    s_vm_vm*            VirtualMachine;

    /* Load Test Script */
    FilePtr = fopen("C:\\Users\\Josip\\eclipse-workspace\\Stack Based ByteCode Interpreter\\src\\Test_Script.txt", "r");

    if(FilePtr == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    fseek(FilePtr, 0, SEEK_END);
    FileSize = ftell(FilePtr);
    rewind(FilePtr);

    Text = (uint8_t*) malloc(FileSize);

    ReadSize = fread((void*)Text, 1, FileSize, FilePtr);
    Text[ReadSize] = 0;
    if(ReadSize == 0)
    {
        printf("File could not be read.\n");
        exit(1);
    }

    /* Init Lexer & Assembler */
    Lexer_Init( &Lexer, Text );
    Assembler_Init( &Assembler, &Lexer );

    /* Run Assembler */
    Assembler_Parse( &Assembler );

    /* Create Virtual Machine */
    VirtualMachine = VM_New( 128, 32, &Assembler );

    /* Run Byte-Code */
    VM_Run( VirtualMachine );
}
