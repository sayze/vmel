#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vmel.h"

// Comment ignore.
#define IGN '#'
// Newline.
#define NWL '\n'
// Quotes.
#define QT '"'
// Assignment operator.
#define EQ '='
// End of statement.
#define EOS ';'

// Token Names
const char *token_names[] = {
	"EQUAL", "VAR", "NUMBER", "STRING", "SEMI", "COMMENT", "NEWLINE", "DQUOTE"
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(1);
	}

	char *buff = file_to_buffer(argv[1]);

	if (build_tokens(buff) == 0)
		printf("Token went well");

	free(buff);
}

void print_usage(void) {
	printf("Usage: vmel [config_file <run.vml>]\n");
}

char *file_to_buffer(const char *filename) {
	// File pointer.
	FILE *fptr = fopen(filename, "r");
	// Store each line read from file.
	char line[1024];
	// Output buffer.
	char *buff;
	// Each character read in line,
	char c;
	// Read lock to prevent store of comments.
	int rlock = 0;
	// Track index of buffer.
	int bidx = 0;

	if (fptr == NULL) {
		printf("Invalid file provided %s\n", filename);
		exit(1);
	}

	fseek(fptr, 0, SEEK_END);
	long f_size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	buff = calloc(1, f_size);

	while (fgets(line, sizeof(line), fptr)) {
		// Line starts with comment don't bother processing.
		if (line[0] == IGN)
			continue;

		// Iterate through all chars in "line".
		// Strip out remaining inline comments and save to buffer.
		for (size_t i = 0; i < strlen(line); i++) {
			c = line[i];
			switch (c) {
				case IGN:
					rlock = 1;
					break;
				case NWL:
					rlock = 0;
					break;
				default:
					if (rlock == 0) {
						buff[bidx++] = c;
					}
					break;
			}
		}
	}

	buff[bidx] = '\0';
	fclose(fptr);
	return buff;
}

int build_tokens(char *buff) {
	// Each character in buffer.
	char c;
	// Reusable storage to hold strings. 
	char store[30];
	// Buff iterator.
	int i = 0;
	// Track storage size.
	int stctr = 0;
	// Track number of tokens in array.
	int tokctr = 0;
	// Error code.
	int error = 0;
	// Hold array of identified tokens.
	token *tokens[1000]; // TODO make more smarter size allocation.

	// Iterate through all chars until terminator or error.
	while (buff[i] != '\0' && !error) {
		c = buff[i];
		
		// Ignore spaces.
		if (isspace(c)) {
			i++;
			continue;
		}
		else if (c == EQ) {
			token *tmp = malloc(sizeof(token));
			strcpy(tmp->type, "OPERATOR");
			strcpy(tmp->value, "=");
			tokens[tokctr++] = tmp;
			i++;
		}
		else if (c == QT) {
			do {
				store[stctr++] = c;
				c = buff[++i];
			} while ((char) c != EOS);
			
			store[stctr] = '\0';
			token *tmp = malloc(sizeof(token));
			strcpy(tmp->type, "STRING");
			strcpy(tmp->value, store);
			tokens[tokctr++] = tmp;
			stctr = 0;
			i++;
		}
		else if (isalpha(c) || c == '_') {
			while (isalpha(c) || c == '_') {
				store[stctr++] = c;
				c = buff[++i];
			}
			store[stctr] = '\0';
			token *tmp = malloc(sizeof(token));
			strcpy(tmp->type, "IDENTIFIER");
			strcpy(tmp->value, store);
			tokens[tokctr++] = tmp;
			stctr = 0;
			i++;
		}
		else if (isdigit( (int) c)) {
			while (isdigit( (int) c)) {
				store[stctr++] = c;
				c = (int) buff[++i];
			}
			store[stctr] = '\0';
			token *tmp = malloc(sizeof(token));
			strcpy(tmp->type, "INTEGER");
			strcpy(tmp->value, store);
			tokens[tokctr++] = tmp;
			stctr = 0;
			i++;
		}
		else {
			error = 1;
			printf("Invalid syntax: unknown '%c'\n", c);
			i++;
		}
	}

	// Free all malloc'ed tokens
	// Will need when parsing is implemented.
	for (int i =0; i < tokctr; i++) {
		free(tokens[i]);
	}

	return error;
}