#include "vcs.h"

void parse_vcs(filedata *fd) {
	vcsfile *ret = (vcsfile *)malloc(sizeof(vcsfile));
	vcs_header *vh = (vcs_header *)(fd->contents);
	uint32_t cur = 0;
	uint32_t type = 0;
	printf("ID %.8X\n", vh->file_identifier);
	if (vh->file_identifier == 0) {
		// This means we're in the features file.
		// This file contains information that identifies the shader.
		ret->namelen = vh->namelen;
		ret->name = fd->contents + 16;
		memcpy(ret->counts, fd->contents + 16 + ret->namelen + 1,
		       sizeof(ret->counts));
		cur = 16 + ret->namelen + 1 + sizeof(ret->counts);
		printf("Parsing as features file...\n");
		printf("COUNTS: %d %d %d %d %d %d %d %d\n", ret->counts[0],
		       ret->counts[1], ret->counts[2], ret->counts[3],
		       ret->counts[4], ret->counts[5], ret->counts[6],
		       ret->counts[7]);
	} else {
		ret->namelen = 0;
		// Skip a bunch of stuff I don't recognize
		cur = 44;
		type = 1;
	}
	// Read type 1 lumps
	if (type == 0) {
		uint32_t count1 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		if (count1 == 4)
			count1 = 2;
		printf("Type 1 count: %u\n", count1);
		uint32_t i;
		for (i = 0; i < count1; i++) {
			vcsl_1 *thisone = (vcsl_1 *)(fd->contents + cur);
			printf("\t%s\n", thisone->name);
			cur += sizeof(vcsl_1);
		}
	}
	// Read type 2 lumps
	{
		uint32_t count2 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 2 count: %u\n", count2);
		uint32_t i;
		for (i = 0; i < count2; i++) {
			vcsl_2 *thisone = (vcsl_2 *)(fd->contents + cur);
			printf("\t%s\n", thisone->name);
			printf("\t\t(unknowns are %.8X %.8X %.8X   %.8X %.8X "
			       "%.8X)\n",
			       thisone->unknown1, thisone->unknown2,
			       thisone->unknown3, thisone->unknown4,
			       thisone->unknown5, thisone->unknown6);
			cur += sizeof(vcsl_2);
		}
	}
	// Read type 3 lumps
	{
		uint32_t count3 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 3 count: %u\n", count3);
		uint32_t i;
		for (i = 0; i < count3; i++) {
			cur += 8;
		}
		for (i = 0; i < count3; i++) {
			// vcsl_3* thisone = (vcsl_3*)(fd->contents + cur);
			printf("\tcontents unknown\n");
			cur += sizeof(vcsl_3);
		}
	}
	// Read type 4 lumps
	{
		uint32_t count4 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 4 count: %u\n", count4);
		uint32_t i;
		for (i = 0; i < count4; i++) {
			vcsl_4 *thisone = (vcsl_4 *)(fd->contents + cur);
			printf("\t%s\n", thisone->name);
			printf("\t\t(unknowns are %.8X %.8X %.8X   %.8X %.8X "
			       "%.8X)\n",
			       thisone->unknown1, thisone->unknown2,
			       thisone->unknown3, thisone->unknown4,
			       thisone->unknown5, thisone->unknown6);
			cur += sizeof(vcsl_4);
		}
	}
	// Read type 5 lumps
	{
		uint32_t count5 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 5 count: %u\n", count5);
		uint32_t i;
		for (i = 0; i < count5; i++) {
			// vcsl_5* thisone = (vcsl_5*)(fd->contents + cur);
			printf("\tformat unknown\n");
			cur += sizeof(vcsl_5);
		}
	}
	// Read type 6 lumps
	{
		uint32_t count6 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 6 count: %u\n", count6);
		uint32_t i;
		vcsl_6 *lastone = NULL;
		for (i = 0; i < count6; i++) {
			vcsl_6 *thisone = (vcsl_6 *)(fd->contents + cur);
			printf("\t%s :", thisone->name);
			switch (thisone->type) {
			case VCSL_6_TYPE_T:
			case VCSL_6_TYPE_VECTOR:
				printf(" %s\n", thisone->string_value);
				break;
			case VCSL_6_TYPE_FLOAT:
				printf(" %f\n", thisone->float_value);
				break;
			default:
				printf("\n");
				break;
			}
			printf("\t\t(type: %u, unknown: %u)\n", thisone->type,
			       thisone->unknown2);
			printf("\t\tunknowns:\n");
			uint32_t j;
			for (j = 0; j < 70; j++) {
				if (lastone != NULL &&
				    lastone->unknown[j] !=
				        thisone->unknown[j]) {
					printf("\x1b[31m%u:%.8X\x1b[0m\t", j,
					       thisone->unknown[j]);
				} else {
					printf("%u:%.8X\t", j,
					       thisone->unknown[j]);
				}
			}
			lastone = thisone;
			printf("\n");
			cur += sizeof(vcsl_6);
		}
	}
	// Read type 7 lumps
	{
		uint32_t count7 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 7 count: %u\n", count7);
		uint32_t i;
		for (i = 0; i < count7; i++) {
			vcsl_7 *thisone = (vcsl_7 *)(fd->contents + cur);
			printf("\t%s\n", thisone->name);
			cur += sizeof(vcsl_7);
		}
	}
	// Read type 8 lumps
	{
		uint32_t count8 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 8 count: %u\n", count8);
		uint32_t i;
		for (i = 0; i < count8; i++) {
			vcsl_8 *current = (vcsl_8 *)(fd->contents + cur);
			cur += sizeof(vcsl_8);
			printf("\t%s:\n", current->name);
			uint32_t j;
			for (j = 0; j < current->num_keys; j++) {
				vcsl_8_kv *kv =
				    (vcsl_8_kv *)(fd->contents + cur);
				printf("\t\t%s\n", kv->name);
				cur += sizeof(vcsl_8_kv);
			}
			// I think there's a CRC checksum or something at the
			// end...
			cur += 4;
		}
	}
	// Read type 9 lumps
	{
		uint32_t count9 = *(uint32_t *)(fd->contents + cur);
		cur += 4;
		printf("Type 9 count: %u\n", count9);
		// There is an optional set of strings here...
		/*uint32_t flag = *(uint32_t*)(fd->contents + cur);
		if(flag == 1) {
		        printf("\tLump leading strings:\n");
		        while(*(fd->contents+cur)) {
		                printf("\t\t%s\n",fd->contents + cur);
		                cur += strlen(fd->contents + cur)+1;
		        }
		}*/
		{
			uint32_t j;
			for (j = 0; j < count9; j++) {
				printf("skipping %u\n",
				       *(uint32_t *)(fd->contents + cur));
				cur += 4;
				printf(" skipping %u\n",
				       *(uint32_t *)(fd->contents + cur));
				cur += 4;
			}
			for (j = 0; j < count9; j++) {
				printf("type 2 skip %u\n",
				       *(uint32_t *)(fd->contents + cur));
				cur += 4;
			}
		}
		{
			printf("\timportant value I don't know the purpose of "
			       "yet: %u\n",
			       *(uint32_t *)(fd->contents + cur));
			cur += 4;
			printf("\tlength of first compressed block (with "
			       "header): %u\n",
			       *(uint32_t *)(fd->contents + cur));
			cur += 4;
		}
		uint32_t i;
		for (i = 0; i < count9; i++) {
			vcsl_9_l *lump = (vcsl_9_l *)(fd->contents + cur);
			cur += sizeof(vcsl_9_l);
			printf("\tCompressed data (compressed with %.4s, "
			       "compressed length "
			       "%u, uncompressed length %u\n",
			       lump->encoding_aschars, lump->compressed_length,
			       lump->uncompressed_length);
			printf("\t\t(flags are %.8X %.8X)\n", lump->unknown1,
			       lump->unknown2);
			cur += lump->compressed_length +
			       1; // +1 for null terminator
		}
	}
	print_vcs(ret);
	fd->filetype = VCS;
	fd->parsed = ret;
}

void print_vcs(vcsfile *vcs) {
	printf("VCS2 File\n");
	printf("Shader Description: %.*s\n", vcs->namelen, vcs->name);
}
void free_vcs(filedata *fd) {
	if (fd->filetype != VCS) {
		fprintf(stderr,
		        "Error: tried to use VCS free on non-VCS data.\n");
		return;
	}
	free(fd->parsed);
	fd->parsed = NULL;
}
