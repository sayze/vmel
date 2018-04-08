#include "sytable.h"

SyTable *SyTable_new() {
	SyTable *sy_table = malloc(sizeof(SyTable));
	sy_table->sym_cap = INIT_SYTABLE_SIZE;
	sy_table->sym_ctr = 0;
	sy_table->symbols = malloc(sy_table->sym_cap * sizeof(Symbol *));
	return sy_table;
}

void SyTable_free(SyTable *sy_table) {
	if (!sy_table)
		return;
	for (size_t i = 0; i < sy_table->sym_ctr; i++) {
		free(sy_table->symbols[i]);
	}
	free(sy_table->symbols);
	free(sy_table);
}

Symbol *Symbol_new(void) {
	Symbol *sy = malloc(sizeof(Symbol));
	sy->name = NULL;
	return sy;
}

Symbol *SyTable_get_symbol(SyTable *sy_table, char *sy_name) {
	if (!sy_table)
		return NULL;

	for (size_t idx = 0; idx < sy_table->sym_ctr; idx++ ) {
			if (strcmp(sy_table->symbols[idx]->name, sy_name) == 0) {
				return sy_table->symbols[idx];
			}
	}
	return NULL;
}

int SyTable_add_symbol(SyTable *sy_table, Symbol *symbol) {
	if (sy_table->sym_cap - sy_table->sym_ctr <= 5) {
		Symbol **sy_new = grow_sy_table(sy_table);
		if (!sy_new)
			return 1;
		sy_table->symbols = sy_new;
		sy_new = NULL;
	}
	
	// Add symbol and increment counter.
	sy_table->symbols[sy_table->sym_ctr++] = symbol;
	return 0;
}

void SyTable_print_symbols(SyTable *sy_table) {
	printf("--------------------------------------\n");
	printf("** Symbol Table Dump **\n");
	printf("--------------------------------------\n");
	char *t = NULL;
	for (size_t i = 0; i < sy_table->sym_ctr; i++) {
		if (sy_table->symbols[i]->type == E_IDN_TYPE)
			t = "Variable";
		else
			t = "Group Name";

		printf("--> Name : %s | Type: %s  \n", sy_table->symbols[i]->name, t);
	}
}

Symbol **grow_sy_table(SyTable *sy_table) {
	if (!sy_table) 
		return NULL;

    sy_table->sym_cap *= 2;
    Symbol **sy_new = realloc(sy_table->symbols, sizeof(Symbol *) * sy_table->sym_cap);	
    return sy_new;
}
