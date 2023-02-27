#pragma once

#include <stdio.h>
#include <stdbool.h>

#include <executables.h>

#define MAX_OPERANDS 50

bool assemble(FILE* source, FILE* output, executable_format_t format);