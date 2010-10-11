#ifndef __SHIVA_MODULES_JSONSTREAMTESTER_H
#define __SHIVA_MODULES_JSONSTREAMTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/modules/jsonstream/shvjsonstream.h"

//-=========================================================================================================
/// SHVSubProcessTester class
/**
 */

class SHVJsonStreamTester : public SHVModule
{
public:

	SHVJsonStreamTester(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);


private:

};

#endif
