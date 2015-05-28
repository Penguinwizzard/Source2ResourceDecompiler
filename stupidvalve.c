#include "stupidvalve.h"

const char downconvert[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

/*
 * This new valve file format doesn't have type specifiers, so it's awful to handle.
 */
void parse_svf(filedata* fd) {
	svffile_generic* ret = (svffile_generic*)malloc(sizeof(svffile_generic));
	ret->hdr = (svf_header*)(fd->contents);
	if(ret->hdr->filelength != fd->length) {
		fprintf(stderr,"Warning: SVF header length does not match file length.\n");
	}
	ret->lumpheaders = (svflump_header*)(fd->contents + 0x10);
	ret->lumps = (svfl_header*)malloc(sizeof(svfl_header)*ret->hdr->numlumps);
	uint32_t i;
	for(i=0;i<ret->hdr->numlumps;i++) {
		printf("%c%c%c%c: %u %u\n",ret->lumpheaders[i].tag[0],ret->lumpheaders[i].tag[1],ret->lumpheaders[i].tag[2],ret->lumpheaders[i].tag[3],ret->lumpheaders[i].offset,ret->lumpheaders[i].length);
		ret->lumps[i].content = (((char*)&(ret->lumpheaders[i].offset)) + ret->lumpheaders[i].offset);
		// because dealing with these is a lot faster than doing string checks all the time
		ret->lumps[i].type_aschar[0] = ret->lumpheaders[i].tag[0];
		ret->lumps[i].type_aschar[1] = ret->lumpheaders[i].tag[1];
		ret->lumps[i].type_aschar[2] = ret->lumpheaders[i].tag[2];
		ret->lumps[i].type_aschar[3] = ret->lumpheaders[i].tag[3];
		if(strncmp(ret->lumpheaders[i].tag,"RERL",4) == 0) {
			//Parse as a Resource External Reference block
			svfl_rerl_header* rh = (svfl_rerl_header*)&(ret->lumps[i]);
			rh->df = (svfl_rerl_header_datafile*)rh->content;
			rh->entries = (svfl_rerl_entry*)malloc(rh->df->numentries * sizeof(svfl_rerl_entry));
			svfl_rerl_entry_datafile* dfs = (svfl_rerl_entry_datafile*)(((char*)rh->df)+sizeof(svfl_rerl_header_datafile)); 
			uint32_t j;
			for(j=0;j<rh->df->numentries;j++) {
				rh->entries[j].df = &(dfs[j]);
				rh->entries[j].content = ((char*)&rh->entries[j].df->offset) + rh->entries[j].df->offset;
				printf("%.16" PRIx64 ": %s\n",rh->entries[j].df->objecttag,rh->entries[j].content);
			}
		} else if(strncmp(ret->lumpheaders[i].tag,"REDI",4) == 0) {
			//Parse as a Resource Deferred Reference block
			svfl_redi_header* rh = (svfl_redi_header*)&(ret->lumps[i]);
			rh->df = (svfl_redi_header_datafile*)rh->content;
			uint32_t j;
			// Unknown multisub
			{
				printf("SubBlock unknown1 (%u entries)\n",rh->df->unknown1.count);
			}
			// Sourceresourceref multisub
			{
				printf("SubBlock sourceresource (%u entries)\n",rh->df->sourceresource.count);
				rh->srentries = (svfl_redi_sourceresource*)malloc(rh->df->sourceresource.count * sizeof(svfl_redi_sourceresource));
				svfl_redi_sourceresource_datafile* srsd = (svfl_redi_sourceresource_datafile*)(((char*)&(rh->df->sourceresource.offset)) + rh->df->sourceresource.offset);
				for(j=0;j<rh->df->sourceresource.count;j++) {
					rh->srentries[j].filename = ((char*)(&srsd[j].offset_filename)) + srsd[j].offset_filename;
					rh->srentries[j].mod = ((char*)(&srsd[j].offset_modname)) + srsd[j].offset_modname;
					printf("\tfile: %s in mod: %s\n",rh->srentries[j].filename,rh->srentries[j].mod);
				}
			}
			// Typeddata multisub
			{
				printf("SubBlock typeddata (%u entries)\n",rh->df->typeddata.count);
				rh->tdentries = (svfl_redi_typeddata*)malloc(rh->df->typeddata.count*sizeof(svfl_redi_typeddata));
				svfl_redi_typeddata_datafile* srtd = (svfl_redi_typeddata_datafile*)(((char*)&(rh->df->typeddata.offset)) + rh->df->typeddata.offset);
				for(j=0;j<rh->df->typeddata.count;j++) {
					rh->tdentries[j].df = &(srtd[j]);
					rh->tdentries[j].name = ((char*)(&srtd[j].offset_name)) + srtd[j].offset_name;
					rh->tdentries[j].type = ((char*)(&srtd[j].offset_type)) + srtd[j].offset_type;
					printf("\t%s %s",rh->tdentries[j].type,rh->tdentries[j].name);
					if(srtd[j].flags1 != 0)
						printf(" (nameflags = %.8X)",srtd[j].flags1);
					if(srtd[j].flags2 != 0)
						printf(" (typeflags = %.8X)",srtd[j].flags2);
					printf("\n");
				}
			}
			// Namemap multisub
			{
				printf("SubBlock namemap (%u entries)\n",rh->df->namemap.count);
				rh->nmentries = (svfl_redi_namemap*)malloc(rh->df->namemap.count*sizeof(svfl_redi_namemap));
				svfl_redi_namemap_datafile* srnd = (svfl_redi_namemap_datafile*)(((char*)&(rh->df->namemap.offset)) + rh->df->namemap.offset);
				for(j=0;j<rh->df->namemap.count;j++) {
					rh->nmentries[j].df = &(srnd[j]);
					rh->nmentries[j].key = ((char*)(&srnd[j].offset_key)) + srnd[j].offset_key;
					rh->nmentries[j].expanded = ((char*)(&srnd[j].offset_expanded)) + srnd[j].offset_expanded;
					printf("\t%s -> %s (val:%x)\n",rh->nmentries[j].key,rh->nmentries[j].expanded,rh->nmentries[j].df->unk1);
				}
			}
			// Unknown  multisub
			{
				printf("SubBlock unknown2 (%u entries)\n",rh->df->unknown2.count);
			}
			// Unknown multisub
			{
				printf("SubBlock unknown3 (%u entries)\n",rh->df->unknown3.count);
			}
			// DeferredRef multisub
			{
				printf("SubBlock deferredref (%u entries)\n",rh->df->deferredref.count);
				rh->drentries = (svfl_redi_deferredref*)malloc(rh->df->deferredref.count*sizeof(svfl_redi_deferredref));
				svfl_redi_deferredref_datafile* srdd = (svfl_redi_deferredref_datafile*)(((char*)&(rh->df->deferredref.offset)) + rh->df->deferredref.offset);
				for(j=0;j<rh->df->deferredref.count;j++) {
					rh->drentries[j].df = &(srdd[j]);
					rh->drentries[j].content = ((char*)(&srdd[j].offset)) + srdd[j].offset;
					printf("\t%.16" PRIx64 ": %s\n",rh->drentries[j].df->objecttag,rh->drentries[j].content);
				}
			}
			// SpecialData multisub
			{
				printf("SubBlock specialdata (%u entries)\n",rh->df->specialdata.count);
				rh->sdentries = (svfl_redi_specialdata*)malloc(rh->df->specialdata.count*sizeof(svfl_redi_specialdata));
				svfl_redi_specialdata_datafile* srsd = (svfl_redi_specialdata_datafile*)(((char*)&(rh->df->specialdata.offset)) + rh->df->specialdata.offset);
				for(j=0;j<rh->df->specialdata.count;j++) {
					rh->sdentries[j].df = &(srsd[j]);
					rh->sdentries[j].key = ((char*)(&srsd[j].offset_key)) + srsd[j].offset_key;
					printf("\t%s: %i\n",rh->sdentries[j].key,rh->sdentries[j].df->value);
				}
			}
			// Unknown multisub
			{
				printf("SubBlock unknown4 (%u entries)\n",rh->df->unknown4.count);
			}
			// Unknown multisub
			{
				printf("SubBlock unknown5 (%u entries)\n",rh->df->unknown5.count);
			}
		} else if(strncmp(ret->lumpheaders[i].tag,"NTRO",4) == 0) {
			svfl_ntro_header* nh = (svfl_ntro_header*)&(ret->lumps[i]);
			nh->df = (svfl_ntro_header_datafile*)nh->content;
			printf("\tversion: %i\n",nh->df->version);
			nh->entries = (svfl_ntro_entry*)malloc(nh->df->numentries * sizeof(svfl_ntro_entry));
			uint32_t j;
			for(j=0;j < nh->df->numentries;j++) {
				nh->entries[j].hdf = (svfl_ntro_entry_header_datafile*)(((uint8_t*)&nh->df->offset_entries)+nh->df->offset_entries+j*sizeof(svfl_ntro_entry_header_datafile));
				nh->entries[j].classname = ((char*)(&nh->entries[j].hdf->offset_classname)) + nh->entries[j].hdf->offset_classname;
				printf("\t%i: %s (length %u, type tag %.8X)\n",j,nh->entries[j].classname,nh->entries[j].hdf->length, nh->entries[j].hdf->typetag);
				printf("\t%u %.8X %u   %X %X %X %X\n",
						nh->entries[j].hdf->version,
						nh->entries[j].hdf->crc,
						nh->entries[j].hdf->user_version,
						nh->entries[j].hdf->length,
						nh->entries[j].hdf->alignment,
						nh->entries[j].hdf->base_struct_id,
						nh->entries[j].hdf->unknown7);
				nh->entries[j].tags = (svfl_ntro_entry_tag*)malloc(nh->entries[j].hdf->num_tags * sizeof(svfl_ntro_entry_tag));
				svfl_ntro_entry_tag_datafile* ths = (svfl_ntro_entry_tag_datafile*)(((char*)(&nh->entries[j].hdf->offset_tagheaders)) + nh->entries[j].hdf->offset_tagheaders);
				int numrefs = 0;
				uint32_t k;
				for(k=0;k < nh->entries[j].hdf->num_tags; k++) {
					nh->entries[j].tags[k].df = &(ths[k]);
					nh->entries[j].tags[k].name = ((char*)(&nh->entries[j].tags[k].df->offset_tagname)) + nh->entries[j].tags[k].df->offset_tagname;
					switch(nh->entries[j].tags[k].df->datatype) {
						case 1:
							printf("\t\t%s (%.4hX %.4hX %u %u %u), reference of type %.8X\n",
								nh->entries[j].tags[k].name,
								nh->entries[j].tags[k].df->count,
								nh->entries[j].tags[k].df->offset_in_struct,
								nh->entries[j].tags[k].df->unknown2,
								nh->entries[j].tags[k].df->unknown3,
								nh->entries[j].tags[k].df->datatype,
								nh->entries[j].tags[k].df->ref_typetag);
							nh->entries[j].tags[k].refindex = numrefs;
							numrefs++;
							break;
						default:
						printf("\t\t%s (%.4hX %.4hX %u %u %.8X %u)\n",
								nh->entries[j].tags[k].name,
								nh->entries[j].tags[k].df->count,
								nh->entries[j].tags[k].df->offset_in_struct,
								nh->entries[j].tags[k].df->unknown2,
								nh->entries[j].tags[k].df->unknown3,
								nh->entries[j].tags[k].df->ref_typetag,
								nh->entries[j].tags[k].df->datatype);
							nh->entries[j].tags[k].refindex = -1;
					}
				}
				nh->entries[j].numrefs = numrefs;
			}
		} else if(strncmp(ret->lumpheaders[i].tag,"DATA",4) == 0) {
			printf("\tData lump of length %u.\n",ret->lumpheaders[i].length);
			// We don't parse it until later
		} else {
			printf("\tUNHANDLED LUMP TYPE, POKE PWIZ\n");
			char output[2*ret->lumpheaders[i].length];
			uint32_t j;
			for(j=0;j<ret->lumpheaders[i].length;j++) {
				output[2*j] = (char)downconvert[*((uint8_t*)(((char*)&(ret->lumpheaders[i].offset))+j+ret->lumpheaders[i].offset))/16];
				output[2*j+1] = (char)downconvert[*((uint8_t*)(((char*)&(ret->lumpheaders[i].offset))+j+ret->lumpheaders[i].offset))%16];
			}
			printf("%.*s\n",2*ret->lumpheaders[i].length,output);
		}
	}
	// Parse head object
	svfl_rerl_header* rerl = NULL;
	svfl_ntro_header* ntro = NULL;
	char* data = NULL;
	for(i=0;i<ret->hdr->numlumps;i++) {
		printf("lump type is %u and tag is %.4s\n",ret->lumps[i].type,ret->lumpheaders[i].tag);
		switch(ret->lumps[i].type) {
			case 1280460114: // RERL - external reference data
				rerl = (svfl_rerl_header*)&(ret->lumps[i]);
				break;
			case 1229210962: // REDI - dunno what it's used for
				break;
			case 1330795598: // NTRO - type data	
				ntro = (svfl_ntro_header*)&(ret->lumps[i]);
				break;
			case 1096040772:
				data = ret->lumps[i].content;
				break;
		}
	}
	if(data != NULL && ntro != NULL) {
		svfl_struct* rootobject = (svfl_struct*)malloc(sizeof(svfl_struct));
		rootobject->type = &(ntro->entries[0]);
		rootobject->data = data;
		parse_object(rootobject,ntro,data);
		fd->parsed_object = rootobject;
		print_object_recursive(rootobject);
	} else {
		fd->parsed_object = NULL;
	}
	if(rerl == NULL) {
		fprintf(stderr, "Info: No RERL lump...\n");
	}

	// Cleanup
	fd->filetype = SVF;
	fd->parsed = ret;
}

/*
 * Walk the ntro tree for the data.
 * Note that this assumes that the type and content pointer for the object have been set.
 * Basically just handles making the other info. 
 */
void parse_object(svfl_struct* object, svfl_ntro_header* ntro, char* data) {
	uint32_t i;
	if(object->type->numrefs > 0) {
		object->children = (svfl_struct**)malloc(object->type->numrefs * sizeof(svfl_struct));
	} else {
		object->children = NULL;
	}
	for(i=0;i<object->type->hdf->num_tags;i++) {
		int curref = object->type->tags[i].refindex;
		if(curref == -1) {
			continue;
		}
		// Is this how we indicate contained structs vs referenced structs?
		// Since these can be multiple (because of arrays and the like) we have to allocate arrays and the like.
		if(object->type->tags[i].df->unknown3 == 1) {
			uint32_t offset = *((uint32_t*)(object->type->tags[i].df->offset_in_struct + object->data));
			uint32_t count = *((uint32_t*)(object->type->tags[i].df->offset_in_struct + object->data + 4));
			if(count > 0) {
				object->children[curref] = (svfl_struct*)malloc(count*sizeof(svfl_struct));
				uint32_t j;
				svfl_ntro_entry* childtype = do_type_lookup(ntro, object->type->tags[i].df->ref_typetag);
				for(j=0;j<count;j++) {
					object->children[curref][j].type = childtype;
					object->children[curref][j].data = object->type->tags[i].df->offset_in_struct + object->data + offset + childtype->hdf->length * j;
					parse_object(&(object->children[curref][j]),ntro,data);
				}
			}
			else {
				object->children[curref] = NULL;
			}
		} else {
			// Note: figure out when this isn't 1
			printf("potential multiple (in %s)?\n",object->type->classname);
			uint32_t count = 1; // PROBABLY INCORRECT
			object->children[curref] = (svfl_struct*)malloc(count*sizeof(svfl_struct));
			uint32_t j;
			svfl_ntro_entry* childtype = do_type_lookup(ntro, object->type->tags[i].df->ref_typetag);
			for(j=0;j<count;j++) {
				object->children[curref][j].type = childtype;
				object->children[curref][j].data = object->data + object->type->tags[i].df->offset_in_struct + childtype->hdf->length * j;
				parse_object(&(object->children[curref][j]),ntro,data);
			}
		}
		//uint32_t count;
		//uint32_t length;
		//object->children[curref].type = do_type_lookup(ntro,object->type->tags[i].df->ref_typetag);
		//object->children[curref].content = data + object->
	}
	printf("finished parsing a %s at %x\n",object->type->classname,(uint32_t)(object->data - data));
}

/*
 * The internal version, with specifyable depth.
 */
void print_object_recursive_internal(svfl_struct* obj, uint32_t depth) {
	char tabs[31];
	snprintf(tabs,30,"%*.s",depth,"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	printf("%s%s:\n",tabs,obj->type->classname);
	uint32_t i;
	for(i=0;i<obj->type->hdf->num_tags;i++) {
		uint32_t j;
		int curref;
		switch(obj->type->tags[i].df->datatype) {
			case SVFL_DATATYPE_SUBSTRUCT:
				curref = obj->type->tags[i].refindex;
				if(obj->type->tags[i].df->unknown3 == 1) {
					uint32_t count = *(uint32_t*)(obj->type->tags[i].df->offset_in_struct + obj->data + 4);
					printf("%s %s: (%u)˅\n",tabs,obj->type->tags[i].name,count);
					for(j=0;j<count;j++) {
						print_object_recursive_internal(&(obj->children[curref][j]),depth+1);
					}
				} else {
					uint32_t count = 1;
					printf("%s %s: (%u)˅\n",tabs,obj->type->tags[i].name,count);
					for(j=0;j<count;j++) {
						print_object_recursive_internal(&(obj->children[curref][j]),depth+1);
					}
				}
				break;
			case SVFL_DATATYPE_ENUM:
				printf("%s %s: %u (unknown enum type)\n",tabs,obj->type->tags[i].name,*(uint32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_EXTREF:
				if(obj->type->tags[i].df->unknown3 == 1) {
					uint32_t count = *(uint32_t*)(obj->type->tags[i].df->offset_in_struct + obj->data + 4);
					for(j=0;j<count;j++) {
						printf("%s %s: external reference to %.16" PRIx64 "\n",tabs,obj->type->tags[i].name,*(uint64_t*)((char*)(obj->data + obj->type->tags[i].df->offset_in_struct) + *((uint32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct)) + j * 8) );
					}
				} else {
					printf("%s %s: external reference to %.16" PRIx64 "\n",tabs,obj->type->tags[i].name,*(uint64_t*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				}
				break;
			case SVFL_DATATYPE_BYTE:
				if(obj->type->tags[i].df->count == 0) {
					printf("%s %s: %.2x\n",tabs,obj->type->tags[i].name, *(char*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				} else {
					printf("%s %s: ",tabs,obj->type->tags[i].name);
					uint16_t k;
					for(k=0;k<obj->type->tags[i].df->count;k++) {
						printf("%.2x",*(char*)(obj->data + obj->type->tags[i].df->offset_in_struct));
					}
					printf("\n");
				}
				break;
			case SVFL_DATATYPE_SINT:
				printf("%s %s: %u\n",tabs,obj->type->tags[i].name,*(uint32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_NUMBER:
				printf("%s %s: %i\n",tabs,obj->type->tags[i].name,*(int32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_FLAGS:
				printf("%s %s: %x (flags)\n",tabs,obj->type->tags[i].name,*(int32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_FLOAT:
				printf("%s %s: %f\n",tabs,obj->type->tags[i].name,*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_VEC3:
				printf("%s %s: [%f %f %f]\n",tabs,obj->type->tags[i].name,
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct),
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct + 4),
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct + 8));
				break;
			case SVFL_DATATYPE_VEC4:
				printf("%s %s: [%f %f %f %f]\n",tabs,obj->type->tags[i].name,
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct),
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct + 4),
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct + 8),
						*(float*)(obj->data + obj->type->tags[i].df->offset_in_struct + 12));
				break;
			case SVFL_DATATYPE_BOOLEAN:
				printf("%s %s: %hhu\n",tabs,obj->type->tags[i].name,*(char*)(obj->data + obj->type->tags[i].df->offset_in_struct));
				break;
			case SVFL_DATATYPE_STRING4:
			case SVFL_DATATYPE_STRING:
				printf("%s %s: %s\n",tabs,obj->type->tags[i].name,((char*)(obj->data + obj->type->tags[i].df->offset_in_struct) + *((uint32_t*)(obj->data + obj->type->tags[i].df->offset_in_struct))));
				break;
			default:
				printf("%s %s: unhandled type: %u\n",tabs,obj->type->tags[i].name,obj->type->tags[i].df->datatype);
				break;
		}
	}
}

/*
 * The external version (tab reasons)
 */
void print_object_recursive(svfl_struct* obj) {
	print_object_recursive_internal(obj,0);
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
