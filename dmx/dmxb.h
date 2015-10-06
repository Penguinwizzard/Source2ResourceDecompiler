#ifndef DMXB_H
#define DMXB_H
#include "dmx.h"

#define V3(fname, val) fname ## _ ## val
#define V2(fname, val) V3(fname, val)
#define V(fname) V2(fname, DMXB )

#define DMXB 1
#include "dmxbraw.h"
#undef DMXB
#define DMXB 2
#include "dmxbraw.h"
#undef DMXB
#define DMXB 3
#include "dmxbraw.h"
#undef DMXB
#define DMXB 4
#include "dmxbraw.h"
#undef DMXB
#define DMXB 5
#include "dmxbraw.h"
#undef DMXB
#define DMXB 6
#include "dmxbraw.h"
#undef DMXB
#define DMXB 7
#include "dmxbraw.h"
#undef DMXB
#define DMXB 8
#include "dmxbraw.h"
#undef DMXB
#define DMXB 9
#include "dmxbraw.h"
#undef DMXB

#undef V3
#undef V2
#undef V

#endif
