#ifndef FILEINFO_H
#define FILEINFO_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
	UNKNOWN = 0,
	UNK_DMX,
	VPK,
	VMAP_C,
	SVF,
	GNV,
	VCS,
	KV_CLOTH,
	BSP    //Source 1, but w/e, let's id it at least
} filetype_t;

typedef struct {
	char* contents;
	uint64_t length;
	filetype_t filetype;
	void* parsed;
	void* parsed_object;
} filedata;

filedata* loadfile(char* filename);
void parse(filedata* fd);

#include "stupidvalve.h"

#endif
