#include "stupidvalve.h"

#define SETOFFS( x , y )  x = ((char*)( y )) - ((char*)&( x ));

char* construct_file(char* sourcefilename, char* contentdirname, char* contents) {
	uint32_t length = strlen(sourcefilename) + strlen(contents) + strlen(contentdirname) + 400;
	svf_header* output = malloc(length);
	output->unknown_1 = 12;
	output->unknown_2 = 8;
	output->numlumps = 2; // REDI and DATA
	svflump_header* headers = (svflump_header*)(((char*)output) + sizeof(svf_header)); 
	headers[0].tag[0]='R';
	headers[0].tag[1]='E';
	headers[0].tag[2]='D';
	headers[0].tag[3]='I';
	headers[1].tag[0]='D';
	headers[1].tag[1]='A';
	headers[1].tag[2]='T';
	headers[1].tag[3]='A';
	svfl_redi_header_datafile* srhd = (svfl_redi_header_datafile*)(((char*)headers) + output->numlumps*sizeof(svflump_header));
	// we need this
	srhd->sourceresource.count = 1;
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
	svfl_redi_sourceresource_datafile* sourceref = (svfl_redi_sourceresource_datafile*)(((char*)srhd) + sizeof(svfl_redi_header_datafile));
	sourceref->CRC = 0; // Who cares about passing tests?
	sourceref->flags = 2;
	SETOFFS(srhd->sourceresource.offset, sourceref);
	// we'll have the strings later

	svfl_redi_argument_datafile* tdentry = (svfl_redi_argument_datafile*)(((char*)sourceref) + sizeof(svfl_redi_sourceresource_datafile));
	tdentry->fingerprint = 0;
	tdentry-> fingerprint_default = 0;
	SETOFFS(srhd->arguments.offset, tdentry);
	// we'll have the strings later

	svfl_redi_namemap_datafile* ndentry = (svfl_redi_namemap_datafile*)(((char*)tdentry) + sizeof(svfl_redi_argument_datafile));
	ndentry->fingerprint = 5;
	ndentry->userdata = 0;
	SETOFFS(srhd->namemap.offset,ndentry);
	//we'll have the strings later

	svfl_redi_extradata_int_datafile* sdentry = (svfl_redi_extradata_int_datafile*)(((char*)ndentry) + sizeof(svfl_redi_namemap_datafile));
	sdentry->value = 0;
	SETOFFS(srhd->extradata_int.offset,sdentry);

	//Ok, now for strings
	char* pointer = (char*)(((char*)sdentry) + sizeof(svfl_redi_extradata_int_datafile));

#define setOffsetString(a,b) strncpy(pointer, (b), 64); (a) = pointer - ((char*)&(a)); pointer += strlen(b)+1;

	setOffsetString(sourceref->offset_filename,sourcefilename);
	setOffsetString(sourceref->offset_modname,contentdirname);
	setOffsetString(tdentry->offset_name,"__OverrideInputData");
	setOffsetString(tdentry->offset_type,"BinaryBlobArg");
	setOffsetString(ndentry->offset_expanded,"Panorama Compiler Version");
	setOffsetString(ndentry->offset_key,"CompilePanorama");
	setOffsetString(sdentry->offset_key,"IsChildResource");

	// ok, that's it for REDI
	SETOFFS(headers[0].offset,srhd);
	headers[0].length = pointer - ((char*)srhd);
	SETOFFS(headers[1].offset,pointer);
	headers[1].length = strlen(contents) + 6;
	strncpy(pointer,"",6);
	pointer += 6;
	strcpy(pointer,contents);
	length = pointer + strlen(contents) - (char*)output;
	output->filelength = length;
	return (char*)output;
}

int main(int argc, char** argv) {
	if(argc < 5) {
		printf("Penguinwizzard Panorama Compiler v0.1\nUsage: ./panoramacompiler <filename> <outputfile> <internalfilename> <contentdirectory>\nThe last two are used inside the file for reference info.\n");
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
	char* output = construct_file(argv[3],argv[4],buffer);
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
