#include "../posix/platformspc.h"
#include <TargetConditionals.h>

#if TARGET_OS_EMBEDDED == 1
# define __SHIVA_POSIX_IPHONEOS
#elif TARGET_IPHONE_SIMULATOR == 1
# define __SHIVA_POSIX_IPHONEOS
# define __SHIVA_POSIX_IPHONEOS_SIMULATOR
#endif
