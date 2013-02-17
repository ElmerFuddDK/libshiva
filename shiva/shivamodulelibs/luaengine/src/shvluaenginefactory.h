#ifndef __SHIVA_MODULEFACTORY_LUAENGINE_H
#define __SHIVA_MODULEFACTORY_LUAENGINE_H

#include "shiva/include/framework/shvmodulefactory.h"


//-=========================================================================================================
/// SHVModuleFactoryLuaEngine - factory for module library LuaEngine
/**
 */

class SHVModuleFactoryLuaEngine : public SHVModuleFactory
{
public:

	
	// Constructor
	SHVModuleFactoryLuaEngine(SHVModuleList& modules);


	virtual SHVBool CheckVersion(int verMajor,int verMinor,int verRelease);
	virtual SHVInt GetMajor();
	virtual SHVInt GetMinor();
	virtual SHVInt GetRelease();
	virtual SHVInt GetBuild();
	virtual SHVStringBuffer GetName();


	// Get description
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
