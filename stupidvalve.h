#ifndef stupidvalve_h
#define stupidvalve_h
#include "fileinfo.h"
#include<sys/types.h>
#include<inttypes.h>
#include<errno.h>

// SVF = Stupid Valve Format

typedef struct {
	uint32_t filelength;	// Why this is specified in a modern format is beyond me
				// The whole format would be so much better if this was a type-specific magic number instead
				// We'd actually be able to add a `file` rule for them, we'd be able to do exention-agnostic
				// code, and a ton of other stuff
				// But no, Valve is stuck in the 90s.
				// Also, note that this number is often wrong, so don't use it for allocation or anything 
				// terribly important. In fact, it's probably better if you ignore this altogether, and 
				// just go on with your life and ignore the format details.
	uint32_t unknown_1;	// 12 - Length of lump header?
	uint32_t unknown_2;	// 8  - ?
	uint32_t numlumps;	// Number of lumps that immediately follow
} svf_header;

typedef struct {
	union {
		char tag[4];		// A magic number type identifier. AREN'T THEY NICE VALVE?
		uint32_t type_asint;
	};
	uint32_t offset;	// Offset in the source file that the body of the lump starts at
	uint32_t length;	// Note that length includes the 12 bytes for the header
} svflump_header;

typedef struct {
	union {
		uint32_t type;		// uint32 representation of tag, e.g. RERL
		char type_aschar[4];
	};
	char* content;
	uint32_t filler[10];
} svfl_header;

typedef struct {
	svf_header* hdr;
	svflump_header* lumpheaders;
	svfl_header* lumps;
} svffile_generic;

/*
 * RERL
 */

typedef struct {
	uint32_t unknown1;
	uint32_t numentries;	// Short and simple
	uint32_t unknown2;	// fixes a crash after the reborn update.
} svfl_rerl_header_datafile;

typedef struct {
	uint64_t objecttag;	// The tag of the target resource - calculation method unknown
	uint32_t offset;	// Measured from this entry ... volvo plz
	uint32_t unknown;	// Always 0?
} svfl_rerl_entry_datafile;

typedef struct {
	svfl_rerl_entry_datafile* df;
	char* content;
} svfl_rerl_entry;

typedef struct {
	uint32_t type;
	char* content;
	svfl_rerl_header_datafile* df;
	svfl_rerl_entry* entries;
} svfl_rerl_header;

/*
 * REDI
 */
typedef struct {
	uint32_t offset;
	uint32_t count;
} offsetcount;

