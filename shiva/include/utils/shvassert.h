#ifndef __SHIVA_UTILS_ASSERT_H
#define __SHIVA_UTILS_ASSERT_H

#ifdef DEBUG
# ifndef SHVBREAKPOINT
#  define SHVBREAKPOINT false
# endif
# define SHVASSERT(x) (void) ((x) || !SHVAssert::ReportError(__FILE__, __LINE__) || SHVBREAKPOINT)
# define SHVVERIFY(x) SHVASSERT(x)
# define SHVTRACE SHVAssert::Trace
# define SHVDUMPSTACKTRACE(x1,x2,x3) SHVAssert::DumpStackTrace(x1,x2,x3)
# define SHVDUMPSTACKTRACE_EX(x1,x2,x3,x4) SHVAssert::DumpStackTrace(x1,x2,x3,false,x4)
# define SHVADDSTACKTRACE(x1,x2,x3) SHVAssert::DumpStackTrace(x1,x2,x3,true)
# define SHVADDSTACKTRACE_EX(x1,x2,x3,x4) SHVAssert::DumpStackTrace(x1,x2,x3,true,x4)
# define SHVCLEARSTACKTRACE(x1,x2,x3) SHVAssert::ClearStackTrace(x1,x2,x3)
#else
# define SHVASSERT(x)
# define SHVVERIFY(x) if (x) {;}
# define SHVTRACE (void)
# define SHVDUMPSTACKTRACE(x1,x2,x3)
# define SHVDUMPSTACKTRACE_EX(x1,x2,x3,x4)
# define SHVADDSTACKTRACE(x1,x2,x3)
# define SHVADDSTACKTRACE_EX(x1,x2,x3,x4)
# define SHVCLEARSTACKTRACE(x1,x2,x3)
#endif

class SHVAssert
{
public:

	static bool SHVAPI ReportError(const char* fileName, int lineNo);
	static void SHVAPI Trace(const SHVTChar* s, ...);
	
	static void SHVAPI DumpStackTrace(const char* dir, const char* basename, const void* ptr, bool append=false, const char* xtrainfo = NULL);
	static void SHVAPI ClearStackTrace(const char* dir, const char* basename, const void* ptr);
	
};

#endif
