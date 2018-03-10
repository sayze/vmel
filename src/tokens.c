#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

// Comment.
#define COMMENT '#'
// Newline.
#define NEWLINE '\n'
// Double quotes.
#define DQUOTE '"'
// Equals operator.
#define EQUAL '='
// End of statement.
#define SEMI ';'
// Left brace.
#define LBRACE '{'
// Right brace.
#define RBRACE '}'

// Token Names
const char *TOKEN_NAMES[] = {
	"EQUAL", "SEMI", "COMMENT", "NEWLINE", "DQUOTE", "LBRACE", "RBRACE"
};

int build_tokens(char *buff, TokenMgr *tokmgr) {
	// Each character in buffer.
	char c;
	// Reusable storage to hold strings. 
	char store[100];
	// Buff iterator.
	int bidx = 0;
	// Track storage size.
	int stctr = 0;
	// Error code.
	int error = 0;
	
	// Iterate through all chars until terminator or error.
	while (buff[bidx] != '\0' && !error) {
		c = buff[bidx];

		if (isspace(c)) {
			bidx++;
			continue;
		}
		else if (c == EQUAL) {
			TokenMgr_add(tokmgr, "OPERATOR", "=");
			bidx++;
		}
		else if (c == DQUOTE) {
			c = buff[++bidx];
			while (c != DQUOTE) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add(tokmgr, "STRING", store);
			stctr = 0;
			bidx++;
		}
		else if (is_valid_identifier(c)) {
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add(tokmgr, "IDENTIFIER", store);
			stctr = 0;
		}
		else if (isdigit( (int) c)) {
			while (isdigit( (int) c)) {
				store[stctr++] = c;
				c = (int) buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add(tokmgr, "INTEGER", store);
			stctr = 0;
		}
		else if (c == LBRACE) {
			c = buff[++bidx];
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add(tokmgr, "GROUP", store);
			stctr = 0;
			bidx++;
		}
		else {
			error = 1;
			printf("Invalid syntax: unknown '%c'\n", c);
		}
	}

	return error;
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
		if (line[0] == COMMENT)
			continue;

		// Iterate through all chars in "line".
		// Strip out remaining inline comments and save to buffer.
		for (size_t i = 0; i < strlen(line); i++) {
			c = line[i];
			switch (c) {
				case COMMENT:
					rlock = 1;
					break;
				case NEWLINE:
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

int is_valid_identifier(char id) {
	if (isalpha(id) || id == '_')
		return 1;
	else
		return 0;
}

TokenMgr *TokenMgr_new() {
	TokenMgr *tok_mgr = malloc(sizeof(TokenMgr));
	tok_mgr->tok_ctr = 0;
	return tok_mgr;
}

int TokenMgr_add(TokenMgr *tok_mgr, char tok_type[50], char tok_val[100]) {
	Token *tmp = malloc(sizeof(Token));
	strcpy(tmp->type, tok_type);
	strcpy(tmp->value, tok_val);
	tok_mgr->toks[tok_mgr->tok_ctr] = tmp;
	tok_mgr->tok_ctr += 1;
	return 0;
}

void TokenMgr_print_tokens(TokenMgr *tok_mgr) {
	for (size_t i =0; i < tok_mgr->tok_ctr; i++) {
		printf("%s %s \n", tok_mgr->toks[i]->type, tok_mgr->toks[i]->value);
	}
}

int TokenMgr_free(TokenMgr *tok_mgr) {
	for (size_t i =0; i < tok_mgr->tok_ctr; i++) {
		free(tok_mgr->toks[i]);
	}
	free(tok_mgr);
	return 0;
}
