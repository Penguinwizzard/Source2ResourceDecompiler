#include <string.h>
#include <stdlib.h>
// NOTE THAT DMXB is set before this file is loaded!
dmx* V(dmx_from_buffer) (char* buffer, unsigned int length) {
	if(length < sizeof(V(dmx_dict_header))) {
		fprintf(stderr, "Error in dmx parse: insufficient data\n");
		return NULL;
	}
	// Check that we're able to parse it
	V(dmx_dict_header)* dh = (V(dmx_dict_header)*)buffer;
	printf("dmx_from_buffer_%d: Parsing %d strings into string dicitonary...\n", DMXB, (uint32_t)(dh->nStrings));
	char* pointer = buffer + sizeof(V(dmx_dict_header));
	for(uint32_t i=0;i<(uint32_t)(dh->nStrings);i++) {
		printf("dmx_from_buffer_%d: \t\t%s",DMXB,pointer);
		pointer += strlen(pointer)+1;
	}
	// Read it into the data structure
	dmx* ret = (dmx*)malloc(sizeof(dmx));
	dmx_init(ret);
	return ret;
}

