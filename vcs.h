#ifndef VCS_H
#define VCS_H
#include "fileinfo.h"

typedef struct {
	union {
		char tag_aschar[4];
		uint32_t tag;
	};
	uint32_t unknown;		// Always 3E - Version info? 
	uint32_t file_identifier;	// Used for identifying vcs files. This value
					// is generally 0 in the features file of some
					// set of shader files, and then some distinct
					// value in the rest, which is then referenced
					// in the features file itself.
	// From here it changes...
	uint32_t namelen;		// ONLY OCCURS IN THE FEATURES FILES (i.e. previous value is 0)
} vcs_header;

typedef struct {
	uint32_t namelen;
	char* name;
	uint32_t counts[8];
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
	uint32_t unknown[116];
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

// I got nothin - maybe this is something referenced later?
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
	union {
		struct {
			uint32_t unknown_l[34];
			float float_value;
			uint32_t unknown_h[35];
		};
		uint32_t unknown[70];
	};
} vcsl_6;

// Note: This is wrong, I just haven't seen this actually get used,
// so until then it's going to stay as this silly stub.
typedef struct {
	char name[12];
	uint32_t value[10];
} vcsl_7;

typedef struct {
	char name[72];
	uint32_t num_keys;
} vcsl_8;

typedef struct {
	char name[64];
	uint32_t unknown[4];
} vcsl_8_kv;

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
