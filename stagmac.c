#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"

int parse_source_code(const char* filename, Statement** program, int64_t* num_statements) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: [Compiler] Cannot open source file %s.\n", filename);
        return -1;
    }

    char* line = NULL;
    ssize_t chars_read;
    size_t line_len;
    int status = 0;
    *num_statements = 0;
    size_t capacity = 10;
    *program = (Statement*)malloc(sizeof(**program) * capacity);
    if (*program == NULL) {
        fprintf(stderr, "ERROR: [Compiler] Cannot allocate statement array.\n");
        status = -1;
    } else {
        while ((chars_read = getline(&line, &line_len, f)) != -1) {
            if (ignore_line(line)) {
                continue;
            }

            ++(*num_statements);
            if (*num_statements == capacity) {
                size_t new_capacity = 2 * capacity;
                Statement* new_program = (Statement*)malloc(sizeof(**program) * new_capacity);

                if (new_program == NULL) {
                    fprintf(stderr, "ERROR: [Compiler] Cannot reallocate statement array.\n");
                    status = -1;
                    break;
                }

                memcpy(new_program, *program, sizeof(Statement) * capacity);
                free(*program);

                *program = new_program;
                capacity = new_capacity;
            }

            status = parse_line(line, &(*program)[*num_statements - 1]);
            if (status) break;
        }
    }

    free(line);
    fclose(f);
    return status;
}

int write_byte_code(Statement* program, int64_t num_statements, const char* output_filename) {
    FILE* f = fopen(output_filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: [Compiler] Cannot open output file %s.\n", output_filename);
        return -1;
    }

    fwrite(&num_statements, sizeof(num_statements), 1, f);
    fwrite(program, sizeof(*program), num_statements, f);

    fclose(f);
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "ERROR: [Compiler] Compiler option, source file and/or output file is missing.\n");
        return -1;
    }

    Statement* program;
    int64_t num_statements;
    int status = parse_source_code(argv[2], &program, &num_statements);
    if (!status) {
        if (!strcmp(argv[1], "bytecode")) {
            status = write_byte_code(program, num_statements, argv[3]);
        } else {
            fprintf(stderr, "ERROR: [Compiler] Unknwon compiler option %s.\n", argv[1]);
            status = -1;
        }
    }

    free(program);
    return status;
}
