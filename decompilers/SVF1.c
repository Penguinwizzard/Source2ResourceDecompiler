#include "SVF1.h"

/*
 * Write the needed file.
 */
void SVF1_dectodir(DIR* out, filedata* fd) {
	if(out == NULL) {
		fprintf(stderr, "Error: output directory not specified for SVF1 decompile\n");
		return;
	}
	svffile_generic* ret = fd->parsed;
	if(ret->hdr->version != 1) {
		fprintf(stderr, "Error: Using wrong decompile path\n");
		return;
	}
	uint32_t i;
	for(i=0;i<ret->hdr->lumps.count;i++) {
		if(strncmp(ret->lumps[i].type_aschar,"DATA",4)==0) {
			break;
		}
	}
	if(i==ret->hdr->lumps.count) {
		fprintf(stderr,"Error: No DATA lump found in SVF1 file\n");
		return;
	}
	
}
