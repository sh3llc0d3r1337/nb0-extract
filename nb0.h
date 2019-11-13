#ifndef NB0_H
#define NB0_H

/**
 * returns 0 on success
 * will fill in global variable nb0_count and nb0_headers
 */
int extract_nb0(const char *filename, const char *extract_to);

typedef struct {
	long offset;
	long size;
	char name[49];
	/*additional info for direct reading*/
	long nb0_file_offset;
} nb0_header_item;

void free_nb0();

#endif
