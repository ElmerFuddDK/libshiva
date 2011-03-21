#ifndef __SHIVA_MODULES_SHVTEMPLATESHVTESTER_H
#define __SHIVA_MODULES_SHVTEMPLATESHVTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "../../include/modules/shvshvtemplateshv.h"

//-=========================================================================================================
/// SHVSHVTemplateSHVTester class
/**
 */

class SHVSHVTemplateSHVTester : public SHVModule
{
public:

	SHVSHVTemplateSHVTester(SHVModuleList& modules);

	virtual SHVBool Register();

	virtual void OnEvent(SHVEvent* event);


private:

	SHVSHVTemplateSHV* SHVTemplateSHV;
};

#endif
