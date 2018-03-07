#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "vmel.h"

// Comment ignore.
#define IGN '#'
// Newline.
#define NWL '\n'

void print_usage(void) {
	printf("Usage: vmel [source <program.vml>]\n");
}

char *file_to_buffer(char filename[]) {
	FILE *fptr = fopen(filename, "r");
	char line[1024];
	char *buff;
	char c;
	int rlock = 0;
	int bidx = 0;

	// Make sure file is good for read.
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
		for (int i = 0; i < strlen(line); i++) {
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

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(1);
	}

	char *buff = file_to_buffer(argv[1]);
	free(buff);
	return 0;
}