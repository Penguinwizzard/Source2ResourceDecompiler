#include <stdlib.h>
#include <string.h>
// NOTE THAT DMXB is set before this file is loaded!

/*
 *  DMX is a fairly straightforward stream-based file format. The main
 *  issue with handling it properly is that it's poorly documented, and
 *  the documentation that exists can be inconsistent. Additionally,
 *  there have been several versions released over the years; they do
 *  some things differently, which I try to handle in a (fairly) clear
 *  manner below.
 */
#ifdef STRING_INDEX_SIZE
#undef STRING_INDEX_SIZE
#undef STRING_INDEX_UNSET
#undef STRING_DICT_SIZE_TYPE
#endif
#if DMXB < 5
#define STRING_INDEX_SIZE uint16_t
#define STRING_INDEX_UNSET 0xFFFF
#else
#define STRING_INDEX_SIZE uint32_t
#define STRING_INDEX_UNSET 0xFFFFFFFF
#endif
#if DMXB < 4
#define STRING_DICT_SIZE_TYPE uint16_t
#else
#define STRING_DICT_SIZE_TYPE uint32_t
#endif

// one-line definition for a variable-type indirect stream-based read
#define READI(x)                                                               \
	*(x *)(*bufferptrptr);                                                 \
	*bufferptrptr += sizeof(x);
// string handling
#define READISTRING()                                                          \
	(char *)(*bufferptrptr);                                               \
	*bufferptrptr += strlen(*bufferptrptr) + 1;
// object pointer handling
#define READIP(x)                                                              \
	(x *)(*bufferptrptr);                                                  \
	*bufferptrptr += sizeof(x);
// buffer for reading in dmxattributes
dmxattribute *V(readattribute)(dmx *dmx, char **bufferptrptr, bool stringsraw) {
	uint8_t type = READI(uint8_t);
	printf("\t\tparsing attribute with type %hhu\n", type);
	uint32_t typeindex = type - 1;
	bool array = false;
#ifdef TYPECOUNT
#undef TYPECOUNT
#endif
#if DMXB < 9
#define TYPECOUNT 15
#else
#define TYPECOUNT 16
#endif
#if DMXB >= 9
	if (typeindex > 2 * TYPECOUNT) {
		typeindex -= TYPECOUNT * 2;
		array = true;
	}
#endif
	if (typeindex > TYPECOUNT) {
		typeindex -= TYPECOUNT;
		array = true;
	}

	uint32_t count = 1;
	if (array) {
		count = READI(uint32_t);
	}
	switch (typeindex) {
	case 0x1:; // int32
	case 0x2:; // float
	case 0x3:; // bool
	case 0x6:; // TimeSpan
	case 0x7:; // Color
	case 0xE:; // uint64
	case 0xF:; // byte
		// these are natively handled! just keep on doing your thing...
		if (array) {
			dmxattribute *ret = allocindirectdmxattribute(dmx);
			ret->type = type;
			ret->val.array = (dmxcontentval *)(*bufferptrptr);
			ret->val.count = count;
			*bufferptrptr += lengths[typeindex] * count;
		} else {
			dmxattribute *ret =
			    (dmxattribute *)((*bufferptrptr) - sizeof(uint8_t));
			*bufferptrptr += lengths[typeindex];
			return ret;
		}
	case 0x0:; // Element
	case 0x4:; // string
	case 0x5:; // byte[]
	case 0x8:; // vector of 2 floats
	case 0x9:; // vector of 3 floats
	case 0xA:; // vector of 4 floats
	case 0xB:; // angle
	case 0xC:; // quaternion
	case 0xD:; // matrix
		// these are indirectly handled - we allocate space specially
		// for these.
		dmxattribute *ret = allocindirectdmxattribute(dmx);
		ret->type = type;
		dmxcontentval *value = &(ret->val);
		dmxcontentval *writeto;
		if (array) {
			value->array = malloc(count * sizeof(dmxcontentval));
			value->count = count;
			writeto = value->array;
		} else {
			writeto = value;
		}
		for (uint32_t i = 0; i < count; i++, writeto += 1) {
			switch (typeindex) {
			case 0x0: // Element
				writeto->index = READI(uint32_t);
				if (writeto->index == -1) {
					// null element
				} else if (writeto->index == -2) {
					// guid is here, as ascii string
					writeto->guid_AsString = READISTRING();
				} else {
					// it's just read based off the index
				}
				break;
			case 0x4: // string
				if (stringsraw) {
					writeto->indirect = false;
					writeto->strval = READISTRING();
					printf("read raw string %s\n",
					       writeto->strval);
				} else {
					writeto->indirect = true;
					writeto->strindex =
					    READI(STRING_INDEX_SIZE);
				}
				break;
			case 0x5: // byte[]
				writeto->barr_length = READI(uint32_t);
				writeto->barr_contents =
				    (uint8_t *)*bufferptrptr;
				// FILE* fp = fopen("dump.jpg","w");
				// fwrite(writeto->barr_contents,
				// sizeof(uint8_t), writeto->barr_length,fp);
				// fclose(fp);
				*bufferptrptr += writeto->barr_length;
				break;
			case 0x8: // vector of 2 floats
				writeto->vec2val = READIP(dmx_vector2);
				break;
			case 0x9:
				writeto->vec3val = READIP(dmx_vector3);
				break;
			case 0xA:
				writeto->vec4val = READIP(dmx_vector4);
				break;
			case 0xB:
				writeto->angleval = READIP(dmx_vector3);
				break;
			case 0xC:
				writeto->quatval = READIP(dmx_vector4);
				break;
			case 0xD:
				writeto->matval = READIP(dmx_matrix);
				break;
			}
		}
		return ret;
	default:;
		fprintf(stderr, "Error: unknown type %hhu!\n", type);
		return NULL;
	}
}
#undef READI
#undef READIP
#undef READISTRING

