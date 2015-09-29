#ifndef DECOMPILERS_H
#define DECOMPILERS_H
#include "../fileinfo.h"
#include <dirent.h>
#include <sys/types.h>

typedef void (*dectodir)(DIR* out, filedata* fd);
#endif
