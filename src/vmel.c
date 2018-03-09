#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "vmel.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(1);
	}

	char *b = file_to_buffer(argv[1]);
	build_tokens(b);	
}

void print_usage(void) {
	printf("Usage: vmel [config_file <run.vml>]\n");
}
