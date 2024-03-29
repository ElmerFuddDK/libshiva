#include "copyrt.h"
#include <stdio.h>
#include "sysdep.h"
#include "globals.h"
#include <string.h>

#ifdef __FreeBSD__
#include "freebsd/sysinfo.c"
#endif


/* system dependent call to get IEEE node ID.
   This sample implementation generates a random node ID. */
void get_ieee_node_identifier(void* globals, uuid_node_t *node)
{
GLOB;
    char seed[16];

    if (!g->get_ieee_node_identifier_inited) {
       get_random_info(seed);
       seed[0] |= 0x01;
       memcpy(&g->get_ieee_node_identifier_saved_node, seed, sizeof g->get_ieee_node_identifier_saved_node);
       g->get_ieee_node_identifier_inited = 1;
    }

    *node = g->get_ieee_node_identifier_saved_node;
}

void set_ieee_node_identifier(void* globals, void* data, size_t sz)
{
GLOB;
	if (sz >= sizeof g->get_ieee_node_identifier_saved_node)
        sz = sizeof g->get_ieee_node_identifier_saved_node;
	else
	{
       char seed[16];
       get_random_info(seed);
       memcpy(&g->get_ieee_node_identifier_saved_node, seed, sizeof g->get_ieee_node_identifier_saved_node);
	}

    memcpy(&g->get_ieee_node_identifier_saved_node, data, sz);

	g->get_ieee_node_identifier_saved_node.nodeID[0] |= 0x01;
    g->get_ieee_node_identifier_inited = 1;
}

/* system dependent call to get the current system time. Returned as
   100ns ticks since UUID epoch, but resolution may be less than
   100ns. */
#if defined(_WIN32_WCE) || defined(__MINGW32__)

void get_system_time(uuid_time_t *uuid_time)
{
	SYSTEMTIME st;
    ULARGE_INTEGER time;

    /* NT keeps time in FILETIME format which is 100ns ticks since
       Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
       The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
       + 18 years and 5 leap days. */
	GetSystemTime(&st);
	SystemTimeToFileTime(&st,(FILETIME *)&time);
    time.QuadPart +=

          (unsigned __int64) (1000*1000*10)       // seconds
        * (unsigned __int64) (60 * 60 * 24)       // days
        * (unsigned __int64) (17+30+31+365*18+5); // # of days
    *uuid_time = time.QuadPart;
}

/* Sample code, not for use in production; see RFC 1750 */
void get_random_info(char seed[16])
{
	SYSTEMTIME st;
    MD5_CTX c;
    struct {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
    } r;

    MD5Init(&c);
    GlobalMemoryStatus(&r.m);
    GetSystemInfo(&r.s);
	GetSystemTime(&st);
	SystemTimeToFileTime(&st,&r.t);
    QueryPerformanceCounter(&r.pc);
    r.tc = GetTickCount();
    MD5Update(&c, (unsigned char*)&r, sizeof r);
    MD5Final((unsigned char*)seed, &c);
}

#elif defined(_WINDOWS_)

void get_system_time(uuid_time_t *uuid_time)
{
    ULARGE_INTEGER time;

    /* NT keeps time in FILETIME format which is 100ns ticks since
       Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
       The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
       + 18 years and 5 leap days. */
    GetSystemTimeAsFileTime((FILETIME *)&time);
    time.QuadPart +=

          (unsigned __int64) (1000*1000*10)       // seconds
        * (unsigned __int64) (60 * 60 * 24)       // days
        * (unsigned __int64) (17+30+31+365*18+5); // # of days
    *uuid_time = time.QuadPart;
}

/* Sample code, not for use in production; see RFC 1750 */
void get_random_info(char seed[16])
{
    MD5_CTX c;
    struct {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
        TCHAR hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } r;

    MD5Init(&c);
    GlobalMemoryStatus(&r.m);
    GetSystemInfo(&r.s);
    GetSystemTimeAsFileTime(&r.t);
    QueryPerformanceCounter(&r.pc);
    r.tc = GetTickCount();
    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.hostname, &r.l);
    MD5Update(&c, (unsigned char*)&r, sizeof r);
    MD5Final(seed, &c);
}

#else

#include <unistd.h>
#ifdef __SHIVA_SYSINFOSYSCALL
# include <asm/unistd.h>
# define sysinfo(x) syscall(__NR_sysinfo,x)
#endif

void get_system_time(uuid_time_t *uuid_time)
{
    struct timeval tp;

    gettimeofday(&tp, (struct timezone *)0);

    /* Offset between UUID formatted times and Unix formatted times.
       UUID UTC base time is October 15, 1582.
       Unix base time is January 1, 1970.*/
    *uuid_time = ((unsigned64_t)tp.tv_sec * 10000000)
        + ((unsigned64_t)tp.tv_usec * 10)
        + I64(0x01B21DD213814000);
}

/* Sample code, not for use in production; see RFC 1750 */
void get_random_info(char seed[16])
{
#ifdef __APPLE__
	///\todo REWRITE!!
	//FIXME
    MD5_CTX c;
    struct {
        //struct sysinfo s;
        struct timeval t;
        char hostname[257];
    } r;

    MD5Init(&c);
    //sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);
	MD5Update(&c, (unsigned char*)&r, sizeof r);
	MD5Final((unsigned char*)seed, &c);
#else	
    MD5_CTX c;
    struct {
        struct sysinfo s;
        struct timeval t;
        char hostname[257];
    } r;

    MD5Init(&c);
    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);
	MD5Update(&c, (unsigned char*)&r, sizeof r);
	MD5Final((unsigned char*)seed, &c);
#endif
}

#endif
