#ifndef __SHIVA_SUBPROCESSIMPL_SUBPROCESSSERVER_H
#define __SHIVA_SUBPROCESSIMPL_SUBPROCESSSERVER_H

#include "../../../../include/modules/subprocess/shvsubprocessserver.h"


//-=========================================================================================================
/// SHVSubProcessImpl class
//-=========================================================================================================
/**
 * Implementation of SHVSubProcess
 */

class SHVSubProcessServerImpl : public SHVSubProcessServer
{
public:


	// constructor
	SHVSubProcessServerImpl(SHVModuleList& modules);
	~SHVSubProcessServerImpl();


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// Create a sub process
	virtual SHVSubProcess* New();


private:
};

#endif
