#include "stupidvalve.h"
#include "crc32.h"
#include <stdio.h>
#include <stdlib.h>

#define SETOFFS( x , y )  x = ((char*)( y )) - ((char*)&( x ));
#define setOffsetString(a,b) strncpy(pointer, (b), 64); (a) = pointer - ((char*)&(a)); pointer += strlen(b)+1;

uint64_t readintobuf(char* filename, char** out) {
	FILE* file;
	file = fopen(filename, "r");
	if(file == NULL) {
		fprintf(stderr,"Error: unable to open file with name %s\n",filename);
		exit(2);
	}
	fseek(file, 0, SEEK_END);
	uint64_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	*out = (char*)malloc(length*sizeof(char));
	fread(*out, length, 1, file);
	return length;
}

int main(int argc, char** argv) {
	if(argc < 4) {
		fprintf(stderr,"Usage: ./panorepack oldfile newcontents outputfile\n");
		exit(1);
	}
	filedata* fd = loadfile(argv[1]);
	printf("TEST\n");
	if(fd->filetype != SVF) {
		fprintf(stderr,"Error: oldfile must be valid SVF file\n");
		exit(4);
	}
	char* newcontents;
       	uint64_t newlen = readintobuf(argv[2], &newcontents);
	printf("TEST\n");
	svffile_generic* ret = fd->parsed;
	// find the data lump header
	uint32_t i;
	for(i=0;i<ret->hdr->lumps.count;i++) {
		if(strncmp(ret->lumpheaders[i].tag,"DATA",4)==0) {
			break;
		}
	}
	if(i==ret->hdr->lumps.count) {
		fprintf(stderr,"Error: No DATA lump found in input file!\n");
		exit(8);
	}
	uint64_t headerlen = fd->length - ret->lumpheaders[i].length;
	uint64_t newtotallen = headerlen + newlen + 6;
	printf("TEST\n");
	char* newbuff = (char*)calloc(sizeof(char),newtotallen);
	ret->lumpheaders[i].length = newlen + 6;
	memcpy(newbuff, fd->contents, headerlen);
	memcpy(newbuff+headerlen+6, newcontents, newlen);
	*(uint32_t*)(newbuff + headerlen) = crc32(0,newcontents, newlen);
	*(uint32_t*)newbuff = newtotallen;
	FILE* of = fopen(argv[3],"w");
	if(of == NULL) {
		fprintf(stderr,"Error: Unable to open output file %s for writing!\n",argv[3]);
		exit(16);
	}
	fwrite(newbuff, sizeof(char), newtotallen, of);
}
