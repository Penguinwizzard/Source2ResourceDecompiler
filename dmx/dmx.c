#include "dmx.h"
#include "dmxb.h"
#include <string.h>

void dmx_init(dmx* obj) {
	if(obj == NULL) {
		fprintf(stderr,"Error: tried to dmx_init null pointer\n");
		return;
	}
	// Set up the dynamically-reallocated table for indirect attributes.
	obj->__attributebuf_length = 0;
	obj->__attributebuf_count = 0;
	obj->__indirectattributes = NULL;
}
void dmx_free(dmx* obj) {
	if(obj == NULL) {
		fprintf(stderr,"Error: tried to dmx_free null pointer\n");
		return;
	}
}
int dmx_to_file_kv3(FILE* out, dmx* obj) {
	if(out == NULL) {
		fprintf(stderr,"Error: tried to write dmx to nonexistent output file\n");
		return -1;
	}
	if(obj == NULL) {
		fprintf(stderr,"Error: tried to print nonexistent dmx\n");
		return -1;
	}
	return 0;
}
dmx* dmx_from_file(FILE* in) {
	if(in == NULL) {
		fprintf(stderr,"Error: tried to create dmx from null file pointer\n");
		return NULL;
	}
	return NULL;
}
dmx* dmx_from_buffer(char* buffer, int length) {
	if(buffer == NULL) {
		fprintf(stderr,"Error: tried to create dmx from null buffer\n");
		return NULL;
	}
	if(length == 0) {
		fprintf(stderr,"Error: tried to create dmx from empty buffer\n");
		return NULL;
	}
	// Identify the type of the dmx file
	if(!strncmp(buffer,"<!-- DMXVersion binary_v2 -->",29)) {
		fprintf(stderr, "Error: The legacy dmx version binary_v2 is not supported\n");
		return NULL;
	}
	if(strncmp(buffer,"<!-- dmx encoding ",18)) {
		fprintf(stderr, "Error: DMX format specifier header seems a bit off...determination as a dmx file potentially erroneous.\n");
		return NULL;
	}
	char* current = buffer+18;
	if(!strncmp(current,"binary",6)) {
		//ok, it's binary, now what kind of binary? 
		int encodingversion;
		char filetype[20];
		int fileencodingversion;
		int lengthparsed;
		int matched = sscanf(buffer,"<!-- dmx encoding binary %i format %19[a-zA-Z] %i -->\n%n",&encodingversion, filetype, &fileencodingversion, &lengthparsed);
		printf("%i %i\n",matched,lengthparsed);
		if(matched == EOF) {
			fprintf(stderr, "Error parsing dmx header - it seems to be misformatted?\n");
			return NULL;
		}
		switch(encodingversion) {
			case 1: return dmx_from_buffer_1(buffer+lengthparsed+1,length);
			case 2: return dmx_from_buffer_2(buffer+lengthparsed+1,length);
			case 3: return dmx_from_buffer_3(buffer+lengthparsed+1,length);
			case 4: return dmx_from_buffer_4(buffer+lengthparsed+1,length);
			case 5: return dmx_from_buffer_5(buffer+lengthparsed+1,length);
			case 6: return dmx_from_buffer_6(buffer+lengthparsed+1,length);
			case 7: return dmx_from_buffer_7(buffer+lengthparsed+1,length);
			case 8: return dmx_from_buffer_8(buffer+lengthparsed+1,length);
			case 9: return dmx_from_buffer_9(buffer+lengthparsed+1,length);
			default:
				fprintf(stderr,"Unknown variant of binary dmx - %i\n",encodingversion);
				return NULL;
		}
	} else if (!strncmp(current,"keyvalues",8)) {
		fprintf(stderr,"keyvalues2/3 not yet handled!\n");
		return NULL;
	}
	fprintf(stderr,"Unknown dmx variant\n");
	return NULL;
}
