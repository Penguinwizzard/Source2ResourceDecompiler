#ifndef wgeo_h
#define wgeo_h
#include "fileinfo.h"
#include<sys/types.h>
#include<inttypes.h>
#include<errno.h>

// WGEO - League of Legends World Geometry Format

#ifndef WIN32
typedef struct __attribute__((__packed__)) {
#else
#pragma pack(push,1)
typedef struct {
#endif
	uint32_t magic;		// Woah, someone knows file format design basics in the ARTS dev scene? WAOW!
	uint16_t majorVersion;
	uint16_t minorVersion;
	uint32_t meshCount;	// Maps are split into several meshes (potentially)
	uint32_t triCount;	// For pre-allocating enough memory
} wgeo_header;

typedef struct {		// Come on Riot, why didn't you make this fixed-size? it would save so much parsing time!
	char materialName[256];
	uint32_t unknown;
	char name[64];
	float sphere[4];	// Visibility checking?
	float min[3];		// bounds
	float max[3];
	uint32_t numverts;
	uint32_t numindicies;
} wgeo_mesh_header;

typedef struct {
	float pos[3];
	float uv[3];
} wgeo_vert;

#endif
