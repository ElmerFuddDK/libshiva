#ifndef __SHIVA_SUBPROCESS_SUBPROCESSSERVER_H
#define __SHIVA_SUBPROCESS_SUBPROCESSSERVER_H

#include "../../framework/shvmodule.h"
#include "shvsubprocess.h"

#define __SUBPROCESS_VERSION_MAJOR   0
#define __SUBPROCESS_VERSION_MINOR   1
#define __SUBPROCESS_VERSION_RELEASE 0


//-=========================================================================================================
/// SHVSubProcessServer module
//-=========================================================================================================
/**
 * Documentation for SHVSubProcessServer
 */

class SHVSubProcessServer : public SHVModule
{
public:

	virtual SHVSubProcess* New() = 0;
	inline static SHVSubProcess* Create(SHVModuleList& modules);

protected:
	inline SHVSubProcessServer(SHVModuleList& modules) : SHVModule(modules,"SubProcessServer") {}
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * Create
 *************************************/
SHVSubProcess* SHVSubProcessServer::Create(SHVModuleList& modules)
{
SHVSubProcessServer* server;
	if (SHVModuleResolver<SHVSubProcessServer>(modules,server,"SubProcessServer"))
	{
		return server->New();
	}
	
	return NULL;
}

#endif
