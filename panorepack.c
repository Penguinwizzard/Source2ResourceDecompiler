#include "stupidvalve.h"
#include "crc32.h"
#include <stdio.h>

int main(int argc, char** argv) {
	if(argc < 4) {
		fprintf(stderr,"Usage: ./panorepack oldfile newcontents outputfile\n");
		exit(1);
	}
	filedata* fd = loadfile(argv[1]);
	if(fd->filetype != SVF) {
		fprintf(stderr,"Error: oldfile must be valid SVF file\n");
		exit(4);
	}
	char* newcontents;
       	uint64_t newlen = readintobuf(argv[2], &newcontents);
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
	fclose(of);
	free(newbuff);
	free(newcontents);
	fd_free(fd);
}
