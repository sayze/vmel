#include <stdio.h>
#include <stdlib.h>
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

char *file_to_buffer(const char *filename) {
	// File pointer.
	FILE *fptr = fopen(filename, "r");
	// Output buffer.
	char *buff;

	if (fptr == NULL) {
		perror("Error: ");
		exit(-1);
	}

	fseek(fptr, 0, SEEK_END);
	
	long f_size = ftell(fptr);
	
	fseek(fptr, 0, SEEK_SET);
	
	buff = calloc(1, f_size);
	
	fread(buff, f_size, 1, fptr);
	
	fclose(fptr);
	return buff;
}
