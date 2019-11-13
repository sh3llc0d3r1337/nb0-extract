#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nb0.h"

int nb0_count;
nb0_header_item *nb0_headers;

static long read_dword(FILE *f)
{
	unsigned int a,b,c,d;
	unsigned long result;
	a = getc(f);
	b = getc(f);
	c = getc(f);
	d = getc(f);
	result = a | b << 8 | c << 16 | d << 24;
	return result;	
}

void write_dword(FILE *f, long l)
{
	fputc(l & 0xff, f);
	fputc((l >> 8) & 0xff, f);
	fputc((l >> 16) & 0xff, f);
	fputc((l >> 24) & 0xff, f);	
}


static int read_header(nb0_header_item *header, FILE *f)
{
	unsigned int i = 0;

	header->offset = read_dword(f);
	header->size = read_dword(f);
	//ignore these
	read_dword(f);
	read_dword(f);
	fread(header->name, 48, 1, f);
	header->name[48] = '\0';
	i = 48;
	while (i>=0) {
		if (header->name[i]==' ') {
			header->name[i]='\0';
		} else {
			break;
		}
	}

	return 0;

}

static char buffer[1024*1024];

int extract_nb0(const char *filename, const char *extract_to)
{
	char buf[FILENAME_MAX];
	FILE *f;
	FILE *listfile = 0;
	unsigned int i;
	unsigned int size;
	unsigned long fsize;
	unsigned long lastoffset;

	f = fopen(filename, "r");

	if (!f) {
		fprintf(stderr, "error opering file %s\n", filename);
		return -1;
	}

	if (extract_to) {
		sprintf(buf, "%s/%s", extract_to, "list");
		listfile = fopen(buf,  "w");
		if (!listfile) {
			fprintf(stderr, "ERROR can not create 'list' file\n");
			fclose(f);
			return -1;
		}
	}	

	fseek(f, 0, SEEK_END);

	fsize = ftell(f);

	rewind(f);
	
	size = read_dword(f);


	if (size>(fsize/64)) {
		fprintf(stderr, "ERROR invalid nb0 file\n");
		fclose(f);
		return -1;
	}

	nb0_count = size;

	printf("File count: %d\n", size);

	nb0_headers = (nb0_header_item *)malloc(sizeof(nb0_header_item)*size);


	lastoffset = 0;

	for (i = 0; i < size; i++) {
		nb0_header_item *nb;
		read_header(&nb0_headers[i], f);		
		nb = &nb0_headers[i];

		if (feof(f) || (nb->offset < lastoffset)) {
			fprintf(stderr, "ERROR invalid nb0 file\n");
			free_nb0();
			fclose(f);
			return -1;
		}

		printf("offset = %08lx size = %08lx name: '%s'\n", nb->offset, nb->size, nb->name);
	}

	fseek(f, 64*size + 4, SEEK_SET);

	printf("filepos %ld\n", ftell(f));

	for (i = 0; i < size; i++) {
		unsigned int sz;
		FILE *out = 0;
		nb0_header_item *nb = &nb0_headers[i];
		nb->nb0_file_offset = ftell(f);

		if (extract_to) {
			sprintf(buf, "%s/%s", extract_to, nb->name);
			printf("extracting to %s (%ld bytes)\n", buf, nb->size);
			fprintf(listfile, "%s\n", nb->name);
			out = fopen(buf, "w");
			if (!out) {
				fclose(f);
				fprintf(stderr, "ERROR can not create output file '%s'\n", buf);
				free_nb0();
				return -1;
			}
		}
		/*we will read the whole file even though we don't
		  extract it.  This is to make sure that we can really
		  read the file.
		 */
		
		sz = nb->size;

		while (sz>0) {

			unsigned int toread, rd;

			if (feof(f)) {
				fclose(f);
				fprintf(stderr, "ERROR unexpected end of file\n");
				free_nb0();
				if (listfile) 
					fclose(listfile);
				return -1;
			}

			toread = sz > sizeof(buffer)?sizeof(buffer):sz;

			rd = fread(buffer, 1, toread, f);

			if (rd<=0)  {
				if (ferror(f)) {
					fclose(f);
					if (listfile) 
						fclose(listfile);
					return -1;
				}
				break;
			}			

			sz -= rd;

			if (extract_to) {
				fwrite(buffer, 1, rd, out);
			}
		}
		if (out)
			fclose(out);
	}

	fclose(f);
	if (listfile)
		fclose(listfile);

	return 0;
}

void free_nb0()
{
	free(nb0_headers);
	nb0_count = 0;
	nb0_headers = 0;
}

