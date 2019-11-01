typedef struct {
	uint32_t encodingversion;
	char *type;
	uint32_t typeversion;
} dmxfile;

typedef struct dmx_binary_dmeheader_t {
	uint32_t type; // index into stringdict
	uint32_t name; // index into stringdict
	char guid[16];
	dmx_binary_dmeheader_t subelements;
} dmx_binary_dmeheader;

typedef struct dmx_binary_dmebody_t {
} dmx_binary_dmebody;

typedef struct {
	// there's a string first that identifies it as dmx and the version
	uint32_t nStrings;
	uint32_t stringdict_offset;
	uint32_t nElements;
	dmx_binary_dmeheader;
	dmx_binary_dmebody;
} dmx_binary_header_datafile;

typedef struct {
	dmx_binary_header *df;
	char **strings;
} dmx_binary_header;
