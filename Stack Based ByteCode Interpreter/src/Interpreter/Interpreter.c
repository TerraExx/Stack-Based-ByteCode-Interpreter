
#include "System.h"
#include "Lexer.h"
#include "Assembler.h"
#include "Interpreter.h"

#define PUSH(vm, v, type) ((type*)vm->operandStack)[++vm->sp] = v; \
                          strcpy((char*)vm->operandType + (vm->sp * 8), (const char*)#type)

#define POP(vm, type) ((type*)vm->operandStack)[vm->sp--]

s_vm_vm* VM_New( uint16_t operandStackSize, uint8_t callStackSize, s_assembler_parser* Assembler )
{
    s_vm_vm* VirtualMachine;

    VirtualMachine = (s_vm_vm*)calloc(1,sizeof(s_vm_vm));

    /* Init */
    VirtualMachine->stackFrames = Assembler->stackFrames;

    VirtualMachine->code = Assembler->program;
    VirtualMachine->ip = 0;

    VirtualMachine->operandStack = (int32_t*)calloc(1,operandStackSize);
    VirtualMachine->operandType = (uint8_t*)calloc(1,operandStackSize*8);
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
        if( strcmp((const char*)VM->operandType + (Idx * 8), (const char*)"float") == 0 )
        {
            System_Print("%.2f ", *(float*)(VM->operandStack + Idx));
        }
        else
        {
            System_Print("%d ", *(VM->operandStack + Idx));
        }
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

void VM_Call( s_vm_vm* VM )
{
    uint16_t count;

    /* Push Function to Call Stack */
    *(VM->callStack + ++VM->cp) = *(VM->stackFrames + *(int32_t*)(VM->code + VM->ip));
    VM->ip += 4;

    /* Init Parameters */
    for( count = 0; count < (VM->callStack + VM->cp)->args; count++ )
    {
        *((VM->callStack + VM->cp)->local + count) = POP(VM, int32_t);
    }

    /* Set Return Address */
    (VM->callStack + VM->cp)->retAdrress = VM->ip;

    /* Set Ip to Function Jump Address */
    VM->ip = (VM->callStack + VM->cp)->jumpAdrress;
}

void VM_Ret( s_vm_vm* VM )
{
    /* Set Ip to Function Return Address */
    VM->ip = (VM->callStack + VM->cp)->retAdrress;

    /* Pop Function from Call Stack */
    VM->cp--;
}

void VM_CPU( s_vm_vm* VM )
{
    int8_t opCode;
    int32_t a,b;
    float   f,l;

    while(1)
    {
        opCode = VM->code[VM->ip++];

        switch( opCode )
        {
        case ICONST:
            PUSH(VM, *(int32_t*)(VM->code + VM->ip), int32_t);
            VM->ip += 4;
            break;

        case F_CONST:
            PUSH(VM, *(float*)(VM->code + VM->ip), float);
            VM->ip += 4;
            break;

        case CALL:
            VM_Call( VM );
            break;

        case LOAD:
            PUSH( VM, *((VM->callStack + VM->cp)->local + *(int32_t*)(VM->code + VM->ip)), int32_t);
            VM->ip += 4;
            break;

        case STORE:
            *((VM->callStack + VM->cp)->local + *(int32_t*)(VM->code + VM->ip)) = POP(VM, int32_t);
            VM->ip += 4;
            break;

        case RET:
            VM_Ret( VM );
            break;

        case ILT:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a < b, int32_t);
            break;

        case ILTE:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a <= b, int32_t);
            break;

        case IEQ:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a == b, int32_t);
            break;

        case IGTE:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a >= b, int32_t);
            break;

        case IGT:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a > b, int32_t);
            break;

        case JMP:
            VM->ip = *(int32_t*)(VM->code + VM->ip);
            break;

        case JMPT:
            if( POP(VM, int32_t) > 0 )
            {
                VM->ip = *(int32_t*)(VM->code + VM->ip);
            }
            else
            {
                VM->ip += 4;
            }
            break;

        case JMPF:
            if( POP(VM, int32_t) == 0 )
            {
                VM->ip = *(int32_t*)(VM->code + VM->ip);
            }
            else
            {
                VM->ip += 4;
            }
            break;

        case IADD:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a + b, int32_t);
            break;

        case ISUB:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a - b, int32_t);
            break;

        case IMUL:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a * b, int32_t);
            break;

        case IDIV:
            b = POP(VM, int32_t);
            a = POP(VM, int32_t);
            PUSH(VM, a / b, int32_t);
            break;

        case FADD:
            f = POP(VM, float);
            l = POP(VM, float);
            PUSH(VM, f + l, float);
            break;

        case POP:
            a = POP(VM, int32_t);
            break;

        case PRINT:
            System_Print("%d\n", VM->operandStack[VM->sp]);
            break;

        case F_PRINT:
            System_Print("%.2f\n", ((float*)VM->operandStack)[VM->sp]);
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
