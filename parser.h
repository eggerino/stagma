#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"

int ignore_line(const char* line);
int parse_line(const char* line, Statement* statement);

#endif
