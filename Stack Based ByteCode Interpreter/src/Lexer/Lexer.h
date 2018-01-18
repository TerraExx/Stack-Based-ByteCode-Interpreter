/*
 * Lexer.h
 *
 *  Created on: 30.11.2017
 *      Author: posavi
 */

#ifndef LEXER_H_
#define LEXER_H_

#define KEY_WORD_MAX_LENGHT     32
#define VAR_NAME_MAX_LENGHT     32

typedef enum
{
	/* Mnemonics */
    POP_T,
    ICONST_T,
    CALL_T,
    LOAD_T,
    STORE_T,
    RET_T,
    ILT_T,
    ILTE_T,
    IEQ_T,
    IGTE_T,
    IGT_T,
    JMP_T,
    JMPT_T,
    JMPF_T,
    IADD_T,
    ISUB_T,
    IMUL_T,
    IDIV_T,
    PRINT_T,
    HALT_T,
    /* KeyWords */
	DEF,
	ARGS,
	LOCALS,
    KEYWORD_NUM,

	/* Other Tokens */
    ID,
	INTEGER_CONST_T,
	REAL_CONST_T,
	ASSIGN,
	COMMA_T,
	COLON,
	END_OF_FILE,
	TOKEN_TYPE_NUM
} e_lexer_token_type;

typedef struct
{
    uint16_t        line;
	e_lexer_token_type	type;
	union
	{
		int32_t	    integer_const;
		float       float_const;
		char        string[VAR_NAME_MAX_LENGHT];
	} value;
} s_lexer_token;

typedef struct
{
    uint8_t         string[KEY_WORD_MAX_LENGHT];
    s_lexer_token   keyword_token;
} s_lexer_keyword;

typedef struct
{
	uint8_t*	text;
	uint8_t*    textStart;
	uint16_t	current_Pos;
	uint8_t*	current_Char;
	uint16_t    line;
} s_lexer_lexer;

extern uint8_t TokenTypeString[TOKEN_TYPE_NUM][KEY_WORD_MAX_LENGHT];

void Lexer_Init(s_lexer_lexer* Lexer, uint8_t* text);

uint8_t Lexer_Peek( uint8_t* tokenText, s_lexer_lexer* Lexer );

s_lexer_token Lexer_GetNextToken(s_lexer_lexer* Lexer);

#endif /* LEXER_H_ */
