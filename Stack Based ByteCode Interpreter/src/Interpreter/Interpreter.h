
#ifndef INTERPRETER_INTERPRETER_H_
#define INTERPRETER_INTERPRETER_H_

enum
{
    POP,
    ICONST,
    CALL,
    LOAD,
    STORE,
    RET,
    IADD,
    ISUB,
    PRINT,
    HALT
} e_vm_operationCodes;

typedef struct
{
    s_assembler_stackFrame* stackFrames;

    int8_t*    code;
    int16_t    ip;

    int32_t*   operandStack;
    int16_t    sp;

    s_assembler_stackFrame* callStack;
    int8_t           cp;
} s_vm_vm;

s_vm_vm* VM_New( uint16_t operandStackSize, uint8_t callStackSize, s_assembler_parser* Assembler );

void VM_Run( s_vm_vm* VM );

#endif /* INTERPRETER_INTERPRETER_H_ */
