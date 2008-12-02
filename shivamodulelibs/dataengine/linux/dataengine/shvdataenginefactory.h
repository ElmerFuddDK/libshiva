#ifndef __SHIVA_MODULEFACTORY_H
#define __SHIVA_MODULEFACTORY_H

#include "../../../../include/framework/shvmodulefactory.h"


//-=========================================================================================================
/// SHVModuleFactorydataengine - factory for module library dataengine
/**
 */

class SHVModuleFactorydataengine : public SHVModuleFactory
{
public:

	
	// Constructor
	SHVModuleFactorydataengine(SHVModuleList& modules);


	virtual SHVBool CheckVersion(int verMajor,int verMinor,int verRelease);
	virtual SHVInt GetMajor();
	virtual SHVInt GetMinor();
	virtual SHVInt GetRelease();
	virtual SHVInt GetBuild();


	// Get description
	virtual SHVStringBuffer GetName();
	virtual SHVStringBuffer GetDescription();


	// Module instantiation
	virtual SHVBool ResolveModules(const SHVString8C symbol);


	// from SHVEventTarget
protected:
	virtual void OnEvent(SHVEvent* event);

private:
	SHVModuleList& Modules;
};

#endif
