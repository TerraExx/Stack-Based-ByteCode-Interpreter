
#include "System.h"
#include "Lexer.h"
#include "Assembler.h"
#include "Interpreter.h"

#define PUSH(vm,v) vm->operandStack[++VM->sp] = v
#define POP(vm) vm->operandStack[VM->sp--]

s_vm_vm* VM_New( uint16_t operandStackSize, uint8_t callStackSize, s_assembler_parser* Assembler )
{
    s_vm_vm* VirtualMachine;

    VirtualMachine = (s_vm_vm*)calloc(1,sizeof(s_vm_vm));

    /* Init */
    VirtualMachine->stackFrames = Assembler->stackFrames;

    VirtualMachine->code = Assembler->program;
    VirtualMachine->ip = 0;

    VirtualMachine->operandStack = (int32_t*)calloc(1,operandStackSize);
    VirtualMachine->sp = -1;

    VirtualMachine->callStack = (s_assembler_stackFrame*)calloc(1,callStackSize*sizeof(s_assembler_stackFrame));
    VirtualMachine->cp = 0;

    /* Init VM */
    *(VirtualMachine->callStack + VirtualMachine->cp) = *(VirtualMachine->stackFrames + Assembler->startFrame);
    VirtualMachine->ip = Assembler->startAddress;

    return VirtualMachine;
}

void VM_DebugPrint( s_vm_vm* VM )
{
    uint16_t    Idx;

    /* Print Operand Stack */
    System_Print("opStack [ ");

    for( Idx = 0; Idx < VM->sp + 1; Idx++ )
    {
        System_Print("%d ", *(VM->operandStack + Idx));
    }

    System_Print("]\t");

    /* Print Call Stack */
    System_Print("callStack [ ");

    for( Idx = 0; Idx < VM->cp + 1; Idx++ )
    {
        System_Print("%s ", (VM->callStack + Idx)->name);
    }

    System_Print("]\n");
}

void VM_CPU( s_vm_vm* VM )
{
    int8_t opCode;
    int32_t a,b;
    uint16_t count;

    while(1)
    {
        opCode = VM->code[VM->ip++];

        switch( opCode )
        {
        case ICONST:
            PUSH(VM, *(int32_t*)(VM->code + VM->ip));
            VM->ip += 4;
            break;

        case CALL:
            /* Push Function to Call Stack */
            *(VM->callStack + ++VM->cp) = *(VM->stackFrames + *(int32_t*)(VM->code + VM->ip));
            VM->ip += 4;

            /* Init Parameters */
            for( count = 0; count < (VM->callStack + VM->cp)->args; count++ )
            {
                *((VM->callStack + VM->cp)->local + count) = POP(VM);
            }

            /* Set Return Address */
            (VM->callStack + VM->cp)->retAdrress = VM->ip;

            /* Set Ip to Function Jump Address */
            VM->ip = (VM->callStack + VM->cp)->jumpAdrress;
            break;

        case LOAD:
            PUSH( VM, *((VM->callStack + VM->cp)->local + *(int32_t*)(VM->code + VM->ip)));
            VM->ip += 4;
            break;

        case STORE:
            *((VM->callStack + VM->cp)->local + *(int32_t*)(VM->code + VM->ip)) = POP(VM);
            VM->ip += 4;
            break;

        case RET:
            /* Set Ip to Function Return Address */
            VM->ip = (VM->callStack + VM->cp)->retAdrress;

            /* Pop Function from Call Stack */
            VM->cp--;
            break;

        case IADD:
            b = POP(VM);
            a = POP(VM);
            PUSH(VM, a + b);
            break;

        case ISUB:
            b = POP(VM);
            a = POP(VM);
            PUSH(VM, a - b);
            break;

        case POP:
            a = POP(VM);
            break;

        case PRINT:
            System_Print("%d\n", VM->operandStack[VM->sp]);
            break;

        case HALT:
            exit(1);
            break;
        }

        VM_DebugPrint( VM );
    }
}

void VM_Run( s_vm_vm* VM )
{
    /* Run VMs code */
    VM_CPU( VM );
}
