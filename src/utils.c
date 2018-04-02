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
	char *buff = NULL;
	// File size;
	long f_size = 0;

	if (fptr == NULL) {
		perror("Error: ");
		exit(-1);
	}

	fseek(fptr, 0, SEEK_END);
	
	f_size = ftell(fptr);
	
	fseek(fptr, 0, SEEK_SET);
	
	if (f_size > 0) {
		buff = calloc(1, f_size+1);	
		fread(buff, f_size, 1, fptr);
	}
		
	fclose(fptr);
	return buff;
}

int string_to_int(char *str, size_t len) {
	if (str == NULL)
		return 0;
		
	int dec = 0;
	for(size_t i =0; i<len; i++){
		dec = dec * 10 + ( str[i] - '0' );
	}
	return dec;

}

char *string_map_vars(const char *src, char **vars, size_t src_len, size_t vars_len) {
	if (src == NULL || vars == NULL)
		return NULL;
	
	if (src_len < 1 || vars_len < 1 )
		return NULL;
	
	// Biggest possible size for output string.
	size_t upper_n = 0;
	// Number of variable chars in source.
	int var_ct = 0;
	// Iterators.
	size_t src_i;
	size_t vars_i;

	// Find the number of variables in src. "@"*.
	for (src_i = 0; src_i < src_len; src_i++) {
		if (src[src_i] == '@' && isdigit(src[src_i+1]))
			var_ct++;
	}

	for (vars_i = 0; vars_i < vars_len; vars_i++) {
		upper_n += strlen(vars[vars_i]);
	}

	// Return null if no subs can be made. @ not there.
	// Or there are no substitute variables.
	if (var_ct == 0 || upper_n == 0)
		return NULL;
	
	upper_n += var_ct + src_len;

	char *new_str = malloc(sizeof(char) * upper_n);
	src_i = 0;
	vars_i = 0;
	size_t vars_pos = 0;
	size_t new_str_i = 0;
	while (src_i < src_len) {
		if (src[src_i] == '@' && isdigit(src[src_i+1])) {
			vars_pos = src[++src_i] - '0';
			if (vars_pos > vars_len)
				break;
			for (size_t var_w_i = 0; var_w_i < strlen(vars[vars_pos]); var_w_i++) {
				new_str[new_str_i++] = vars[vars_pos][var_w_i];
			}
			src_i++;
		}
		new_str[new_str_i++] = src[src_i++];
	}

	new_str[new_str_i++] = '\0';
	// Resize memory.
	new_str = realloc(new_str, new_str_i * sizeof(char));
	return new_str;
}

int int_to_string(char *out, int src) {
	return sprintf(out, "%d", src);
}
