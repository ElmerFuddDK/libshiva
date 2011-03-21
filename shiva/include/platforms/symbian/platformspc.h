#define NOTIMPLEMENTED

#include <stddef.h>
#include <e32def.h>
#include <stdlib.h>

#define SHVVA_LIST VA_LIST
#define SHVVA_START(x,y) VA_START(x,y)
#define SHVVA_END(x) VA_END(x)
#define SHVVA_COPY(x,y) x[0] = y[0]
#define SHVVA_ARG(x,y)  VA_ARG(x,y)
