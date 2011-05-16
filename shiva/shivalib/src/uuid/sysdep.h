#include "copyrt.h"
#if defined(_WINDOWS_) || defined(_WIN32_WCE) || defined(_WINDOWS) || defined(__MINGW32__)
# define WININC 0
#endif

#ifdef WININC
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#ifdef __FreeBSD__
#include "freebsd/sys/sysinfo.h"
#else
#include <sys/sysinfo.h>
#endif
#endif

/* change to point to where MD5 .h's live; RFC 1321 has sample
   implementation */
#include "../md5/md5.h"

/* set the following to the number of 100ns ticks of the actual
   resolution of your system's clock */
#define UUIDS_PER_TICK 1024

/* Set the following to a calls to get and release a global lock */
#define LOCK
#define UNLOCK

typedef unsigned int    unsigned32;
typedef unsigned short  unsigned16;
typedef unsigned char   unsigned8;
typedef unsigned char   byte;

/* Set this to what your compiler uses for 64-bit data type */
#ifdef WININC
#define unsigned64_t unsigned __int64
#define I64(C) C
#else
#define unsigned64_t unsigned long long
#define I64(C) C##LL
#endif

typedef unsigned64_t uuid_time_t;
typedef struct {
    char nodeID[6];
} uuid_node_t;

void get_ieee_node_identifier(void* globals, uuid_node_t *node);
void set_ieee_node_identifier(void* globals, void* data, size_t sz);
void get_system_time(uuid_time_t *uuid_time);
void get_random_info(char seed[16]);
