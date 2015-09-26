#include "s2rd.h"


int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Error: Insufficient arguments. Use `s2rd help` for more info.\n");
		return 1;
	}
	if(strncmp(argv[1],"help",5)==0) {
		printf("Source 2 Resource Decompiler\nUsage: s2rd <action>\nActions:\n\thelp\t\t\t\tDisplay this message.\n\tdecompile <filein> <fileout>\tDecompile a file.\n");
		return 0;
	} else if(strncmp(argv[1],"decompile",10)==0) {
		if(argc < 4) {
			printf("Error: insufficient arguments. Use `s2rd help` for more info.\n");
			return 1;
		}
		
		filedata* data = loadfile(argv[2]);
		if(data == NULL) {
			printf("Error: Unable to load '%s' properly.\n",argv[1]);
		}
		return 0;
	} else {
		//printf("Error: Unknown action '%s'.\n",argv[1]);
		printf("Assuming you want just a decompile dump...\n");
		filedata* data = loadfile(argv[1]);
		if(data == NULL) {
			printf("Error: Unable to load '%s' properly.\n",argv[1]);
		}
		return 0;
	}
}
