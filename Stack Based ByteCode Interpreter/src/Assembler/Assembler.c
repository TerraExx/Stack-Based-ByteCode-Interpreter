
#include "System.h"
#include "Lexer.h"
#include "Assembler.h"
#include "Interpreter.h"

#define ADD_OPCODE( ASM, OpCode )   *(ASM->program + ASM->programIdx) = OpCode; \
                                    ASM->programIdx++;                          \
                                    Assembler_Eat( ASM, OpCode );

#define ADD_OPERAND_INT_32( ASM )  *((int32_t*)(ASM->program + ASM->programIdx)) = ASM->current_token.value.integer_const; \
                                   ASM->programIdx += 4;                                                                   \
                                   Assembler_Eat( ASM, INTEGER_CONST_T );

#define ADD_OPERAND_FLOAT( ASM )  *((float*)(ASM->program + ASM->programIdx)) = ASM->current_token.value.float_const; \
                                   ASM->programIdx += 4;                                                                         \
                                   Assembler_Eat( ASM, REAL_CONST_T );

void Assembler_Init(s_assembler_parser* Assembler, s_lexer_lexer* Lexer )
{
    Assembler->lexer = Lexer;
    Assembler->current_token = Lexer_GetNextToken(Lexer);

    Assembler->program = (int8_t*)calloc(1,CODE_MAX_SIZE);
    Assembler->programIdx = 0;

    Assembler->stackFrames = (s_assembler_stackFrame*)calloc(1,STACK_FRAME_ARRAY_MAX_NUM*sizeof(s_assembler_stackFrame));
    Assembler->stackFrameIdx = 0;

    Assembler->labels = (s_assembler_label*)calloc(1,LABELS_MAX_NUM*sizeof(s_assembler_label));
    Assembler->labelsIdx = 0;

    Assembler->startAddress = 0;
    Assembler->startFrame = 0;
}

void Assembler_LineError(s_assembler_parser* Assembler)
{
    System_Print("Error on line %d:\n", Assembler->lexer->line);
}

s_lexer_token Assembler_Eat(s_assembler_parser* Assembler, e_lexer_token_type Type)
{
	s_lexer_token Token;

	if(Assembler->current_token.type == Type)
	{
	    Assembler->current_token = Lexer_GetNextToken(Assembler->lexer);
	}
	else
	{
	    Assembler_LineError( Assembler );
	    System_Print("Parser Error: Wrong Token Type -> Current_Token: %s, Expected: %s.\n", TokenTypeString[Assembler->current_token.type], TokenTypeString[Type]);
		exit(1);
	}

	return Token;
}

void Assembler_AddLabel(s_assembler_parser* Assembler, uint8_t* Name)
{
    /* Add Label to Array */
    strcpy((char*)(Assembler->labels + Assembler->labelsIdx)->name, (const char*)Name);
    (Assembler->labels + Assembler->labelsIdx)->jumpAdrress = Assembler->programIdx;

    Assembler->labelsIdx++;
}

s_assembler_label* Assembler_GetLabel(s_assembler_parser* Assembler, uint8_t* Name)
{
    s_assembler_label* Label;
    uint8_t Idx;

    /* Init */
    Label = NULL;

    /* Find Label */
    for(Idx = 0; Idx < Assembler->labelsIdx; Idx++)
    {
        if( strcmp((const char*)(Assembler->labels + Idx)->name, (const char*)Name) == 0 )
        {
            Label = (Assembler->labels + Idx);
            break;
        }
    }

    return Label;
}

void Assembler_FindLabels(s_assembler_parser* Assembler)
{
    s_assembler_label*  Label;

    /* Find all Labels */
    while( Assembler->current_token.type != END_OF_FILE )
    {
        switch( Assembler->current_token.type )
        {
        case POP_T:
            Assembler->programIdx++;
            break;

        case ICONST_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case CALL_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case LOAD_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case STORE_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case RET_T:
            Assembler->programIdx++;
            break;

        case ILT_T:
            Assembler->programIdx++;
            break;

        case ILTE_T:
            Assembler->programIdx++;
            break;

        case IEQ_T:
            Assembler->programIdx++;
            break;

        case IGTE_T:
            Assembler->programIdx++;
            break;

        case IGT_T:
            Assembler->programIdx++;
            break;

        case JMP_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case JMPT_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case JMPF_T:
            Assembler->programIdx++;

            Assembler->programIdx+=4;
            break;

        case IADD_T:
            Assembler->programIdx++;
            break;

        case ISUB_T:
            Assembler->programIdx++;
            break;

        case IMUL_T:
            Assembler->programIdx++;
            break;

        case IDIV_T:
            Assembler->programIdx++;
            break;

        case PRINT_T:
            Assembler->programIdx++;
            break;

        case HALT_T:
            Assembler->programIdx++;
            break;

        case ID:
            /* Parse Label */
            if( Lexer_Peek( (uint8_t*)":", Assembler->lexer ) == TRUE )
            {
                Label = Assembler_GetLabel( Assembler, (uint8_t*)Assembler->current_token.value.string );
                if( Label == NULL )
                {
                    /* Add Label to Array */
                    Assembler_AddLabel( Assembler, (uint8_t*)Assembler->current_token.value.string );
                }
            }
            break;

        default:
            break;
        }

        Assembler_Eat( Assembler, Assembler->current_token.type );
    }

    /* Reset Lexer */
    Assembler->lexer->current_Pos = 0;
    Assembler->lexer->line = 1;
    Assembler->lexer->current_Char = Assembler->lexer->textStart;

    /* Reset Assembler */
    Assembler->current_token = Lexer_GetNextToken(Assembler->lexer);
    Assembler->programIdx = 0;
}

