#ifndef DMX_H
#define DMX_H

#include <sys/types.h>
#include <stdio.h>

// This is the core abstraction for dmxes
typedef struct {
	uint32_t foobar;
} dmx;

void dmx_init(dmx* obj);
void dmx_free(dmx* obj);
int dmx_to_file_kv3(FILE* out, dmx* obj);
dmx* dmx_from_file(FILE* in);
dmx* dmx_from_buffer(char* buffer, int length);
#endif
