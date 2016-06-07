
#include "dmxb.h"

/*
 *  This hopefully appears somewhat non-idiomatic to you. If not, please seek
 *  help. The goal of doing this is for the compiler to generate optimized
 *  routines for each of the different variants of binary DMX, without having to
 *  go through and create a ton of _actual_ different files. Instead, we use the
 *  power of the C preprocessor to generate virtual files for each of the
 *  versions, each of which has special changes courtesy of preprocessor macros
 *  and conditionals.
 */

#define V3(fname, val) fname ## _ ## val
#define V2(fname, val) V3(fname, val)
#define V(fname) V2(fname, DMXB )

dmxattribute* allocindirectdmxattribute(dmx* obj) {
	// See if we need to increase the size of the buffer
	if(obj->__attributebuf_length == obj->__attributebuf_count) {
		obj->__attributebuf_length += 16;
		obj->__indirectattributes = realloc(obj->__indirectattributes, obj->__attributebuf_length * sizeof(dmxattribute));
	}
	obj->__attributebuf_count++;
	return &(obj->__indirectattributes[obj->__attributebuf_count-1]);
}

uint32_t lengths[] = {
	0,
	sizeof(int32_t),
	sizeof(float),
	sizeof(bool),
	0,
	0,
	sizeof(uint32_t),
	sizeof(uint32_t),
	0,
	0,
	0,
	0,
	0,
	0,
	sizeof(uint64_t)+2,
	sizeof(uint8_t)};

#define DMXB 1
#include "dmxbraw.c"
#undef DMXB
#define DMXB 2
#include "dmxbraw.c"
#undef DMXB
#define DMXB 3
#include "dmxbraw.c"
#undef DMXB
#define DMXB 4
#include "dmxbraw.c"
#undef DMXB
#define DMXB 5
#include "dmxbraw.c"
#undef DMXB
#define DMXB 6
#include "dmxbraw.c"
#undef DMXB
#define DMXB 7
#include "dmxbraw.c"
#undef DMXB
#define DMXB 8
#include "dmxbraw.c"
#undef DMXB
#define DMXB 9
#include "dmxbraw.c"
#undef DMXB

#undef V3
#undef V2
#undef V