void V(printattribute)(const dmxattribute *attr) {
	if (attr == NULL) {
		fprintf(
		    stderr,
		    "Error: tried printing null pointer as dmx attribute\n");
		return;
	}
}

char *V(dictsearch)(char *const *const dict, const STRING_INDEX_SIZE index,
                    const STRING_DICT_SIZE_TYPE count) {
	if (index == STRING_INDEX_UNSET) {
		fprintf(stderr, "was null\n");
		return "";
	}
	if (index >= count) {
		fprintf(stderr, "string index out of range!\n");
		return "";
	}
	char *ret = dict[index];
	return ret;
}

// The main function to parse a dmx file from a buffer. Note that this
// could probably be re-written to handle streams, and get a little bit
// of a performance boost from that.

// one-line definition for a variable-type stream-based read
#define READ(x)                                                                \
	*(x *)curindex;                                                        \
	curindex += sizeof(x)
// string handling
#define READSTRING()                                                           \
	(char *)curindex;                                                      \
	curindex += strlen(curindex) + 1;
// object pointer handling
#define READP(x)                                                               \
	(x *)(curindex);                                                       \
	curindex += sizeof(x);
dmx *V(dmx_from_buffer)(char *buffer, const unsigned int length) {
	char *curindex = buffer;
	dmx *ret = (dmx *)malloc(sizeof(dmx));
	dmx_init(ret);
#if DMXB >= 7
	// In version 7, the prefix data structure got introduced.
	// This is (I believe) how to properly parse it:
	{
		printf("reading prefix data...\n");
		if (curindex - buffer + sizeof(uint32_t) > length) {
			fprintf(
			    stderr,
			    "Error in dmx parse: insufficient data (type 0)\n");
			return NULL;
		}
		uint32_t numelements = READ(uint32_t);
		printf("reading %i elements...\n", numelements);
		for (uint32_t i = 0; i < numelements; i++) {
			uint32_t numattributes = READ(uint32_t);
			if (i == 0) {
				ret->prefixdata = (dmxattribute **)malloc(
				    numattributes * sizeof(dmxattribute *));
			}
			printf("\treading %i attributes...\n", numattributes);
			for (uint32_t j = 0; j < numattributes; j++) {
				char *name = READSTRING();
				printf("%s\n", name);
				V(readattribute)(ret, &curindex, true);
			}
		}
	}
#endif
	if (length < sizeof(STRING_DICT_SIZE_TYPE)) {
		fprintf(stderr,
		        "Error in dmx parse: insufficient data (type 1)\n");
		return NULL;
	}
	// Check that we're able to parse it
	STRING_DICT_SIZE_TYPE numstrings = READ(STRING_DICT_SIZE_TYPE);
	printf("dmx_from_buffer_%d: Parsing %d strings into string "
	       "dicitonary...\n",
	       DMXB, (uint32_t)numstrings);
	char **strings = (char **)malloc(sizeof(char *) * numstrings);
	for (uint32_t i = 0; i < (uint32_t)(numstrings); i++) {
		strings[i] = READSTRING();
		printf("dmx_from_buffer_%d: \t\t%s\n", DMXB, strings[i]);
	}
	for (uint32_t i = 0; i < (uint32_t)(numstrings); i++) {
		printf("dictionary contains %s at %u\n",
		       V(dictsearch)(strings, i, numstrings), i);
	}
	// Read it into the data structure
	// Take a look at the elements
	uint32_t numelements = READ(uint32_t);
	printf("dmx_from_buffer_%d: Parsing %u elements...\n", DMXB,
	       numelements);
	for (uint32_t i = 0; i < numelements; i++) {
		STRING_INDEX_SIZE typeindex = READ(STRING_INDEX_SIZE);
		char *type = V(dictsearch)(strings, typeindex, numstrings);
#if DMXB >= 4
		STRING_INDEX_SIZE nameindex = READ(STRING_INDEX_SIZE);
		char *name = V(dictsearch)(strings, nameindex, numstrings);
#else
		char *name = READSTRING();
#endif
		STRING_INDEX_SIZE unknownindex = READ(STRING_INDEX_SIZE);
		char *unknown =
		    V(dictsearch)(strings, unknownindex, numstrings);
		GUID *guid = READP(GUID);
		printf("ELEMENT_PARSE_TEST: %s: %s (%s) %hhx\n", type, name,
		       unknown, guid->bytes[0]);
		printf("CORRUPTION CHECK: %p\n", (void *)strings[6]);
		fflush(stdout);
	}
	for (uint32_t i = 0; i < (uint32_t)(numstrings); i++) {
		printf("dictionary contains %s at %u\n",
		       V(dictsearch)(strings, i, numstrings), i);
	}
	for (uint32_t elem = 0; elem < numelements; elem++) {
		uint32_t numattributes = READ(uint32_t);
		printf("dmx_from_buffer_%d: Parsing %u attributes...(%u/%u)\n",
		       DMXB, numattributes, elem + 1, numelements);
		fflush(stdout);
		for (uint32_t i = 0; i < numattributes; i++) {
			printf("strings[6] is %p\n", (void *)strings[6]);
			fflush(stdout);
			STRING_INDEX_SIZE attrnameindex =
			    READ(STRING_INDEX_SIZE);
			printf("got index, it's %u\n", (uint32_t)attrnameindex);
			fflush(stdout);
			char *attrname =
			    V(dictsearch)(strings, attrnameindex, numstrings);
			printf("2: strings[6] is %p\n", (void *)strings[6]);
			fflush(stdout);
			printf("got string!\n");
			fflush(stdout);
			printf("%s:\n", attrname);
			fflush(stdout);
			V(readattribute)(ret, &curindex, true);
			printf("test\n");
			fflush(stdout);
		}
	}
	fflush(stdout);
	printf("finished parse!\n");
	// cleanup
	free(strings);
	return ret;
}

#undef READ
#undef READP
#undef READSTRING

// take a memory dmx, and put it in a data file appropriately
/* TODO
char* V(dmx_to_buffer) (dmx* in, long* length) {
        if(in==NULL) {
                fprintf(stderr, "Error: Tried writing dmx to null buffer!\n");
                *length = 0;
                return NULL;
        }
        if(length == NULL) {
                fprintf(stderr, "Error: length return for dmx_to_buffer_%u was
null!\n",DMXB); *length = 0; return NULL;
        }
        int size = 1000;
        char* ret = (char*)malloc(size*sizeof(char));
        char* cur = ret;
        // go through the dmx, and build a dictionary for it.
        uint32_t count = 0;
        uint32_t charcount = 0;
        for(uint32_t i=0;i<in->numelements;i++) {
        }
        // emit the header
        // emit the prefix element
        // emit the element table
        // emit the attribute table
        // return successfully!
        *length = 0;
        return NULL;
}
*/
