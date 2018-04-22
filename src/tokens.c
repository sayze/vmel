#include <stdio.h>
#include <string.h>
#include "tokens.h"

static const char *Keywords[] = {
	"print", "func", "if", "else", "foreach"
};

int is_valid_keyword(char *str) {
	int ret = 0;
	
	if (!str)
		return ret;
	
	for (int x = 0; x < KWORDS_SIZE; x++) {
		const char *t = Keywords[x];
		if (strcmp(t, str) == 0) {
			ret = 1;
			break;
		}
	}
	return ret;
}
