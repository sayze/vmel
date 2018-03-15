#include <stdio.h>
#include <ctype.h>
#include "utils.h"

void print_usage(void) {
	printf("Usage: vmel [config_file <run.vml>]\n");
}

int is_valid_identifier(char id) {
	if (isalpha(id) || id == '_' || id == '-' )
		return 1;
	else
		return 0;
}