void Assembler_Parse(s_assembler_parser* Assembler)
{
    s_assembler_label*  Label;

    /* Find all Labels */
    Assembler_FindLabels( Assembler );

    /* Parser Script */
    while( Assembler->current_token.type != END_OF_FILE )
    {
        switch( Assembler->current_token.type )
        {
        case POP_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, POP_T )
            break;

        case ICONST_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, ICONST_T )

            /* Add Operand */
            ADD_OPERAND_INT_32( Assembler )
            break;

        case F_CONST_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, F_CONST_T )

            /* Add Operand */
            ADD_OPERAND_FLOAT( Assembler )
            break;

        case CALL_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, CALL_T )

            /* Add Operand */
            ADD_OPERAND_INT_32( Assembler )
            break;

        case LOAD_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, LOAD_T )

            /* Add Operand */
            ADD_OPERAND_INT_32( Assembler )
            break;

        case STORE_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, STORE_T )

            /* Add Operand */
            ADD_OPERAND_INT_32( Assembler )
            break;

        case RET_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, RET_T )
            break;

        case ILT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, ILT_T )
            break;

        case ILTE_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, ILTE_T )
            break;

        case IEQ_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IEQ_T )
            break;

        case IGTE_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IGTE_T )
            break;

        case IGT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IGT_T )
            break;

        case JMP_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, JMP_T )

            /* Add Operand */
            if( Assembler->current_token.type == ID )
            {
                Label = Assembler_GetLabel( Assembler, (uint8_t*)Assembler->current_token.value.string );
                *((int32_t*)(Assembler->program + Assembler->programIdx)) = Label->jumpAdrress;
                Assembler->programIdx += 4;
                Assembler_Eat( Assembler, ID );
            }
            else if( Assembler->current_token.type == INTEGER_CONST_T )
            {
                ADD_OPERAND_INT_32( Assembler )
            }
            break;

        case JMPT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, JMPT_T )

            /* Add Operand */
            if( Assembler->current_token.type == ID )
            {
                Label = Assembler_GetLabel( Assembler, (uint8_t*)Assembler->current_token.value.string );
                *((int32_t*)(Assembler->program + Assembler->programIdx)) = Label->jumpAdrress;
                Assembler->programIdx += 4;
                Assembler_Eat( Assembler, ID );
            }
            else if( Assembler->current_token.type == INTEGER_CONST_T )
            {
                ADD_OPERAND_INT_32( Assembler )
            }
            break;

        case JMPF_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, JMPF_T )

            /* Add Operand */
            if( Assembler->current_token.type == ID )
            {
                Label = Assembler_GetLabel( Assembler, (uint8_t*)Assembler->current_token.value.string );
                *((int32_t*)(Assembler->program + Assembler->programIdx)) = Label->jumpAdrress;
                Assembler->programIdx += 4;
                Assembler_Eat( Assembler, ID );
            }
            else if( Assembler->current_token.type == INTEGER_CONST_T )
            {
                ADD_OPERAND_INT_32( Assembler )
            }
            break;

        case IADD_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IADD_T )
            break;

        case ISUB_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, ISUB_T )
            break;

        case IMUL_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IMUL_T )
            break;

        case IDIV_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, IDIV_T )
            break;

        case FADD_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, FADD_T )
            break;

        case PRINT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, PRINT_T )
            break;

        case F_PRINT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, F_PRINT_T )
            break;

        case HALT_T:
            /* Add OpCode */
            ADD_OPCODE( Assembler, HALT_T )
            break;

        case DEF:
            /* Add Function to Stack Frame Array */
            Assembler_Eat( Assembler, DEF );

            if( strcmp((const char*)"main", (const char*)Assembler->current_token.value.string) == 0 )
            {
                Assembler->startFrame = Assembler->stackFrameIdx;
                Assembler->startAddress = Assembler->programIdx;
            }

            strcpy((char*)((Assembler->stackFrames + Assembler->stackFrameIdx))->name, Assembler->current_token.value.string);
            Assembler_Eat( Assembler, ID );

            Assembler_Eat( Assembler, ARGS );
            Assembler_Eat( Assembler, ASSIGN );
            (Assembler->stackFrames + Assembler->stackFrameIdx)->args = Assembler->current_token.value.integer_const;
            Assembler_Eat( Assembler, INTEGER_CONST_T );

            Assembler_Eat( Assembler, LOCALS );
            Assembler_Eat( Assembler, ASSIGN );
            (Assembler->stackFrames + Assembler->stackFrameIdx)->locals = Assembler->current_token.value.integer_const;
            Assembler_Eat( Assembler, INTEGER_CONST_T );

            (Assembler->stackFrames + Assembler->stackFrameIdx)->jumpAdrress = Assembler->programIdx;

            (Assembler->stackFrames + Assembler->stackFrameIdx)->local = (int32_t*)calloc(1,STACK_FRAME_LOCAL_MAX);

            Assembler->stackFrameIdx++;
            break;

        case ID:
            /* Parse Label */
            if( Lexer_Peek( (uint8_t*)":", Assembler->lexer ) == TRUE )
            {
                Assembler_Eat( Assembler, ID );
                Assembler_Eat( Assembler, COLON );
            }
            else
            {
                Assembler_LineError( Assembler );
                exit(1);
            }
            break;

        default:
            Assembler_LineError( Assembler );
            exit(1);
            break;
        }
    }
}
