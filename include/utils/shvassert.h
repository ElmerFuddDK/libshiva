#ifndef __SHIVA_UTILS_ASSERT_H
#define __SHIVA_UTILS_ASSERT_H

#ifdef DEBUG
# ifndef SHVBREAKPOINT
#  define SHVBREAKPOINT false
# endif
# define SHVASSERT(x) (void) ((x) || !SHVAssert::ReportError(__FILE__, __LINE__) || SHVBREAKPOINT)
# define SHVVERIFY(x) SHVASSERT(x)
#else
# define SHVASSERT(x)
# define SHVVERIFY(x) x
#endif

class SHVAssert
{
public:

	static bool SHVAPI ReportError(const char* fileName, int lineNo);
	
};

#endif
