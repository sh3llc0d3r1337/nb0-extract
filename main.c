#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "nb0.h"


int main(int argc, char *argv[])
{
	int r;
	if (argc<3) {
		printf("nb-extract <nb0 file> <output folder>");
		return 0;
	}

	r = mkdir(argv[2],  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (r!=0 && errno!=EEXIST) {
		fprintf(stderr, "ERROR creating directory '%s'\n", argv[2]);
		return -1;
	}

	if (extract_nb0(argv[1], argv[2])!=0) {
		free_nb0();
		fprintf(stderr, "ERROR: failed extracting nb0 file\n");
		return -1;
	}

	free_nb0();

	return 0;
}
