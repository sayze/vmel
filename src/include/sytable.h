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

#define INIT_SYTABLE_SIZE 7

enum Sy_Type {
	E_GROUP_TYPE, E_IDN_TYPE, E_FUNC_TYPE
};

/**
 * @brief Individual Symbols pertaining.
 */
typedef struct {
	char *name;
	enum Sy_Type type;
} Symbol;

/**
 * @brief SymbolTable which stores collection of symbols.
 */
typedef struct {
	Symbol **symbols;
	size_t sym_cap;
	size_t sym_ctr;
} SyTable;

/**
 * @brief Create malloc'ed SyTable instance.
 * 
 * @return New instance of SyTable.
 */
SyTable *SyTable_new();

/**
 * @brief Add a symbol to SyTable instance.
 * 
 * @param sy_table Instance of SyTable.
 * @return 0 if success or 1 if error.
 */
int SyTable_add_symbol(SyTable *sy_table, Symbol *symbol);

/**
 * @brief Print contents of SyTable. Useful for debugging.
 * 
 * @param sy_table Pointer to symbol table.
 */
void SyTable_print_symbols(SyTable *sy_table);

/**
 * @brief Free resources used by SyTable instance. 
 * 
 * @param sy_table Pointer to symbol table.
 */
void SyTable_free(SyTable *sy_table);

/**
 * @brief Create malloc'ed instance of Symbol.
 * 
 * @return Symbol pointer.
 */
Symbol *Symbol_new(void);

/**
 * @brief Perform relloc on array of of symbols in SyTable.
 * 
 * This function is to be used internally to allocate more space.
 * 
 * @param sy_table instance of SyTable.
 * @return Newly allocated SyTablee**.
 */
Symbol **grow_sy_table(SyTable *sy_table);

#endif