#ifndef WIN32
typedef struct __attribute__((__packed__)) {
#else
#pragma pack(push,1)
typedef struct {
#endif
	offsetcount sourceresource;
	offsetcount sourceresourceadd;
	offsetcount typeddata;
	offsetcount namemap;
	offsetcount unknown2;
	offsetcount unknown3;
	offsetcount deferredref;
	offsetcount specialdata;
	offsetcount unknown4;
	offsetcount unknown5;
} svfl_redi_header_datafile;
#ifdef WIN32
#pragma pack(pop)
#endif

// Sourceresourceref sub
// Sourceresourcerefadd sub
typedef struct {
	uint32_t offset_filename;
	uint32_t offset_modname;
	uint32_t unknown1;	// dunno
	uint32_t unknown2;	// 2?
} svfl_redi_sourceresource_datafile;

typedef struct {
	svfl_redi_sourceresource_datafile* df;
	char* filename;
	char* contentsearchpath;
} svfl_redi_sourceresource;

// Typeddata sub
typedef struct {
	uint32_t offset_name;
	uint32_t offset_type;
	uint32_t flags1;	// The two flag fields are generally the same value.
	uint32_t flags2;	// observed are 0x00000001 and 0x00002000
} svfl_redi_typeddata_datafile;

typedef struct {
	svfl_redi_typeddata_datafile* df;
	char* name;
	char* type;
} svfl_redi_typeddata;

// Namemap sub
typedef struct {
	uint32_t offset_expanded;// the longer one, occurs second later :/
	uint32_t offset_key;	// the shorter one, occurs first later
	uint32_t unk1;		// Various values
	uint32_t unk2;		// Always 0?
} svfl_redi_namemap_datafile;

typedef struct {
	svfl_redi_namemap_datafile* df;
	char* key;
	char* expanded;
} svfl_redi_namemap;

// Deferredref sub
typedef struct {
	uint64_t objecttag;	// The tag of the target resource - calculation method unknown
	uint32_t offset;	// Measured from this entry ... volvo plz
	uint32_t unknown;	// Always 0?
} svfl_redi_deferredref_datafile;

typedef struct {
	svfl_redi_deferredref_datafile* df;
	char* content;
} svfl_redi_deferredref;

// SpecialData sub
typedef struct {
	uint32_t offset_key;
	uint32_t value;
} svfl_redi_specialdata_datafile;

typedef struct {
	svfl_redi_specialdata_datafile* df;
	char* key;
} svfl_redi_specialdata;

typedef struct {
	uint32_t type;
	char* content;
	svfl_redi_header_datafile* df;
	svfl_redi_sourceresource* srentries;
	svfl_redi_sourceresource* sraentries;
	svfl_redi_typeddata* tdentries;
	svfl_redi_namemap* nmentries;
	void* unknown2;
	void* unknown3;
	svfl_redi_deferredref* drentries;
	svfl_redi_specialdata* sdentries;
	void* unknown4;
	void* unknown5;
	char* resourcename;
	char* modname;
} svfl_redi_header;

/*
 * NTRO
 */
typedef struct {
	uint32_t version;	// Always 4? (thanks hmfd for tag)
	uint32_t typetag;	// Used for structs in structs and the like
	uint32_t offset_classname;
	uint32_t crc;		// crc (thanks hmfd for tag)
	uint32_t user_version;	// Always 0? (thanks hmfd for tag)
	uint16_t length;	// length in data lump
	uint16_t alignment;	// Always 4? (thanks hmfd for tag)
	uint32_t base_struct_id;// Always 0? (thanks hmfd for tag)
	uint32_t offset_tagheaders;
	uint32_t num_tags;
	uint32_t unknown7;	// padding?
} svfl_ntro_entry_header_datafile;

#define SVFL_DATATYPE_SUBSTRUCT 1
#define SVFL_DATATYPE_ENUM 2
#define SVFL_DATATYPE_EXTREF 3
#define SVFL_DATATYPE_STRING4 4
#define SVFL_DATATYPE_BYTE 11
#define SVFL_DATATYPE_SINT 12
#define SVFL_DATATYPE_NUMBER 14
#define SVFL_DATATYPE_FLAGS 15
#define SVFL_DATATYPE_FLOAT 18
#define SVFL_DATATYPE_VEC3 22
#define SVFL_DATATYPE_MASSIVE 23
#define SVFL_DATATYPE_VEC4 28
#define SVFL_DATATYPE_BOOLEAN 30
#define SVFL_DATATYPE_STRING 31

#ifndef WIN32
typedef struct __attribute__((__packed__)) {
#else
#pragma pack(push,1)
typedef struct {
#endif
	uint32_t offset_tagname;
	uint16_t count;		// if 0, treat as 1; if more, treat as is
	uint16_t offset_in_struct;	// increases based on size of entry
	uint32_t unknown2;	// Always 0? (hmfd has as "offset to indirection (?)")
	uint32_t unknown3;	// Always 0? (hmfd has as "number of indirections (?)")
	uint32_t ref_typetag;	// Type tag referenced
	uint32_t datatype;	// 1     - array reference - 8 bytes - offset 4b, count 4b
				// 2/n   - enum?
				// 3     - other resource reference - 8 bytes, holds resource key
				// 4     - also string? (hmfd)
				// 11/p  - 
				// 12/n  - signed number? - 4 bytes
				// 14/n  - number - 4 bytes
				// 15/n  - flags  - 4 bytes
				// 17/h  - buffer? - 8 bytes
				// 18/fl - float  - 4 bytes
				// 22/v  - vector - 12 bytes
				// 23/v  - vector - 64 bytes!
				// 25/q  - quaternion? - 16 bytes
				// 30/b  - boolean - 1 byte
				// 31	 - worldNodePrefix/ResoureFileNameList?
} svfl_ntro_entry_tag_datafile;
#ifdef WIN32
#pragma pack(pop)
#endif

typedef struct {
	svfl_ntro_entry_tag_datafile* df;
	char* name;
	int refindex;		// Index into the refs table
} svfl_ntro_entry_tag;

typedef struct {
	svfl_ntro_entry_header_datafile* hdf;
	svfl_ntro_entry_tag* tags;
	char* classname;
	int numrefs;		// Number of references to internal stuffs
} svfl_ntro_entry;

typedef struct {
	uint32_t version;
	uint32_t offset_entries;
	uint32_t numentries;
} svfl_ntro_header_datafile;

typedef struct {
	uint32_t type;
	char* content;
	svfl_ntro_header_datafile* df;
	svfl_ntro_entry* entries;
} svfl_ntro_header;

/*
 * DATA sub
 */
// Since this is where actual data is handled, we change form a bit
// The data is just an instance of the first described struct (from the NTRO sub)
// which has been encoded as specified. We just need to decode it, and then
// provide means by which we can access the constituent values.
typedef struct svfl_struct_t {
	svfl_ntro_entry* type;
	struct svfl_struct_t** children;
	char* data;
} svfl_struct;

/*
 * Function stubs
 */

void parse_svf(filedata* fd);
void parse_object(svfl_struct* object, svfl_ntro_header* ntro, char* data);
void print_object_recursive(svfl_struct* object);
svfl_ntro_entry* do_type_lookup(svfl_ntro_header* ntro, uint32_t typetag);
void free_svf(filedata* fd);
#endif
