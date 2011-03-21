#ifndef __SHIVA_MODULEFACTORY_H
#define __SHIVA_MODULEFACTORY_H

#include "../../../include/framework/shvmodulefactory.h"


//-=========================================================================================================
/// SHVModuleFactorySocketServer - factory for module library SocketServer
/**
 */

class SHVModuleFactorySocketServer : public SHVModuleFactory
{
public:

	
	// Constructor
	SHVModuleFactorySocketServer(SHVModuleList& modules);


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
