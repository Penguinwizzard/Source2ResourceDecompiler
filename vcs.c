#include "vcs.h"

void parse_vcs(filedata* fd) {
	vcsfile* ret = (vcsfile*)malloc(sizeof(vcsfile));
	uint32_t cur = 0;
	uint32_t type = 0;
	if(*(uint32_t*)(fd->contents+8) == 0) {
		ret->namelen = *(uint32_t*)(fd->contents+12);
		ret->name = fd->contents + 16;
		memcpy(ret->counts,fd->contents + 16 + ret->namelen + 1,sizeof(ret->counts));
		cur = 16 + ret->namelen + 1 + sizeof(ret->counts);
	} else {
		ret->namelen = 0;
		// Skip a bunch of stuff I don't recognize
		cur = 44;
		type = 1;
	}
	// Read type 1 lumps
	if(type==0) {
		uint32_t count1 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 1 count: %u\n",count1);
		uint32_t i;
		for(i=0;i<count1;i++) {
			vcsl_1* thisone = (vcsl_1*)(fd->contents + cur);
			printf("\t%s\n",thisone->name);
			cur += sizeof(vcsl_1);
		}
	}
	// Read type 2 lumps
	{
		uint32_t count2 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 2 count: %u\n",count2);
		uint32_t i;
		for(i=0;i<count2;i++) {
			vcsl_2* thisone = (vcsl_2*)(fd->contents + cur);
			printf("\t%s\n",thisone->name);
			printf("\t\t(unknowns are %.8X %.8X %.8X   %.8X %.8X %.8X)\n",
					thisone->unknown1,
					thisone->unknown2,
					thisone->unknown3,
					thisone->unknown4,
					thisone->unknown5,
					thisone->unknown6);
			cur += sizeof(vcsl_2);
		}
	}
	// Read type 3 lumps
	{
		uint32_t count3 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 3 count: %u\n",count3);
		uint32_t i;
		for(i=0;i<count3;i++) {
			vcsl_3* thisone = (vcsl_3*)(fd->contents + cur);
			printf("\t%s\n",thisone->name);
			printf("\t\t(unknowns are %.8X %.8X %.8X   %.8X %.8X %.8X)\n",
					thisone->unknown1,
					thisone->unknown2,
					thisone->unknown3,
					thisone->unknown4,
					thisone->unknown5,
					thisone->unknown6);
			cur += sizeof(vcsl_3);
		}
	}
	// Read type 4 lumps
	{
		uint32_t count4 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 4 count: %u\n",count4);
		uint32_t i;
		for(i=0;i<count4;i++) {
			vcsl_4* thisone = (vcsl_4*)(fd->contents + cur);
			printf("\t%s\n",thisone->name);
			printf("\t\t(unknowns are %.8X %.8X %.8X   %.8X %.8X %.8X)\n",
					thisone->unknown1,
					thisone->unknown2,
					thisone->unknown3,
					thisone->unknown4,
					thisone->unknown5,
					thisone->unknown6);
			cur += sizeof(vcsl_4);
		}
	}
	// Read type 5 lumps
	{
		uint32_t count5 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 5 count: %u\n",count5);
		uint32_t i;
		for(i=0;i<count5;i++) {
			//vcsl_5* thisone = (vcsl_5*)(fd->contents + cur);
			printf("\tformat unknown\n");
			cur += sizeof(vcsl_5);
		}
	}
	// Read type 6 lumps
	{
		uint32_t count6 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 6 count: %u\n",count6);
		uint32_t i;
		for(i=0;i<count6;i++) {
			vcsl_6* thisone = (vcsl_6*)(fd->contents + cur);
			printf("\t%s : %s\n",thisone->name,thisone->value);
			printf("\t\t(type: %u, unknown: %u)\n",thisone->type,thisone->unknown2);
			cur += sizeof(vcsl_6);
		}
	}
	// Read type 7 lumps
	{
		uint32_t count7 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 7 count: %u\n",count7);
		uint32_t i;
		for(i=0;i<count7;i++) {
			vcsl_7* thisone = (vcsl_7*)(fd->contents + cur);
			printf("\t%s\n",thisone->name);
			cur += sizeof(vcsl_7);
		}
	}
	// Read type 8 lumps
	{
		uint32_t count8 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 8 count: %u\n",count8);
	}
	// Read type 9 lumps
	{
		uint32_t count9 = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		printf("Type 9 count: %u\n",count9);
		// There is an optional set of strings here...
		uint32_t flag = *(uint32_t*)(fd->contents + cur);
		cur += 4;
		if(flag == 1) {
			printf("\tLump leading strings:\n");
			while(*(fd->contents+cur)) {
				printf("\t\t%s\n",fd->contents + cur);
				cur += strlen(fd->contents + cur)+1;
			}
		}
		cur+=4; // skip a zero
		{
			printf("\tunknown0: %u\n",*(uint32_t*)(fd->contents + cur));
			cur += 4;
			printf("\tunknown1: %u\n",*(uint32_t*)(fd->contents + cur));
			cur += 4;
		}
		{
			uint32_t su = *(uint32_t*)(fd->contents + cur);
			printf("\tsemi-unknown?: %u\n",su);
			cur += 4;
			uint32_t j;
			for(j=0;j<su;j++) {
				printf("skipping %u\n",*(uint32_t*)(fd->contents + cur));
				cur += 4;
				printf(" skipping %u\n",*(uint32_t*)(fd->contents + cur));
				cur += 4;
				printf("  skipping %u\n",*(uint32_t*)(fd->contents + cur));
				cur += 4;
			}
		}
		{
			printf("\tunknown0: %u\n",*(uint32_t*)(fd->contents + cur));
			cur += 4;
			printf("\tunknown1: %u\n",*(uint32_t*)(fd->contents + cur));
			cur += 4;
		}
		{
			printf("\tlength of first compressed block (with header): %u\n",*(uint32_t*)(fd->contents + cur));
			cur += 4;
		}
		uint32_t i;
		for(i=0;i<count9;i++) {
			vcsl_9_l* lump = (vcsl_9_l*)(fd->contents + cur);
			cur += sizeof(vcsl_9_l);
			printf("\tCompressed data (compressed with %.4s, compressed length %u, uncompressed length %u\n",lump->encoding_aschars,lump->compressed_length,lump->uncompressed_length);
			printf("\t\t(flags are %.8X %.8X)\n",lump->unknown1,lump->unknown2);
			cur += lump->compressed_length + 1; // +1 for null terminator
		}
	}
	print_vcs(ret);
	fd->filetype = VCS;
	fd->parsed = ret;
}

void print_vcs(vcsfile* vcs) {
	printf("VCS2 File\n");
	printf("Shader Description: %.*s\n",vcs->namelen,vcs->name);
}
void free_vcs(filedata* fd) {
	if(fd->filetype != VCS) {
		fprintf(stderr,"Error: tried to use VCS free on non-VCS data.\n");
		return;
	}
	free(fd->parsed);
	fd->parsed = NULL;
}
