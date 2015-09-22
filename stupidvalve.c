#include "stupidvalve.h"

const char downconvert[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#define RLHI ret->lumpheaders[i]
// Take an offset field, and compute the target address
#define OFFS( x ) (((char*)&( x )) + ( x ))
/*
 * This new valve file format doesn't have type specifiers, so it's awful to handle.
 */
void parse_svf(filedata* fd) {
	svffile_generic* ret = (svffile_generic*)malloc(sizeof(svffile_generic));
	ret->hdr = (svf_header*)(fd->contents);
	if(ret->hdr->filelength != fd->length) {
		fprintf(stderr,"Warning: SVF header length does not match file length.\n");
	}
	ret->lumpheaders = (svflump_header*)(fd->contents + sizeof(svf_header));
	ret->lumps = (svfl_header*)malloc(sizeof(svfl_header)*ret->hdr->lumps.count);
	uint32_t i;
	for(i=0;i<ret->hdr->lumps.count;i++) {
		printf("[%c%c%c%c]: offset %X | length: %X\n",RLHI.tag[0],RLHI.tag[1],RLHI.tag[2],RLHI.tag[3],RLHI.offset,RLHI.length);
		ret->lumps[i].content = OFFS(RLHI.offset);
		// because dealing with these is a lot faster than doing string checks all the time
		ret->lumps[i].type = RLHI.type_asint;
		if(strncmp(RLHI.tag,"RERL",4) == 0) {
			//Parse as a Resource External Reference block
			svfl_rerl_header* rh = (svfl_rerl_header*)&(ret->lumps[i]);
			rh->df = (svfl_rerl_header_datafile*)rh->content;
			rh->entries = (svfl_rerl_entry*)malloc(rh->df->entries.count * sizeof(svfl_rerl_entry));
			svfl_rerl_entry_datafile* dfs = (svfl_rerl_entry_datafile*)OFFS(rh->df->entries.offset); 
			uint32_t j;
			for(j=0;j<rh->df->entries.count;j++) {
				rh->entries[j].df = &(dfs[j]);
				rh->entries[j].content = OFFS( rh->entries[j].df->offset );
				printf("%.16" PRIx64 ": %s\n",rh->entries[j].df->objecttag,rh->entries[j].content);
			}
		} else if(strncmp(RLHI.tag,"REDI",4) == 0) {
			//Parse as a Resource Deferred Reference block
			svfl_redi_header* rh = (svfl_redi_header*)&(ret->lumps[i]);
			rh->df = (svfl_redi_header_datafile*)rh->content;
			uint32_t j;
			// Sourceresource multisub
			{
				printf("\t[SubBlock] sourceresource (%u entries)\n",rh->df->sourceresource.count);
				rh->srentries = (svfl_redi_sourceresource*)malloc(rh->df->sourceresource.count * sizeof(svfl_redi_sourceresource));
				svfl_redi_sourceresource_datafile* srsd = (svfl_redi_sourceresource_datafile*)OFFS(rh->df->sourceresource.offset);
				for(j=0;j<rh->df->sourceresource.count;j++) {
					rh->srentries[j].filename = OFFS(srsd[j].offset_filename);
					rh->srentries[j].contentsearchpath = OFFS(srsd[j].offset_modname);
					printf("\t\t[File] '%s' in contentsearchpath: '%s'\n",rh->srentries[j].filename,rh->srentries[j].contentsearchpath);
				}
			}
			// Sourceresourceadd multisub
			{
				printf("\t[SubBlock] sourceresourceadd (%u entries)\n",rh->df->sourceresourceadd.count);
				rh->sraentries = (svfl_redi_sourceresource*)malloc(rh->df->sourceresourceadd.count * sizeof(svfl_redi_sourceresource));
				svfl_redi_sourceresource_datafile* srsd = (svfl_redi_sourceresource_datafile*)OFFS(rh->df->sourceresource.offset);
				for(j=0;j<rh->df->sourceresourceadd.count;j++) {
					rh->sraentries[j].filename = OFFS(srsd[j].offset_filename);
					rh->sraentries[j].contentsearchpath = OFFS(srsd[j].offset_modname);
					printf("\t\t[File] '%s' in contentsearchpath: '%s'\n",rh->sraentries[j].filename,rh->sraentries[j].contentsearchpath);
				}
			}
			// Typeddata multisub
			{
				printf("\t[SubBlock]argumentss (%u entries)\n",rh->df->arguments.count);
				rh->tdentries = (svfl_redi_argument*)malloc(rh->df->arguments.count*sizeof(svfl_redi_argument));
				svfl_redi_argument_datafile* srtd = (svfl_redi_argument_datafile*)OFFS(rh->df->arguments.offset);
				for(j=0;j<rh->df->arguments.count;j++) {
					rh->tdentries[j].df = &(srtd[j]);
					rh->tdentries[j].name = OFFS(srtd[j].offset_name);
					rh->tdentries[j].type = OFFS(srtd[j].offset_type);
					printf("\t\t%s %s",rh->tdentries[j].type,rh->tdentries[j].name);
					if(srtd[j].fingerprint != 0)
						printf(" (fingerprint = %.8X)",srtd[j].fingerprint);
					if(srtd[j].fingerprint != 0)
						printf(" (fingerprint_default = %.8X)",srtd[j].fingerprint_default);
					printf("\n");
				}
			}
			// Namemap multisub
			{
				printf("\t[SubBlock] namemap (%u entries)\n",rh->df->namemap.count);
				rh->nmentries = (svfl_redi_namemap*)malloc(rh->df->namemap.count*sizeof(svfl_redi_namemap));
				svfl_redi_namemap_datafile* srnd = (svfl_redi_namemap_datafile*)OFFS(rh->df->namemap.offset);
				for(j=0;j<rh->df->namemap.count;j++) {
					rh->nmentries[j].df = &(srnd[j]);
					rh->nmentries[j].key = OFFS(srnd[j].offset_key);
					rh->nmentries[j].expanded = OFFS(srnd[j].offset_expanded);
					printf("\t\t%s -> %s (fingerprint:%x, userdata:%x)\n",rh->nmentries[j].key,rh->nmentries[j].expanded,rh->nmentries[j].df->fingerprint,rh->nmentries[j].df->userdata);
				}
			}
			// Custom dependencies multisub
			{
				printf("\t[SubBlock] customdeps (%u entries)\n",rh->df->customdeps.count);
			}
			// Additional related resources multisub
			{
				printf("\t[SubBlock] additional_related (%u entries)\n",rh->df->additional_related.count);
			}
			// ChildRef multisub
			{
				printf("\t[SubBlock] childref (%u entries)\n",rh->df->child_ref.count);
				rh->crentries = (svfl_redi_childref*)malloc(rh->df->child_ref.count*sizeof(svfl_redi_childref));
				svfl_redi_childref_datafile* srdd = (svfl_redi_childref_datafile*)OFFS(rh->df->child_ref.offset);
				for(j=0;j<rh->df->child_ref.count;j++) {
					rh->crentries[j].df = &(srdd[j]);
					rh->crentries[j].content = OFFS(srdd[j].offset);
					printf("\t\t%.16" PRIx64 ": %s\n",rh->crentries[j].df->objecttag,rh->crentries[j].content);
				}
			}
			// Extra Integer-Valued Data multisub
			{
				printf("\t[SubBlock] extradatadata_int (%u entries)\n",rh->df->extradata_int.count);
				rh->edientries = (svfl_redi_extradata_int*)malloc(rh->df->extradata_int.count*sizeof(svfl_redi_extradata_int));
				svfl_redi_extradata_int_datafile* srsd = (svfl_redi_extradata_int_datafile*)OFFS(rh->df->extradata_int.offset);
				for(j=0;j<rh->df->extradata_int.count;j++) {
					rh->edientries[j].df = &(srsd[j]);
					rh->edientries[j].key = OFFS(srsd[j].offset_key);
					printf("\t\t%s: %i\n",rh->edientries[j].key,rh->edientries[j].df->value);
				}
			}
			// Extra Float-Valued Data multisub
			{
				printf("\t[SubBlock] extradatadata_float (%u entries)\n",rh->df->extradata_float.count);
				rh->edfentries = (svfl_redi_extradata_float*)malloc(rh->df->extradata_float.count*sizeof(svfl_redi_extradata_float));
				svfl_redi_extradata_float_datafile* srsd = (svfl_redi_extradata_float_datafile*)OFFS(rh->df->extradata_float.offset);
				for(j=0;j<rh->df->extradata_float.count;j++) {
					rh->edfentries[j].df = &(srsd[j]);
					rh->edfentries[j].key = OFFS(srsd[j].offset_key);
					printf("\t\t%s: %f\n",rh->edfentries[j].key,rh->edfentries[j].df->value);
				}
			}
			// Extra String-Valued Data multisub
			{
				printf("\t[SubBlock] extradata_string (%u entries)\n",rh->df->extradata_string.count);
			}
		} else if(strncmp(RLHI.tag,"NTRO",4) == 0) {
			svfl_ntro_header* nh = (svfl_ntro_header*)&(ret->lumps[i]);
			nh->df = (svfl_ntro_header_datafile*)nh->content;
			printf("\tVersion: %i\n",nh->df->version);
			nh->entries = (svfl_ntro_entry*)malloc(nh->df->numentries * sizeof(svfl_ntro_entry));
			uint32_t j;
			for(j=0;j < nh->df->numentries;j++) {
				nh->entries[j].hdf = (svfl_ntro_entry_header_datafile*)(OFFS(nh->df->offset_entries)+j*sizeof(svfl_ntro_entry_header_datafile));
				nh->entries[j].classname = OFFS(nh->entries[j].hdf->offset_classname);
				printf("\t%i: %-30s (length %u, type tag %.8X)\n",j,nh->entries[j].classname,nh->entries[j].hdf->length, nh->entries[j].hdf->typetag);
				printf("\t  Version:%u | CRC:%.8X | uVersion:%u   L:%X A:%X Parent:%X flags:%hhX\n",
						nh->entries[j].hdf->version,
						nh->entries[j].hdf->crc,
						nh->entries[j].hdf->user_version,
						nh->entries[j].hdf->length,
						nh->entries[j].hdf->alignment,
						nh->entries[j].hdf->base_struct_id,
						nh->entries[j].hdf->struct_flags);
				nh->entries[j].tags = (svfl_ntro_entry_tag*)malloc(nh->entries[j].hdf->num_tags * sizeof(svfl_ntro_entry_tag));
				svfl_ntro_entry_tag_datafile* ths = (svfl_ntro_entry_tag_datafile*)OFFS(nh->entries[j].hdf->offset_tagheaders);
				int numrefs = 0;
				uint32_t k;
				for(k=0;k < nh->entries[j].hdf->num_tags; k++) {
					nh->entries[j].tags[k].df = &(ths[k]);
					nh->entries[j].tags[k].name = OFFS(nh->entries[j].tags[k].df->offset_tagname);
					printf("\t\t%-30s: (count: %.4hX offset: %.4hX indirections.offset: %u indirections.count: %u datatype: %u), ref_typetag:%.8X\n",
							nh->entries[j].tags[k].name,
							nh->entries[j].tags[k].df->count,
							nh->entries[j].tags[k].df->offset_in_struct,
							nh->entries[j].tags[k].df->indirections.offset,
							nh->entries[j].tags[k].df->indirections.count,
							nh->entries[j].tags[k].df->datatype,
							nh->entries[j].tags[k].df->ref_typetag);
					if(nh->entries[j].tags[k].df->indirections.count > 0) {
						uint32_t inds;
						char* baseaddr = OFFS(nh->entries[j].tags[k].df->indirections.offset);
						printf("\t\t\tindirections:\n");
						for(inds = 0; inds < nh->entries[j].tags[k].df->indirections.count; inds++) {
							printf("\t\t\t\t%hhu\n",*(uint8_t*)(baseaddr+inds));
						}
					}
					if(nh->entries[j].tags[k].df->datatype == 1) {
						nh->entries[j].tags[k].refindex = numrefs;
						numrefs++;
					} else {
						nh->entries[j].tags[k].refindex = -1;
					}
				}
				nh->entries[j].numrefs = numrefs;
			}
		} else if(strncmp(RLHI.tag,"DATA",4) == 0) {
			printf("\tDATA Lump Length: %u\n",RLHI.length);
			// We don't parse it until later
		} else if(strncmp(RLHI.tag,"VBIB",4) == 0) {
			printf("\tVBIB LUMP, POKE PWIZ\n");
			char* output = (char*)malloc(2*RLHI.length*sizeof(char));
			uint32_t j;
			for(j=0;j<RLHI.length;j++) {
				output[2*j] = (char)downconvert[*((uint8_t*)(OFFS(RLHI.offset)+j))/16];
				output[2*j+1] = (char)downconvert[*((uint8_t*)(OFFS(RLHI.offset)+j))%16];
			}
			printf("%.*s\n",2*RLHI.length,output);
		} else {
			printf("\tUNHANDLED LUMP TYPE, POKE PWIZ\n");
			char* output = (char*)malloc(2*RLHI.length*sizeof(char));
			uint32_t j;
			for(j=0;j<RLHI.length;j++) {
				output[2*j] = (char)downconvert[*((uint8_t*)(OFFS(RLHI.offset)+j))/16];
				output[2*j+1] = (char)downconvert[*((uint8_t*)(OFFS(RLHI.offset)+j))%16];
			}
			printf("%.*s\n",2*RLHI.length,output);
		}
		printf("\n");
	}

	// Parse head object
	svfl_rerl_header* rerl = NULL;
	svfl_ntro_header* ntro = NULL;
	char* data = NULL;
	for(i=0;i<ret->hdr->lumps.count;i++) {
		printf("[Lump] Tag: %.4s | Type: %u\n",
				RLHI.tag, ret->lumps[i].type);
		switch(ret->lumps[i].type) {
			case 1280460114: // RERL - external reference data
				rerl = (svfl_rerl_header*)&(ret->lumps[i]);
				break;
			case 1229210962: // REDI - dunno what it's used for
				break;
			case 1330795598: // NTRO - type data	
				ntro = (svfl_ntro_header*)&(ret->lumps[i]);
				break;
			case 1096040772: //DATA
				data = ret->lumps[i].content;
				break;
		}
	}
	if(rerl == NULL) {
		printf("[Lump] No RERL lump...\n");
	}
	printf("\n");

	if(data != NULL && ntro != NULL) {
		svfl_struct* rootobject = (svfl_struct*)malloc(sizeof(svfl_struct));
		rootobject->NTRO = ntro;
		rootobject->type = &(ntro->entries[0]);
		rootobject->data = data;

		//parse_object(rootobject,ntro,data);
		//fd->parsed_object = rootobject;
		printf("\n\n");

		print_object_recursive(rootobject);
	} else if (ret->hdr->version == 1) {
		// Version 1 files are stuff like panorama non-xml, and sounds
		printf("DATA LUMP START:\n");
		printf("%X\n",*(uint32_t*)data);
	} else {
		fd->parsed_object = NULL;
	}

	// Cleanup
	fd->filetype = SVF;
	fd->parsed = ret;
}

// STUB
void print_object_recursive_internal(svfl_struct* obj, uint32_t depth, svfl_ntro_entry* curtype);

/*
 * Print a single tagged item
 */
void print_thing_at_location(svfl_struct* obj, uint32_t depth, svfl_ntro_entry_tag* curtag, char* location, int indirectionlevels) {
	char* tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"+(50-depth);
	if(indirectionlevels > 0) {
		// follow the level of indirection to determine what to do
		char* baseaddr = OFFS(curtag->df->indirections.offset);
		uint32_t indirectiontype = *(uint8_t*)((curtag->df->indirections.count-indirectionlevels) + baseaddr);
		if(indirectiontype == 3) {
			printf("%s %s (ptr): ˅\n",tabs, curtag->name);
			if((*(uint32_t*)location) == 0) {
				// Null pointer
				printf("%s\t -> NULL\n",tabs);
			} else {
				// single-object pointer derefernce
				print_thing_at_location(obj, depth+1,curtag, location + *(uint32_t*)(location), indirectionlevels-1);
			}
		}
		if(indirectiontype == 4) {
			uint32_t j;
			uint32_t count = *(uint32_t*)(location + 4);
			printf("%s %s [%u]: ˅\n",tabs, curtag->name, count);
			uint32_t levelsize = 4;
			if(indirectionlevels - 1 == 0) {
				switch(curtag->df->datatype) {
					case SVFL_DATATYPE_SUBSTRUCT:
						levelsize = do_type_lookup(obj->NTRO, curtag->df->ref_typetag)->hdf->length;
						break;
					case SVFL_DATATYPE_BYTE:
					case SVFL_DATATYPE_BOOLEAN:
						levelsize = 1;
						break;
					case SVFL_DATATYPE_UINT16:
						levelsize = 2;
						break;
					case SVFL_DATATYPE_STRING4:
					case SVFL_DATATYPE_SINT:
					case SVFL_DATATYPE_ENUM:
					case SVFL_DATATYPE_NUMBER:
					case SVFL_DATATYPE_FLAGS:
					case SVFL_DATATYPE_FLOAT:
					case SVFL_DATATYPE_STRING:
					default:
						levelsize = 4;
						break;
					case SVFL_DATATYPE_EXTREF:
					case SVFL_DATATYPE_UINT64:
						levelsize = 8;
						break;
					case SVFL_DATATYPE_VEC3:
						levelsize = 12;
						break;
					case SVFL_DATATYPE_VECTOR4D:
					case SVFL_DATATYPE_QUATERNION:
					case SVFL_DATATYPE_FLTX4:
					case SVFL_DATATYPE_VEC4:
						levelsize = 16;
						break;
					case SVFL_DATATYPE_CTRANSFORM:
						levelsize = 32;
						break;
					case SVFL_DATATYPE_MATRIX3X4:
					case SVFL_DATATYPE_MATRIX3X4A:
						levelsize = 48;
						break;
				}
			}
			for(j=0;j<count;j++) {
				//printf("location will be %p\n",location + *(uint32_t*)(location) + j * levelsize);
				print_thing_at_location(obj, depth+1,curtag, location + *(uint32_t*)(location) + j * levelsize, indirectionlevels-1);
			}
		}
	} else {
		// Just print the thing at the specified address by type
		switch(curtag->df->datatype) {
			svfl_struct* child;
			case SVFL_DATATYPE_SUBSTRUCT:
				//curref = curtag->refindex;
				child = (svfl_struct*)malloc(sizeof(svfl_struct));
				child->NTRO = obj->NTRO;
				child->type = do_type_lookup(obj->NTRO, curtag->df->ref_typetag);
				child->data = location;
				print_object_recursive_internal(child, depth+1, do_type_lookup(obj->NTRO, curtag->df->ref_typetag));
				break;
			case SVFL_DATATYPE_ENUM:
				printf("%s %s: (unknown enum) %u\n",tabs,curtag->name,*(uint32_t*)(location));
				break;
			case SVFL_DATATYPE_EXTREF:
				printf("%s %s: (ext ref) %.16" PRIx64 "\n",tabs,curtag->name,*(uint64_t*)(location));
				break;
			case SVFL_DATATYPE_BYTE:
				if(curtag->df->count == 0) {
					printf("%s %s: (byte) %.2x\n",tabs,curtag->name, *(char*)(location));
				} else {
					printf("%s %s: (bytes)",tabs,curtag->name);
					uint16_t k;
					for(k=0;k<curtag->df->count;k++) {
						printf(" %.2x ",*(char*)(location));
					}
					printf("\n");
				}
				break;
			case SVFL_DATATYPE_SINT:
				printf("%s %s: (uint32) %u\n",tabs,curtag->name,*(uint32_t*)(location));
				break;
			case SVFL_DATATYPE_UINT16:
				printf("%s %s: (uint16) %hu\n",tabs,curtag->name,*(uint16_t*)(location));
				break;
			case SVFL_DATATYPE_NUMBER:
				printf("%s %s: (int32) %i\n",tabs,curtag->name,*(int32_t*)(location));
				break;
			case SVFL_DATATYPE_FLAGS:
				printf("%s %s: (flags) %08x\n",tabs,curtag->name,*(int32_t*)(location));
				break;
			case SVFL_DATATYPE_UINT64:
				printf("%s %s: (uint64) %lu\n",tabs,curtag->name,*(uint64_t*)(location));
				break;
			case SVFL_DATATYPE_FLOAT:
				printf("%s %s: (float) %f\n",tabs,curtag->name,*(float*)(location));
				break;
			case SVFL_DATATYPE_VEC3:
				printf("%s %s: (vec3) [%f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8));
				break;
			case SVFL_DATATYPE_FLTX4:
				printf("%s %s: (fltx4) [%f %f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12));
				break;
			case SVFL_DATATYPE_VEC4:
				printf("%s %s: (vec4) [%f %f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12));
				break;
			case SVFL_DATATYPE_QUATERNION:
				printf("%s %s: (quat) [x: %f y: %f z: %f w: %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12));
				break;
			case SVFL_DATATYPE_BOOLEAN:
				printf("%s %s: (bool) %hhu\n",tabs,curtag->name,*(char*)(location));
				break;
			case SVFL_DATATYPE_STRING4:
			case SVFL_DATATYPE_STRING:
				printf("%s %s: (str) '%s'\n",tabs,curtag->name,((char*)(location) + *((uint32_t*)(location))));
				break;
			case SVFL_DATATYPE_VECTOR4D:
				printf("%s %s: (vector4d) [%f %f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12));
				break;
			case SVFL_DATATYPE_MATRIX3X4:
				printf("%s %s: (matrix3x4) [ %f %f %f %f\n%s\t\t\t\t %f %f %f %f\n%s\t\t\t\t %f %f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12),
						tabs,
						*(float*)(location + 16),
						*(float*)(location + 20),
						*(float*)(location + 24),
						*(float*)(location + 28),
						tabs,
						*(float*)(location + 32),
						*(float*)(location + 36),
						*(float*)(location + 40),
						*(float*)(location + 44));
				break;
			case SVFL_DATATYPE_MATRIX3X4A:
				printf("%s %s: (matrix3x4a) [ %f %f %f %f\n%s\t\t\t\t %f %f %f %f\n%s\t\t\t\t %f %f %f %f]\n",tabs,curtag->name,
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8),
						*(float*)(location + 12),
						tabs,
						*(float*)(location + 16),
						*(float*)(location + 20),
						*(float*)(location + 24),
						*(float*)(location + 28),
						tabs,
						*(float*)(location + 32),
						*(float*)(location + 36),
						*(float*)(location + 40),
						*(float*)(location + 44));
				break;
			case SVFL_DATATYPE_CTRANSFORM:
				printf("%s %s: (CTransform(?)) [q={%f %f %f %f}, p={%f %f %f}]\n",tabs,curtag->name,
						*(float*)(location + 16),
						*(float*)(location + 20),
						*(float*)(location + 24),
						*(float*)(location + 28),
						*(float*)(location),
						*(float*)(location + 4),
						*(float*)(location + 8));
				// Note that we do skip 4 bytes in here, for alignment purposes
				break;
			default:
				printf("%s %s: (unhandled type %u)\n",tabs,curtag->name,curtag->df->datatype);
				break;
		}
	}
}

/*
 * The internal version, with specifyable depth.
 */
void print_object_recursive_internal(svfl_struct* obj, uint32_t depth, svfl_ntro_entry* curtype) {
	char* tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"+(50-depth);
	// only print type name of the childmost class
	if(obj->type == curtype)
		printf("%s%s:\n",tabs,obj->type->classname);
	// We now do a stack of types
	if(curtype->hdf->base_struct_id != 0) {
		svfl_ntro_entry* parent = do_type_lookup(obj->NTRO,curtype->hdf->base_struct_id);
		if(parent == NULL)
			fprintf(stderr, "ERROR PRINTING DATA, INHERITANCE FROM UNDEFINED TYPE!\n");
		print_object_recursive_internal(obj, depth, parent);
	}
	uint32_t i;
	for(i=0;i<curtype->hdf->num_tags;i++) {
		print_thing_at_location(obj, depth, &(curtype->tags[i]), curtype->tags[i].df->offset_in_struct + obj->data, curtype->tags[i].df->indirections.count);
	}
}

/*
 * The external version (tab reasons)
 */
void print_object_recursive(svfl_struct* obj) {
	print_object_recursive_internal(obj,0,obj->type);
}

/*
 * Look up a type entry in the ntro table by typetag.
 */
svfl_ntro_entry* do_type_lookup(svfl_ntro_header* ntro, uint32_t typetag) {
	uint32_t i;
	for(i=0;i<ntro->df->numentries;i++) {
		if(ntro->entries[i].hdf->typetag == typetag)
			return &(ntro->entries[i]);
	}
	printf("FAILED CLASS LOOKUP (%.8X)\n",typetag);
	return NULL;
}

/*
 * Cleanup
 */
void free_svf(filedata* fd) {
	if(fd->filetype != SVF) {
		fprintf(stderr,"Error: tried to use SVF free on non-SVF data.\n");
		return;
	}
	free(((svffile_generic*)fd->parsed)->lumps);
	free(fd->parsed);
	fd->parsed = NULL;
}
