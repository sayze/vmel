#include "sytable.h"
#include "utils.h"
#include "conf.h"

SyTable *SyTable_new() {
	SyTable *sy_table = malloc(sizeof(SyTable));
	sy_table->sym_cap = INIT_SYTABLE_SIZE;
	sy_table->sym_ctr = 0;
	sy_table->symbols = malloc(sy_table->sym_cap * sizeof(Symbol *));
	return sy_table;
}

void SyTable_free(SyTable *sy_table) {
	if (null_check(sy_table, "sytable free")) return;

	for (size_t i = 0; i < sy_table->sym_ctr; i++) {
		if (sy_table->symbols[i]->sy_type == E_IDN_TYPE && sy_table->symbols[i]->val) {
			free(sy_table->symbols[i]->val);
		}
		free(sy_table->symbols[i]);
	}
	
	free(sy_table->symbols);
	free(sy_table);
}

Symbol *Symbol_new(void) {
	Symbol *sy = malloc(sizeof(Symbol));
	sy->val = NULL;
	return sy;
}

Symbol *SyTable_get_symbol(SyTable *sy_table, char *sy_name) {
	if (null_check(sy_table, "sytable free")) return NULL;
		
	for (size_t idx = 0; idx < sy_table->sym_ctr; idx++ ) {
			if (strcmp(sy_table->symbols[idx]->sy_token->value, sy_name) == 0) {
				return sy_table->symbols[idx];
			}
	}

	return NULL;
}

int SyTable_add_symbol(SyTable *sy_table, Token *sy_tok, enum SyType sy_type) {
	if (null_check(sy_table, "sytable add") || null_check(sy_tok, "sytable add")) 
		return -1;

	if (sy_table->sym_cap - sy_table->sym_ctr <= 5) {
		Symbol **sy_new = grow_sy_table(sy_table);
		if (!sy_new)
			return 1;
		sy_table->symbols = sy_new;
		sy_new = NULL;
	}
	
	// Add symbol and increment counter.
	Symbol *sy = Symbol_new();
	sy->sy_token = sy_tok;
	sy->sy_type = sy_type;
	sy_table->symbols[sy_table->sym_ctr++] = sy;
	sy = NULL;
	return 0;
}

int SyTable_update_symbol(SyTable *sy_table, char *sy_name, char *sy_n_value) {
	if (!sy_table || !sy_name || !sy_n_value) return -1;

	Symbol *sy = SyTable_get_symbol(sy_table, sy_name);
	
	// Does the symbol exist.
	if (!sy)
		return -1;
	
	// Has it already been set
	if (sy->val)
		free(sy->val);

	// Reassign to new value.
	sy->val = string_dup(sy_n_value);
	return 0;
}

void SyTable_print_symbols(SyTable *sy_table) {
	if (null_check(sy_table, "sytable print")) return;

	printf("--------------------------------------\n");
	printf("** Symbol Table Dump **\n");
	printf("--------------------------------------\n");
	char *t = NULL;
	for (size_t i = 0; i < sy_table->sym_ctr; i++) {
		if (sy_table->symbols[i]->sy_type == E_IDN_TYPE)
			t = "Variable";
		else
			t = "Group Name";
		char *sy_val = sy_table->symbols[i]->val ? sy_table->symbols[i]->val : "Undefined";
		printf("--> Name : %s | Type: %s  | Value: %s \n", sy_table->symbols[i]->sy_token->value, t, sy_val);
	}
}

Symbol **grow_sy_table(SyTable *sy_table) {
	if (null_check(sy_table, "sytable grow")) return NULL;
    sy_table->sym_cap *= 2;
    Symbol **sy_new = realloc(sy_table->symbols, sizeof(Symbol *) * sy_table->sym_cap);	
    return sy_new;
}
