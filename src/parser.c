#include "parser.h"

#include <ctype.h>
#include <string.h>

#include "dynamic_array.h"

#define TRUE 1
#define FALSE 0

#define COUNT_SIMPLE_KEYWORDS 14
const char* const SIMPLE_KEYWORDS[COUNT_SIMPLE_KEYWORDS] = {"pop",   "swap", "dup",   "deref",

                                                            "print", "err",  "input",

                                                            "+",     "-",    "*",     "/",     "%", "^",

                                                            "exit"};
InstructionType const SIMPLE_KEYWORD_TYPES[COUNT_SIMPLE_KEYWORDS] = {INST_POP,   INST_SWAP, INST_DUP,   INST_DEREF,

                                                                     INST_PRINT, INST_ERR,  INST_INPUT,

                                                                     INST_ADD,   INST_SUB,  INST_MUL,   INST_DIV,
                                                                     INST_MOD,   INST_POW,

                                                                     INST_EXIT};

#define COUNT_END_OF_IF 2
const char* const END_OF_IF[COUNT_END_OF_IF] = {"else", "end"};

#define COUNT_END_OF_WHILE 1
const char* const END_OF_WHILE[COUNT_END_OF_WHILE] = {"end"};

static int32_t _parse_instructions(Instructions* restrict inst, const char* restrict* restrict cursor,
                                   const char* restrict const* restrict end_tokens, int32_t num_end_tokens);

static int _try_parse_comment(const char* restrict* restrict cursor);
static int _try_parse_simple(Instructions* restrict inst, const char* restrict* restrict cursor,
                             const char* restrict keyword, InstructionType type);
static int _try_parse_push(Instructions* restrict inst, const char* restrict* restrict cursor);
static int _try_parse_if(Instructions* restrict inst, const char* restrict* restrict cursor);
static int _try_parse_while(Instructions* restrict inst, const char* restrict* restrict cursor);

static void _next_token(const char* restrict* restrict cursor);
static int _is_delimiter(char character);

Instructions* parse_instructions(const char* content) {
    Instructions* inst = (Instructions*)calloc(1, sizeof(Instructions));

    _parse_instructions(inst, &content, NULL, 0);

    return inst;
}

int32_t _parse_instructions(Instructions* restrict inst, const char* restrict* restrict cursor,
                            const char* restrict const* restrict end_tokens, int32_t num_end_tokens) {
    while (**cursor != '\0') {
        // Break on end tokens
        for (int32_t i = 0; i < num_end_tokens; ++i) {
            if (strncmp(*cursor, end_tokens[i], strlen(end_tokens[i])) == 0) {
                return i;
            }
        }

        if (_try_parse_comment(cursor)) {
            continue;
        }

        if (_try_parse_push(inst, cursor)) {
            continue;
        }

        if (_try_parse_if(inst, cursor)) {
            continue;
        }

        if (_try_parse_while(inst, cursor)) {
            continue;
        }

        // Parse simple keywords last, "-" might catch negative number as subtraction
        int result_simple = 0;
        for (int i = 0; i < COUNT_SIMPLE_KEYWORDS; ++i) {
            result_simple = _try_parse_simple(inst, cursor, SIMPLE_KEYWORDS[i], SIMPLE_KEYWORD_TYPES[i]);
            if (result_simple) {
                break;
            }
        }
        if (result_simple) {
            continue;
        }

        if (_is_delimiter(**cursor)) {
            _next_token(cursor);
            continue;
        }

        assert(FALSE && "Illegal token");
    }

    return -1;
}

int _try_parse_comment(const char* restrict* restrict cursor) {
    if (**cursor == '#') {
        do {
            ++(*cursor);
        } while (**cursor != '#' && **cursor != '\0');

        // Cursor is at ending # -> _next_token consumes the # and delimiter
        _next_token(cursor);
        return TRUE;
    }

    return FALSE;
}

int _try_parse_simple(Instructions* restrict inst, const char* restrict* restrict cursor, const char* restrict keyword,
                      InstructionType type) {
    if (strncmp(*cursor, keyword, strlen(keyword)) == 0) {
        Instruction instruction = {0};
        instruction.type = type;

        da_push(*inst, instruction);
        _next_token(cursor);
        return TRUE;
    }

    return FALSE;
}

int _try_parse_push(Instructions* restrict inst, const char* restrict* restrict cursor) {
    const char* str = *cursor;
    int is_negative = *str == '-';
    if (is_negative) {
        ++str;
    }

    if (isdigit(*str)) {
        int64_t sign = is_negative ? -1 : 1;

        Instruction instruction = {0};
        instruction.type = INST_PUSH;
        instruction.context.push_instruction.literal = sign * atoll(str);

        da_push(*inst, instruction);
        _next_token(cursor);
        return TRUE;
    }

    return FALSE;
}

int _try_parse_if(Instructions* restrict inst, const char* restrict* restrict cursor) {
    if (strncmp(*cursor, "if", 2) == 0) {
        _next_token(cursor);  // Consumes "if"

        Instruction instruction = {0};
        instruction.type = INST_IF;

        int32_t index_end_token = _parse_instructions(&instruction.context.if_instruction.if_block, cursor, END_OF_IF, COUNT_END_OF_IF);

        if (index_end_token == 0) {     // else occured
            _next_token(cursor);  // Consumes "else"
            index_end_token = _parse_instructions(&instruction.context.if_instruction.else_block, cursor, END_OF_IF, COUNT_END_OF_IF);
        }

        assert(index_end_token == 1 && "If condition isnt closed correctly");

        da_push(*inst, instruction);
        _next_token(cursor);  // Consumes "end"
        return TRUE;
    }

    return FALSE;
}

int _try_parse_while(Instructions* restrict inst, const char* restrict* restrict cursor) {
    if (strncmp(*cursor, "while", 5) == 0) {
        _next_token(cursor);  // Consumes "while"

        Instruction instruction = {0};
        instruction.type = INST_WHILE;

        int32_t index_end_token = _parse_instructions(&instruction.context.while_instruction.inner_block, cursor, END_OF_WHILE, COUNT_END_OF_WHILE);
        assert(index_end_token == 0 && "While loop isnt closed correctly");

        da_push(*inst, instruction);
        _next_token(cursor);  // Consumes "end"
        return TRUE;
    }

    return FALSE;
}

void _next_token(const char* restrict* restrict cursor) {
    // consume current token
    for (; **cursor != '\0'; ++(*cursor)) {
        if (_is_delimiter(**cursor)) {
            break;
        }
    }

    // Consume delimiter
    for (; **cursor != '\0'; ++(*cursor)) {
        if (!_is_delimiter(**cursor)) {
            break;
        }
    }
}

int _is_delimiter(char character) {
    if (isspace(character)) {
        return TRUE;
    }

    if (character == ',') {
        return TRUE;
    }

    return FALSE;
}
