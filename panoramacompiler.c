#include "stupidvalve.h"


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
	srhd->typeddata.count = 1;
	// we need this
	srhd->namemap.count = 1;
	srhd->unknown2.offset = 0;
	srhd->unknown2.count = 0;
	srhd->unknown3.offset = 0;
	srhd->unknown3.count = 0;
	srhd->deferredref.offset = 0;
	srhd->deferredref.count = 0;
	// we need this
	srhd->specialdata.count = 1;
	srhd->unknown4.offset = 0;
	srhd->unknown4.count = 0;
	srhd->unknown5.offset = 0;
	srhd->unknown5.count = 0;
	svfl_redi_sourceresource_datafile* sourceref = (svfl_redi_sourceresource_datafile*)(((char*)srhd) + sizeof(svfl_redi_header_datafile));
	sourceref->unknown1 = 0;
	sourceref->unknown2 = 2;
	srhd->sourceresource.offset = ((char*)sourceref) - ((char*)&(srhd->sourceresource.offset));
	// we'll have the strings later

	svfl_redi_typeddata_datafile* tdentry = (svfl_redi_typeddata_datafile*)(((char*)sourceref) + sizeof(svfl_redi_sourceresource_datafile));
	tdentry->flags1 = 0;
	tdentry-> flags2 = 0;
	srhd->typeddata.offset = ((char*)tdentry) - ((char*)&(srhd->typeddata.offset));
	// we'll have the strings later

	svfl_redi_namemap_datafile* ndentry = (svfl_redi_namemap_datafile*)(((char*)tdentry) + sizeof(svfl_redi_typeddata_datafile));
	ndentry->unk1 = 5;
	ndentry->unk2 = 0;
	srhd->namemap.offset = ((char*)ndentry) - ((char*)&(srhd->namemap.offset));
	//we'll have the strings later

	svfl_redi_specialdata_datafile* sdentry = (svfl_redi_specialdata_datafile*)(((char*)ndentry) + sizeof(svfl_redi_namemap_datafile));
	sdentry->value = 0;
	srhd->specialdata.offset = ((char*)sdentry) - ((char*)&(srhd->specialdata.offset));

	//Ok, now for strings
	char* pointer = (char*)(((char*)sdentry) + sizeof(svfl_redi_specialdata_datafile));

#define setOffsetString(a,b) strncpy(pointer, (b), 64); (a) = pointer - ((char*)&(a)); pointer += strlen(b)+1;

	setOffsetString(sourceref->offset_filename,sourcefilename);
	setOffsetString(sourceref->offset_modname,contentdirname);
	setOffsetString(tdentry->offset_name,"__OverrideInputData");
	setOffsetString(tdentry->offset_type,"BinaryBlobArg");
	setOffsetString(ndentry->offset_expanded,"Panorama Compiler Version");
	setOffsetString(ndentry->offset_key,"CompilePanorama");
	setOffsetString(sdentry->offset_key,"IsChildResource");

	// ok, that's it for REDI
	headers[0].offset = ((char*)srhd)-(char*)&(headers[0].offset);
	headers[0].length = pointer - ((char*)srhd);
	headers[1].offset = pointer-(char*)&(headers[1].offset);
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
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buffer = malloc(size+1);
	fread(buffer, size, 1, f);
	fclose(f);
	buffer[size]='\0';
	char* output = construct_file(argv[3],argv[4],buffer);
	FILE* out = fopen(argv[2],"w");
	fwrite(output,*((uint32_t*)output),1,out);
	fclose(out);
	printf("finished!\n");
	return 0;
}
