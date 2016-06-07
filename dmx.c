#include "dmx/dmx.h"
#include "dmx.h"
void parse_dmx(filedata* fd) {
	fd->parsed_object = dmx_from_buffer(fd->contents, fd->length);
}
