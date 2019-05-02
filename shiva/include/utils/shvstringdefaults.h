#ifndef __SHIVA_UTILS_STRINGDEFAULTS_H
#define __SHIVA_UTILS_STRINGDEFAULTS_H

#include "shvrefobject.h"

// Typedefs and forward declares
class SHVString8C;
class SHVString8;
class SHVString8CRef;
class SHVStringBuffer8;
class SHVStringBufferUTF8;
class SHVStringBuffer16;


#ifndef __SHVSTRING_EXCLUDE_UNICODE
class SHVString16C;
class SHVString16;
class SHVString16CRef;
class SHVStringBuffer8;
class SHVStringBufferUTF8;
class SHVStringBuffer16;

class SHVStringUTF8C;
class SHVString8C;
class SHVStringUTF8;
class SHVStringUTF8CRef;
class SHVStringBufferUTF8;
class SHVStringBuffer8;
class SHVStringBuffer16;
#endif

#if __SHVSTRINGDEFAULT == 8
typedef SHVString8C  SHVStringC;
typedef SHVString8   SHVString;
typedef SHVString8CRef SHVStringCRef;
typedef SHVStringBuffer8 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(x)
# ifndef __SHVSTRING_EXCLUDE_UNICODE
#  define _SHVS16(x) SHVStringC(x).ToStr16()
#  ifdef UNICODE
#   define _SHVTTOS(x) SHVString16C((const SHVWChar*)x).ToStr8()
#   define _SHVTTOSD(x) SHVString16C(_S(x)).ToStr8()
#   define _SHVSTOT(x) SHVStringC(x).ToStr16()
#  else
#   define _SHVTTOS(x) SHVString8C(x)
#   define _SHVTTOSD(x) SHVString8C(x)
#   define _SHVSTOT(x) SHVStringC(x)
#  endif
# endif
///\cond INTERNAL
# define _S(x)  x
# define _SD(x) x
# define _SC(x) x
# define _SCD(x) x
///\endcond
#elif __SHVSTRINGDEFAULT == 16
typedef SHVString16C SHVStringC;
typedef SHVString16  SHVString;
typedef SHVString16CRef SHVStringCRef;
typedef SHVStringBuffer16 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(_S(x)).ToStr8()
# define _SHVS16(x) SHVString16C(_S(x))
# ifdef UNICODE
#  define _SHVTTOS(x) SHVString16C((const SHVWChar*)x)
#  define _SHVTTOSD(x) SHVString16C(_S(x))
#  define _SHVSTOT(x) SHVStringC(x)
# else
#  define _SHVTTOS(x) SHVString8C(x).ToStr16()
#  define _SHVTTOSD(x) SHVString8C(x).ToStr16()
#  define _SHVSTOT(x) SHVStringC(x).ToStr8()
# endif
///\cond INTERNAL
# ifdef __SHIVA_POSIX_LINUX
#  error "Linux is currently UCS4 only - doesn't work with shiva in unicode"
# endif
# define _S(x)  (const SHVWChar*)L##x
# define _SD(x) _S(x)
# define _SC(x) L##x
# define _SCD(x) _SC(x)
///\endcond
#elif __SHVSTRINGDEFAULT == utf8
typedef SHVStringUTF8C  SHVStringC;
typedef SHVStringUTF8   SHVString;
typedef SHVStringUTF8CRef SHVStringCRef;
typedef SHVStringBufferUTF8 SHVStringBuffer;
# define _SHVS8(x)  SHVStringC(x).ToStr8()
# define _SHVS16(x) SHVStringC(x).ToStr16()
# ifdef UNICODE
#  define _SHVTTOS(x) SHVString16C((const SHVWChar*)x).ToStrUTF8()
#  define _SHVTTOSD(x) SHVString16C(_S(x)).ToStrUTF8()
#  define _SHVSTOT(x) SHVStringC(x).ToStr16()
# else
#  define _SHVTTOS(x) SHVString8C(x).ToStrUTF8()
#  define _SHVTTOSD(x) SHVString8C(x).ToStrUTF8()
#  define _SHVSTOT(x) SHVStringC(x).ToStr8()
# endif
///\cond INTERNAL
# define _S(x)  x
# define _SD(x) x
# define _SC(x) x
# define _SCD(x) x
///\endcond
#endif

#endif
