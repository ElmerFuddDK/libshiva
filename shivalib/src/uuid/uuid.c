#include "copyrt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if !defined(_WIN32_WCE) && !defined(__MINGW32__)
# include <time.h>
# if !defined(_WINDOWS_) && !defined(_WINDOWS)
#  include <arpa/inet.h>
# endif
#else
# include <winsock.h>
#endif
#include "sysdep.h"
#include "uuid.h"
#include "globals.h"

/* various forward declarations */
static int read_state(void* globals, unsigned16 *clockseq, uuid_time_t *timestamp,
    uuid_node_t *node);
static void write_state(void* globals, unsigned16 clockseq, uuid_time_t timestamp,
    uuid_node_t node);
static void format_uuid_v1(uuid_t *uuid, unsigned16 clockseq,
    uuid_time_t timestamp, uuid_node_t node);
static void format_uuid_v3or5(uuid_t *uuid, unsigned char hash[16],
    int v);
static void get_current_time(void* globals, uuid_time_t *timestamp);
static unsigned16 true_random(void);

void* init_globals()
{
     void* retVal;
	 
	 retVal = malloc(sizeof(uuid_globals_t));
     memset(retVal,0,sizeof(uuid_globals_t));
     return retVal;
}

/* uuid_create -- generator a UUID */
int uuid_create(void* globals, uuid_t *uuid)
{
     uuid_time_t timestamp, last_time;
     unsigned16 clockseq;
     uuid_node_t node;
     uuid_node_t last_node;
     int f;

     /* acquire system-wide lock so we're alone */
     LOCK;
     /* get time, node ID, saved state from non-volatile storage */
     get_current_time(globals, &timestamp);
     get_ieee_node_identifier(globals,&node);
     f = read_state(globals, &clockseq, &last_time, &last_node);

     /* if no NV state, or if clock went backwards, or node ID
        changed (e.g., new network card) change clockseq */
     if (!f || memcmp(&node, &last_node, sizeof node))
         clockseq = true_random();
     else if (timestamp < last_time)
         clockseq++;

     /* save the state for next time */
     write_state(globals, clockseq, timestamp, node);

     UNLOCK;

     /* stuff fields into the UUID */
     format_uuid_v1(uuid, clockseq, timestamp, node);
     return 1;
}

/* format_uuid_v1 -- make a UUID from the timestamp, clockseq,
                     and node ID */
void format_uuid_v1(uuid_t* uuid, unsigned16 clock_seq,
                    uuid_time_t timestamp, uuid_node_t node)
{
    /* Construct a version 1 uuid with the information we've gathered
       plus a few constants. */
    uuid->time_low = (unsigned long)(timestamp & 0xFFFFFFFF);
    uuid->time_mid = (unsigned short)((timestamp >> 32) & 0xFFFF);
    uuid->time_hi_and_version =
        (unsigned short)((timestamp >> 48) & 0x0FFF);
    uuid->time_hi_and_version |= (1 << 12);
    uuid->clock_seq_low = clock_seq & 0xFF;
    uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    memcpy(&uuid->node, &node, sizeof uuid->node);
}

/* read_state -- read UUID generator state from non-volatile store */
int read_state(void* globals, unsigned16 *clockseq, uuid_time_t *timestamp,
               uuid_node_t *node)
{
GLOB;

    /* only need to read state once per boot */
	if (!g->state_inited) {
        return 0;
    }
    *clockseq = g->st.cs;
    *timestamp = g->st.ts;
    *node = g->st.node;
    return 1;
}

/* write_state -- save UUID generator state back to non-volatile
   storage */
void write_state(void* globals, unsigned16 clockseq, uuid_time_t timestamp,
                 uuid_node_t node)
{
GLOB;

	if (!g->state_inited) {
        g->state_inited = 1;
    }

    /* always save state to volatile shared state */
    g->st.cs = clockseq;
    g->st.ts = timestamp;
    g->st.node = node;
}

