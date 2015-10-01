#include "stupidvalve.h"
#include "crc32.h"

#define SETOFFS( x , y )  x = ((char*)( y )) - ((char*)&( x ));
#define setOffsetString(a,b) strncpy(pointer, (b), 64); (a) = pointer - ((char*)&(a)); pointer += strlen(b)+1;

char* construct_file(char* sourcefilename, char* sourcefilename2, char* contentdirname, char* contents) {
	uint32_t length = strlen(sourcefilename) + strlen(contents) + strlen(contentdirname) + 400;
	svf_header* output = calloc(length,1);
	char* pointer = (char*)output;
	pointer += sizeof(svf_header);
	output->always_twelve = 12;
	output->version = 2;
	output->lumps.offset = 8;
	output->lumps.count = 2; // REDI and DATA
	svflump_header* headers = (svflump_header*)(pointer); 
	pointer += output->lumps.count*sizeof(svflump_header);
	headers[0].tag[0]='R';
	headers[0].tag[1]='E';
	headers[0].tag[2]='D';
	headers[0].tag[3]='I';
	headers[1].tag[0]='D';
	headers[1].tag[1]='A';
	headers[1].tag[2]='T';
	headers[1].tag[3]='A';
	svfl_redi_header_datafile* srhd = (svfl_redi_header_datafile*)(pointer);
	pointer += sizeof(svfl_redi_header_datafile);
	// we need this
	srhd->sourceresource.count = 2;
	srhd->sourceresourceadd.offset = 0;
	srhd->sourceresourceadd.count = 0;
	// we need this
	srhd->arguments.count = 1;
	// we need this
	srhd->namemap.count = 1;
	srhd->customdeps.offset = 0;
	srhd->customdeps.count = 0;
	srhd->additional_related.offset = 0;
	srhd->additional_related.count = 0;
	srhd->child_ref.offset = 0;
	srhd->child_ref.count = 0;
	// we need this
	srhd->extradata_int.count = 1;
	srhd->extradata_float.offset = 0;
	srhd->extradata_float.count = 0;
	srhd->extradata_string.offset = 0;
	srhd->extradata_string.count = 0;

	// Information about source files
	svfl_redi_sourceresource_datafile* sourceref = (svfl_redi_sourceresource_datafile*)pointer;
	pointer += srhd->sourceresource.count * sizeof(svfl_redi_sourceresource_datafile);
	sourceref[0].CRC = 0; // Valve generally doesn't compile a vxml, and this is just an alias, therefore the crc is 0
	sourceref[0].flags = 1;
	sourceref[1].CRC = 0; // This is generally non-zero, but it seems to be only used for checking when things need recompilation, which we're not bothering with (yet, anyway).
	sourceref[1].flags = 2;
	SETOFFS(srhd->sourceresource.offset, sourceref);
	setOffsetString(sourceref[0].offset_filename,sourcefilename);
	setOffsetString(sourceref[0].offset_modname,contentdirname);
	setOffsetString(sourceref[1].offset_filename,sourcefilename2);
	setOffsetString(sourceref[1].offset_modname,contentdirname);

	if((pointer - (char*)srhd)%4 != 0) {
		pointer += 4-((pointer-(char*)srhd)%4);
	}

	// Creation arguments
	svfl_redi_argument_datafile* tdentry = (svfl_redi_argument_datafile*)(pointer);
	pointer += srhd->arguments.count * sizeof(svfl_redi_argument_datafile);
	tdentry->fingerprint = 0;
	tdentry-> fingerprint_default = 0;
	SETOFFS(srhd->arguments.offset, tdentry);
	setOffsetString(tdentry->offset_name,"___OverrideInputData___");
	setOffsetString(tdentry->offset_type,"BinaryBlobArg");

	if((pointer - (char*)srhd)%4 != 0) {
		pointer += 4-((pointer-(char*)srhd)%4);
	}

	// Named data map
	svfl_redi_namemap_datafile* ndentry = (svfl_redi_namemap_datafile*)pointer;
	pointer += srhd->namemap.count*sizeof(svfl_redi_namemap_datafile);
	ndentry->fingerprint = 5;
	ndentry->userdata = 0;
	SETOFFS(srhd->namemap.offset,ndentry);
	setOffsetString(ndentry->offset_key,"CompilePanorama");
	setOffsetString(ndentry->offset_expanded,"Panorama Compiler Version");

	if((pointer - (char*)srhd)%8 != 0) {
		pointer += 8-((pointer-(char*)srhd)%8);
	}

	// Extra int data
	svfl_redi_extradata_int_datafile* sdentry = (svfl_redi_extradata_int_datafile*)pointer;
	pointer += srhd->extradata_int.count*sizeof(svfl_redi_extradata_int_datafile);
	sdentry->value = 0;
	SETOFFS(srhd->extradata_int.offset,sdentry);
	setOffsetString(sdentry->offset_key,"IsChildResource");

	// ok, that's it for REDI
	SETOFFS(headers[0].offset,srhd);
	headers[0].length = pointer - ((char*)srhd);
	if((pointer - (char*)output)%16 != 0) {
		pointer += 16-((pointer-(char*)output)%16);
	}
	// The 16-byte alignment for DATA isn't factored into block space
	SETOFFS(headers[1].offset,pointer);
	headers[1].length = strlen(contents) + 6;
	// The 4-byte header from hell, and two bytes of 0
	*((uint32_t*)pointer) = crc32(0,contents,strlen(contents));
	pointer += 4;
	*((uint16_t*)pointer) = 0;
	pointer += 2;
	strcpy(pointer,contents);
	length = pointer + strlen(contents) - (char*)output;
	output->filelength = length;
	return (char*)output;
}

int main(int argc, char** argv) {
	if(argc < 6) {
		printf("Penguinwizzard Panorama Compiler v0.1\nUsage: ./panoramacompiler <filename> <outputfile> <internalfilename> <internalfilename2> <contentdirectory>\nThe last two are used inside the file for reference info.\n");
		return 1;
	}
	FILE* f = fopen(argv[1],"r");
	if(f == NULL) {
		printf("File read error; error code is %i\n",errno);
		return 1;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buffer = malloc(size+1);
	long num_ret = fread(buffer, size, 1, f);
	if(num_ret != 1) {
		printf("Warning: File size is being wonky; expected 1 but got %li\n", num_ret);
	}
	fclose(f);
	buffer[size]='\0';
	char* output = construct_file(argv[3],argv[4],argv[5],buffer);
	FILE* out = fopen(argv[2],"w");
	if(out == NULL) {
		printf("File open error; could not open output file; error code is %i\n",errno);
		return 1;
	}
	fwrite(output,*((uint32_t*)output),1,out);
	fclose(out);
	printf("finished!\n");
	return 0;
}
