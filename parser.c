#include "parser.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _starts_with(const char* str, const char* start);
static int _is_whitespace(const char* str);

int ignore_line(const char* line) {
    if (_starts_with(line, "# ")) {
        return 1;
    }

    if (_is_whitespace(line)) {
        return 1;
    }

    return 0;
}

int parse_line(const char* line, Statement* statement) {
    assert(OP_COUNT == 11 && "Exhaustive pattern matching");
    if (_starts_with(line, "push ")) {
        statement->op = OP_PUSH;
        statement->arg = atoll(&line[5]);
        return 0;
    } else if (_starts_with(line, "pop")) {
        statement->op = OP_POP;
        return 0;
    } else if (_starts_with(line, "dub")) {
        statement->op = OP_DUB;
        return 0;
    } else if (_starts_with(line, "swap")) {
        statement->op = OP_SWAP;
        return 0;
    } else if (_starts_with(line, "jump ")) {
        statement->op = OP_JUMP;
        statement->arg = atoll(&line[5]);
        return 0;
    } else if (_starts_with(line, "add")) {
        statement->op = OP_ADD;
        return 0;
    } else if (_starts_with(line, "sub")) {
        statement->op = OP_SUB;
        return 0;
    } else if (_starts_with(line, "mul")) {
        statement->op = OP_MUL;
        return 0;
    } else if (_starts_with(line, "div")) {
        statement->op = OP_DIV;
        return 0;
    } else if (_starts_with(line, "print")) {
        statement->op = OP_PRINT;
        return 0;
    } else if (_starts_with(line, "debug")) {
        statement->op = OP_DEBUG;
        return 0;
    }

    fprintf(stderr, "ERROR: [Parser] Line\n\t%s does not represent a statement.\n", line);
    return -1;
}

int _starts_with(const char* str, const char* start) {
    while (1) {
        if (*start == '\0') return 1;   // Equality till end of the start string

        if (*str != *start) return 0;   // Strings differ

        ++str;
        ++start;
    }
}

int _is_whitespace(const char* str) {
    while (*str != '\0') {
        if (!isspace(*str)) {
            return 0;
        }
        
        ++str;
    }
    return 1;
}
