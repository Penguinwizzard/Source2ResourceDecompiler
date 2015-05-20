#ifndef VCS_H
#define VCS_H
#include "fileinfo.h"

typedef struct {
	uint32_t namelen;
	char* name;
	uint32_t counts[7];
} vcsfile;

typedef struct {
	char name[128];
	uint32_t unknown[30];
} vcsl_1;

typedef struct {
	char name[128];
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
	uint32_t unknown6;
} vcsl_2;

typedef struct {
	char name[128];
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
	uint32_t unknown6;
} vcsl_3;

typedef struct {
	char name[128];
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
	uint32_t unknown6;
} vcsl_4;

typedef struct {
	uint32_t unknown[118];
} vcsl_5;

#define VCSL_6_TYPE_UNKNOWN 0
#define VCSL_6_TYPE_FLOAT 1
#define VCSL_6_TYPE_T 3
#define VCSL_6_TYPE_VECTOR 4

typedef struct {
	char name[128];
	uint32_t type;
	uint32_t unknown2;
	char string_value[64];
	uint32_t unknown[70];
} vcsl_6;

// Note: This is wrong, I just haven't seen this actually get used,
// so until then it's going to stay as this silly stub.
typedef struct {
	char name[12];
	uint32_t value[10];
} vcsl_7;

// Haven't seen this one used either.
/*typedef struct {
	// doesn't encode well as a struct
} vcsl_8; */

typedef struct {
	union {
		uint32_t encoding; // LZMA
		char encoding_aschars[4];
	};
	uint32_t uncompressed_length;
	uint32_t compressed_length;
	uint32_t unknown1;	// Always 0x0400005d?
	uint32_t unknown2;	// Some kind of flags?
} vcsl_9_l;

void parse_vcs(filedata* fd);
void print_vcs(vcsfile* vcs);
void free_vcs(filedata* fd);
#endif
