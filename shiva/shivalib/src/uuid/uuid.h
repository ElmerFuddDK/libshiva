#include "copyrt.h"

#undef shv_uuid_t

#ifdef __GNUC__
typedef struct  __attribute__((packed)) {
    unsigned32  time_low;
    unsigned16 	time_mid;
    unsigned16 	time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} shv_uuid_t;
#else
typedef struct {
    unsigned32  time_low;
    unsigned16 	time_mid;
    unsigned16 	time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} shv_uuid_t;
#endif

void* init_globals();

/* uuid_create -- generate a UUID */
int uuid_create(void* globals, shv_uuid_t * uuid);

/* uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a
   "name" from a "name space" */
void uuid_create_md5_from_name(
    void* globals,
    shv_uuid_t *uuid,         /* resulting UUID */
    shv_uuid_t nsid,          /* UUID of the namespace */
    void *name,           /* the name from which to generate a UUID */
    int namelen           /* the length of the name */
);

/* uuid_compare --  Compare two UUID's "lexically" and return
        -1   u1 is lexically before u2
         0   u1 is equal to u2
         1   u1 is lexically after u2
   Note that lexical ordering is not temporal ordering!
*/
int uuid_compare(shv_uuid_t *u1, shv_uuid_t *u2);
