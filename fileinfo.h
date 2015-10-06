#ifndef FILEINFO_H
#define FILEINFO_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef enum {
	UNKNOWN = 0,
	UNK_DMX,
	VPK,
	VMAP_C,
	SVF,
	GNV,
	VCS,
	KV_CLOTH,
	BSP,    	//Source 1, but w/e, let's id it at least
	VBKV		//Very rarely used
} filetype_t;

typedef struct {
	char* contents;
	uint64_t length;
	filetype_t filetype;
	void* parsed;
	void* parsed_object;
} filedata;

uint64_t readintobuf(char* filename, char** out);
filedata* loadfile(char* filename);
void parse(filedata* fd);
void fd_free(filedata* fd);

#include "stupidvalve.h"
#include "vcs.h"

#endif
