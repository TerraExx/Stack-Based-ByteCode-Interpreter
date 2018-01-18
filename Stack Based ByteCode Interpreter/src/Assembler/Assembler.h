
#ifndef INTERPRETER_ASSEMBLER_H_
#define INTERPRETER_ASSEMBLER_H_

#define CODE_MAX_SIZE   1024
#define STACK_FRAME_ARRAY_MAX_NUM  32
#define STACK_FRAME_LOCAL_MAX   32

typedef struct
{
    uint8_t     name[VAR_NAME_MAX_LENGHT];

    uint8_t     args;
    uint8_t     locals;
    int32_t*    local;

    uint16_t    jumpAdrress;
    uint16_t    retAdrress;
} s_assembler_stackFrame;

typedef struct
{
	s_lexer_lexer*	 lexer;
	s_lexer_token	 current_token;

	int8_t*          program;
	uint8_t          programIdx;

	s_assembler_stackFrame* stackFrames;
	uint8_t          stackFrameIdx;

    uint16_t         startAddress;
    uint8_t          startFrame;
} s_assembler_parser;

extern s_assembler_stackFrame Assembler_StackFrames[STACK_FRAME_ARRAY_MAX_NUM];

void Assembler_Init(s_assembler_parser* Assembler, s_lexer_lexer* Lexer );

void Assembler_Parse(s_assembler_parser* Assembler);

#endif /* INTERPRETER_ASSEMBLER_H_ */
