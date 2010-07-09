#ifndef __SHIVA_UTILS_DLL_H
#define __SHIVA_UTILS_DLL_H

#include "shvdllbase.h"


#define SHIVALIB_CREATEOBJECTINTSYMBOL _S("CreateObjectInt")
#define SHIVALIB_CREATEOBJECTSTRINGSYMBOL _S("CreateObjectInt")
#define SHIVALIB_CREATEOBJECTINTOCTAL 1
#define SHIVALIB_CREATEOBJECTSTRINGOCTAL 2

// forward declare
class SHVModuleList;

//-=========================================================================================================
/// SHVDll class - Load and use SHIVA libraries
/**
 * Use this class to load and use SHIVA libraries. If you want to load
 * platform specific libaries use SHVDllBase.\n
 * A SHIVA library is a C++ class factory, and can be used to create C++
 * classes of a predefined interface type. The type can either be supplied
 * as an enum or a string.\n
 * This class is NOT to be considered multi threaded, as some platforms
 * only make loaded libraries available to the calling thread. The resolved
 * symbols are however ok to use by multible threads, but the methods on this
 * class are for one thread only.
 */

class SHVAPI SHVDll : SHVDllBase
{
public:


	enum ClassTypes {
		ClassTypeMainThreadDispatcher,
		ClassTypeModuleFactory,
		ClassTypeUser = 100
	};


	// constructor
	SHVDll();
	~SHVDll();


	// create platform specific library file name
	SHVStringBuffer CreateLibFileName(const SHVStringC libName, const SHVStringC defaultPath = NULL);


	// Load/Unload
	SHVBool IsLoaded();
	SHVBool Load(const SHVStringC libFile);
	void Unload();


	// Create class functions
	void* CreateObjectInt(SHVModuleList* list, int classType);
	void* CreateObjectString(SHVModuleList* list, const SHVStringC classType);


	// Rather hackish method to obtain access to the private inherited stuff
	inline SHVDllBase& Base();


private:
	///\cond INTERNAL
	typedef void* (*SHVCreateObjectIntFunc)(SHVModuleList* list, int classType);
	typedef void* (*SHVCreateObjectStringFunc)(SHVModuleList* list, const SHVTChar* classType);

	SHVCreateObjectIntFunc CreateObjectIntPtr;
	SHVCreateObjectStringFunc CreateObjectStringPtr;
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * Base
 *************************************/
SHVDllBase& SHVDll::Base()
{
	return *(SHVDllBase*)this;
}

#endif
