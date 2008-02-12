#ifndef __SHIVA_FRAMEWORK_MODULEFACTORY_H
#define __SHIVA_FRAMEWORK_MODULEFACTORY_H

#include "shvmodule.h"

#define __MODULESYMBOL_DEFAULTS "Defaults"


//-=========================================================================================================
/// SHVModuleFactory class - interface for instantiating modules from a module library
/**
 * Contains the basic interface for instantiating modules from a module library.
 */

class SHVAPI SHVModuleFactory : public SHVEventTarget
{
public:


	// destructor
	virtual ~SHVModuleFactory() {}

	// Version check
	virtual SHVBool CheckVersion(int verMajor,int verMinor,int verRelease) = 0;
	virtual SHVInt GetMajor() = 0;
	virtual SHVInt GetMinor() = 0;
	virtual SHVInt GetRelease() = 0;
	virtual SHVInt GetBuild() = 0;


	// Get description
	virtual SHVStringBuffer GetName() = 0;
	virtual SHVStringBuffer GetDescription() = 0;


	// Module instantiation
	virtual SHVBool ResolveModules(const SHVString8C symbol) = 0;


	// from SHVEventTarget
protected:
	virtual void OnEvent(SHVEvent* event) = 0;
};
typedef SHVPtrContainer<SHVModuleFactory> SHVModuleFactoryPtr;

#endif
