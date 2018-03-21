#include <stdio.h>
#include "tokenizer.h"
#include "utils.h"	
#include "parser.h"
#include "tokens.h"
#include "tokenizer.h"   

int can_consume(char *tok_type, char *type) {
	return strcmp(tok_type, type);
}
