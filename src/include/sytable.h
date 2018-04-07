/**
 * @file sytable.h
 * @author Sayed Sadeed
 * @brief Module defintions for Symbol Table functionality.
 */

#ifndef SYTABLE_H
#define SYTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SYTABLE_SIZE 100

/**
 * Individual Symbol pertaining to source code.
 */
typedef struct {
	char *name;
	char *val;
	unsigned int refs;
} Symbol;

/**
 * @brief SymbolTable which stores collection of symbols.
 */
typedef struct {
	Symbol **symbols;
	size_t sym_cap;
	size_t sym_ctr;
} SyTable;

#endif
