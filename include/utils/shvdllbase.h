#ifndef __SHIVA_UTILS_DLLBASE_H
#define __SHIVA_UTILS_DLLBASE_H

#include "shvstring.h"


#ifdef __SHIVA_EPOC
# define __SHVDLLBASE_OCTALMODE
#else
# define __SHVDLLBASE_TEXTMODE
#endif


//-=========================================================================================================
/// SHVDllBase class - Base class for loading dlls
/**
 * This class implements platform specific methods for resolving functions
 * in a dynamic library.\n
 * Do not use this class directly as libraries are handled differently on
 * various platforms. Instead implement your dll as a SHIVA library, and use
 * the SHVDll class instead.\n
 * In case you want to load some platform native libraries dynamically you
 * can use this class.
 * This class is NOT to be considered multi threaded, as some platforms
 * only make loaded libraries available to the calling thread. The resolved
 * symbols are however ok to use by multible threads, but the methods on this
 * class are for one thread only.
 */

class SHVAPI SHVDllBase
{
public:


	// defines/constants
	static const SHVStringC FileExtension();


	// constructor
	SHVDllBase();
	~SHVDllBase();


	// create platform specific library file name
	SHVStringBuffer CreateLibFileName(const SHVStringC libName, const SHVStringC defaultPath = NULL);


	// Load/Unload
	SHVBool IsLoaded();
	SHVBool Load(const SHVStringC libFile);
	void Unload();


	// Resolve symbol by text or number depending on platform
#if defined(__SHVDLLBASE_TEXTMODE)
	SHVBool Resolve(void** symbol, const SHVStringC name);
#elif defined(__SHVDLLBASE_OCTALMODE)
	SHVBool Resolve(void** symbol, long id);
#endif

#ifdef __SHIVA_WIN32
	inline HINSTANCE GetNativeHandle();
#elif defined __SHIVA_LINUX
	inline void* GetNativeHandle();
#elif defined __SHIVA_EPOC
	inline RLibrary* GetNativeHandle();
#endif


private:
	///\cond INTERNAL
#ifdef __SHIVA_WIN32
	HINSTANCE hDll;
#elif defined __SHIVA_LINUX
	void* hDll;
#elif defined __SHIVA_EPOC
	RLibrary* hDll;
#endif
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * GetNativeHandle
 *************************************/
#ifdef __SHIVA_WIN32
HINSTANCE SHVDllBase::GetNativeHandle()
{
	return hDll;
}
#elif defined __SHIVA_LINUX
void* SHVDllBase::GetNativeHandle()
{
	return hDll;
}
#elif defined __SHIVA_EPOC
RLibrary* SHVDllBase::GetNativeHandle()
{
	return hDll;
}
#endif

#endif
