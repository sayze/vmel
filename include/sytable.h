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
#include "tokenizer.h"

enum SyType {
	E_GROUP_TYPE, E_INTEGER_TYPE, E_IDN_TYPE, E_STRING_TYPE, E_FUNC_TYPE
};

/**
 * @brief Store relevant token pertaining to symbol entry.
 */
typedef struct {
	char *label;
	char *val;
	unsigned int lineno;
	enum SyType sy_type;
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
 * @param label Name of the symbol.
 * @param val Value stored.
 * @param lineno line number where symbol occurs in source map.
 * @param sy_type Enum to specify symbol type.
 * @return 0 if success or 1 if error.
 */
int SyTable_add_symbol(SyTable *sy_table, char *label, char *val, unsigned int lineno, enum SyType sy_type);

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
 * @brief Get an existing symbol from SyTable instance.
 * 
 * Function can be used to determine if a symbol already exist.
 * 
 * @param sy_table SyTable instance.
 * @param sy_name name of the symbol to return.
 * @return NULL if symbol can't be found otherwise return pointer to matched symbol.
 */
Symbol *SyTable_get_symbol(SyTable *sy_table, char *sy_name);

/**
 * @brief Update the value stored inside a symbol
 *
 * @param sy_table SyTable instance.
 * @param sy_name name of the symbol to return.
 * @return 0 if successfully updated otherwise -1.
 */
int SyTable_update_symbol(SyTable *sy_table, char *sy_name, char *sy_n_value);

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
