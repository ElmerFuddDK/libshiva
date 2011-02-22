#ifndef __CONFIG_H
#define __CONFIG_H

#define PACKAGE_VERSION 	"3.7.5" 
#define PACKAGE_STRING  	"sqlite 3.7.5" 
#define PACKAGE_BUGREPORT  	"http://www.sqlite.org"
#define PACKAGE 		"sqlite"  
#define VERSION 		"3.7.5" 
#define STDC_HEADERS 		1
#define HAVE_SYS_TYPES_H 	1 
#define HAVE_SYS_STAT_H 	1 
#define HAVE_STDLIB_H 		1 
#define HAVE_STRING_H 		1 
#define HAVE_MEMORY_H 		1 
#define HAVE_STRINGS_H 		1 
#ifndef _WIN32
# define HAVE_STDINT_H 		1 
# define HAVE_INTTYPES_H 	1 
# define HAVE_LOCALTIME_R 	1 
#endif
#define HAVE_UNISTD_H 		1 
#define HAVE_DLFCN_H 		1 
#define HAVE_FDATASYNC 		1 
#define HAVE_USLEEP 		1 
#define HAVE_GMTIME_R 		1

#ifdef ANDROID
 #define fdatasync fsync
#endif

#endif