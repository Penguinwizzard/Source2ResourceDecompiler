#include "s2rd.h"

dectodir identifydecompiler(filedata *data) {
	switch (data->filetype) {
	case SVF:;
		svffile_generic *ret = data->parsed;
		if (ret->hdr->version == 0) {
			// Look at the first struct; this should identify the
			// variant of SVF file it is
			uint32_t i;
			for (i = 0; i < ret->hdr->lumps.count; i++) {
				if (strncmp(ret->lumps[i].type_aschar, "NTRO",
				            4) == 0) {
					// introspection information!
					break;
				}
			}
			if (i == ret->hdr->lumps.count) {
				printf("not yet handled SVF type\n");
				return NULL;
			}
			svfl_ntro_header *nh =
			    (svfl_ntro_header *)&(ret->lumps[i]);
			switch (nh->entries[0].hdf->crc) {
			default:
				printf("not yet handled SVF type\n");
				return NULL;
				break;
			}
		} else if (ret->hdr->version == 1) {
			// Text container
			printf("not yet handled SVF type\n");
			return SVF1_dectodir;
		} else if (ret->hdr->version == 2) {
			// Other text container?
			printf("not yet handled SVF type\n");
			return NULL;
		}
		break;
	default:
		printf("no decompile target configured for this file type\n");
		return NULL;
		break;
	}
	return NULL;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Error: Insufficient arguments. Use `s2rd "
		                "help` for more info.\n");
		return 1;
	}
	if (strncmp(argv[1], "help", 5) == 0) {
		printf("Source 2 Resource Decompiler\nUsage: s2rd "
		       "<action>\nActions:\n\thelp\t\t\t\tDisplay this "
		       "message.\n\tdecompile <filein> <fileout>\tDecompile a "
		       "file.\n");
		return 0;
	} else if (strncmp(argv[1], "decompile", 10) == 0) {
		if (argc < 4) {
			fprintf(
			    stderr,
			    "Error: insufficient arguments. Use `s2rd help` "
			    "for more info.\n");
			return 1;
		}

		filedata *data = loadfile(argv[2]);
		if (data == NULL) {
			fprintf(stderr,
			        "Error: Unable to load '%s' properly.\n",
			        argv[1]);
		}
		identifydecompiler(data);
		return 0;
	} else {
		// printf("Error: Unknown action '%s'.\n",argv[1]);
		printf("Assuming you want just a decompile dump...\n");
		filedata *data = loadfile(argv[1]);
		if (data == NULL) {
			printf("Error: Unable to load '%s' properly.\n",
			       argv[1]);
		}
		return 0;
	}
}
