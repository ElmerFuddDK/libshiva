#ifndef __SHIVA_MODULES_SHVTEMPLATESHV_H
#define __SHIVA_MODULES_SHVTEMPLATESHV_H

#include "shiva/include/framework/shvmodule.h"

//-=========================================================================================================
/// SHVSHVTemplateSHV class
/**
 */

class SHVSHVTemplateSHV : public SHVModule
{
public:

	SHVSHVTemplateSHV(SHVModuleList& modules);

	virtual SHVBool Register();

};

#endif
