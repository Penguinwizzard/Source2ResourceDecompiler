#ifndef LIBDMX_H
#define LIBDMX_H

#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>

// a few types used in dmxes
typedef struct {
	float x;
	float y;
} dmx_vector2;
typedef struct {
	float x;
	float y;
	float z;
} dmx_vector3;
typedef struct {
	float x;
	float y;
	float z;
	float a;
} dmx_vector4;
typedef struct {
	dmx_vector4 x;
	dmx_vector4 y;
	dmx_vector4 z;
	dmx_vector4 a;
} dmx_matrix;
typedef struct {
	uint8_t bytes[16];
} GUID;
// conveniently, this coincides with the on-disk representation of the dmx files for
// a good number of these - allowing much faster parsing, since we just need to do a
// pointer update, instead of putting these in a separate structure and copying data
// around. The whole structure ends up being 7 bytes after packing, although the end
// is often ignored/unused.
typedef union __attribute__ ((__packed__)) _dmxcontentval_t {
	struct { // array
		uint32_t count;
		union _dmxcontentval_t* array; // note that we treat this as a void* for directly-handled types
	};
	struct { // element
		int32_t index;
		union {
			GUID* guid;
			char* guid_AsString;
		};
	};
	int32_t intval; // direct
	float floatval; // direct
	bool boolval; // direct
	struct { // indirect - needs new one
		bool indirect;
		union {
			uint32_t strindex;
			char* strval;
		};
	};
	struct { // indirect
		uint32_t barr_length;
		uint8_t* barr_contents;
	};
	uint32_t timespan; // direct
	uint32_t color; // direct
	dmx_vector2* vec2val; // indirect
	dmx_vector3* vec3val; // indirect
	dmx_vector4* vec4val; // indirect
	dmx_vector3* angleval; // indirect
	dmx_vector4* quatval; // indirect
	dmx_matrix* matval; // indirect
	uint64_t longval; // direct
	uint8_t byteval; // direct
} dmxcontentval;
typedef struct __attribute__ ((__packed__)) {
	uint8_t type;
	dmxcontentval val;
} dmxattribute;

typedef struct {
	char* type;
	char* name;
	GUID guid;
	uint32_t numattributes;
	dmxattribute* attributes;
} dmxelement;

// This is the core abstraction for dmxes
typedef struct {
	// internal use only - would be private if we had that
	int __attributebuf_length; // Used for dynamic reallocation.
	int __attributebuf_count;
	dmxattribute* __indirectattributes; // Used for holding non-file-defined attributes after parse.
	// "public" use, but modify these through the accessor functions
	dmxattribute** prefixdata;
	uint32_t numelements;
	dmxcontentval* elements;
	char type[4];
	int typeversion;
	// if using the threadsafe variants of the mutator/accessor functions, we may need to lock sometimes.
	pthread_mutex_t* lock;
} dmx;

void dmx_init(dmx* obj);
void dmx_free(dmx* obj);
int dmx_to_file_kv3(FILE* out, dmx* obj);
dmx* dmx_from_file(FILE* in);
dmx* dmx_from_buffer(char* buffer, int length);
char* dmx_to_buffer(dmx* in, long* length, bool binary, int version);
// non-threadsafe versions of functions
// threadsafe versions of functions
#endif
