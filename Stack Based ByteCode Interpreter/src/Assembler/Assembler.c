
#include "System.h"
#include "Lexer.h"
#include "Assembler.h"
#include "Interpreter.h"

void Assembler_Init(s_assembler_parser* Assembler, s_lexer_lexer* Lexer )
{
    Assembler->lexer = Lexer;
    Assembler->current_token = Lexer_GetNextToken(Lexer);

    Assembler->program = (int8_t*)calloc(1,CODE_MAX_SIZE);
    Assembler->programIdx = 0;

    Assembler->stackFrames = (s_assembler_stackFrame*)calloc(1,STACK_FRAME_ARRAY_MAX_NUM*sizeof(s_assembler_stackFrame));
    Assembler->stackFrameIdx = 0;



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

void Assembler_Parse(s_assembler_parser* Assembler)
{
    while( Assembler->current_token.type != END_OF_FILE )
    {
        switch( Assembler->current_token.type )
        {
        case POP_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = POP_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, POP_T );
            break;

        case ICONST_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = ICONST_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, ICONST_T );

            /* Add Operand */
            *((int32_t*)(Assembler->program + Assembler->programIdx)) = Assembler->current_token.value.integer_const;
            Assembler->programIdx += 4;
            Assembler_Eat( Assembler, INTEGER_CONST_T );
            break;

        case CALL_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = CALL_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, CALL_T );

            /* Add Operand */
            *((int32_t*)(Assembler->program + Assembler->programIdx)) = Assembler->current_token.value.integer_const;
            Assembler->programIdx += 4;
            Assembler_Eat( Assembler, INTEGER_CONST_T );
            break;

        case LOAD_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = LOAD_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, LOAD_T );

            /* Add Operand */
            *((int32_t*)(Assembler->program + Assembler->programIdx)) = Assembler->current_token.value.integer_const;
            Assembler->programIdx += 4;
            Assembler_Eat( Assembler, INTEGER_CONST_T );
            break;

        case STORE_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = STORE_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, STORE_T );

            /* Add Operand */
            *((int32_t*)(Assembler->program + Assembler->programIdx)) = Assembler->current_token.value.integer_const;
            Assembler->programIdx += 4;
            Assembler_Eat( Assembler, INTEGER_CONST_T );
            break;

        case RET_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = RET_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, RET_T );
            break;

        case IADD_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = IADD_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, IADD_T );
            break;

        case ISUB_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = ISUB_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, ISUB_T );
            break;

        case PRINT_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = PRINT_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, PRINT_T );
            break;

        case HALT_T:
            /* Add OpCode */
            *(Assembler->program + Assembler->programIdx) = HALT_T;
            Assembler->programIdx++;
            Assembler_Eat( Assembler, HALT_T );
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

        default:
            break;
        }
    }
}