/* get-current_time -- get time as 60-bit 100ns ticks since UUID epoch.
   Compensate for the fact that real clock resolution is
   less than 100ns. */
void get_current_time(void* globals, uuid_time_t *timestamp)
{
GLOB;
    uuid_time_t time_now;

    if (!g->current_time_inited) {
        get_system_time(&time_now);
		g->current_time_uuids_this_tick = UUIDS_PER_TICK;
        g->current_time_inited = 1;
    }

    for ( ; ; ) {
        get_system_time(&time_now);

        /* if clock reading changed since last UUID generated, */
		if (g->current_time_last != time_now) {
            /* reset count of uuids gen'd with this clock reading */
            g->current_time_uuids_this_tick = 0;
            g->current_time_last = time_now;
            break;
        }
        if (g->current_time_uuids_this_tick < UUIDS_PER_TICK) {
            g->current_time_uuids_this_tick++;
            break;
        }

        /* going too fast for our clock; spin */
    }
    /* add the count of uuids to low order bits of the clock reading */
	*timestamp = time_now + g->current_time_uuids_this_tick;
}

/* true_random -- generate a crypto-quality random number.
   **This sample doesn't do that.** */
static unsigned16 true_random(void)
{
    static int inited = 0;
    uuid_time_t time_now;

    if (!inited) {
        get_system_time(&time_now);
        time_now = time_now / UUIDS_PER_TICK;
        srand((unsigned int)
               (((time_now >> 32) ^ time_now) & 0xffffffff));
        inited = 1;
    }

    return rand();
}

/* uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a
   "name" from a "name space" */
void uuid_create_md5_from_name(void* globals, uuid_t *uuid, uuid_t nsid, void *name,
                               int namelen)
{
    MD5_CTX c;
    unsigned char hash[16];
    uuid_t net_nsid;

    (void) globals; // unused parameter

    /* put name space ID in network byte order so it hashes the same
       no matter what endian machine we're on */
    net_nsid = nsid;
    net_nsid.time_low = htonl(net_nsid.time_low);
    net_nsid.time_mid = htons(net_nsid.time_mid);
    net_nsid.time_hi_and_version = htons(net_nsid.time_hi_and_version);

    MD5Init(&c);
    MD5Update(&c, (unsigned char*)&net_nsid, sizeof net_nsid);
    MD5Update(&c, name, namelen);
    MD5Final(hash, &c);

    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, hash, 3);
}

/* format_uuid_v3or5 -- make a UUID from a (pseudo)random 128-bit
   number */
void format_uuid_v3or5(uuid_t *uuid, unsigned char hash[16], int v)
{
    /* convert UUID to local byte order */
    memcpy(uuid, hash, sizeof *uuid);
    uuid->time_low = ntohl(uuid->time_low);
    uuid->time_mid = ntohs(uuid->time_mid);
    uuid->time_hi_and_version = ntohs(uuid->time_hi_and_version);

    /* put in the variant and version bits */
    uuid->time_hi_and_version &= 0x0FFF;
    uuid->time_hi_and_version |= (v << 12);
    uuid->clock_seq_hi_and_reserved &= 0x3F;
    uuid->clock_seq_hi_and_reserved |= 0x80;
}

/* uuid_compare --  Compare two UUID's "lexically" and return */
#define CHECK(f1, f2) if (f1 != f2) return f1 < f2 ? -1 : 1;
int uuid_compare(uuid_t *u1, uuid_t *u2)
{
    int i;

    CHECK(u1->time_low, u2->time_low);
    CHECK(u1->time_mid, u2->time_mid);
    CHECK(u1->time_hi_and_version, u2->time_hi_and_version);
    CHECK(u1->clock_seq_hi_and_reserved, u2->clock_seq_hi_and_reserved);
    CHECK(u1->clock_seq_low, u2->clock_seq_low)
    for (i = 0; i < 6; i++) {
        if (u1->node[i] < u2->node[i])
            return -1;
        if (u1->node[i] > u2->node[i])
            return 1;
    }
    return 0;
}
#undef CHECK
