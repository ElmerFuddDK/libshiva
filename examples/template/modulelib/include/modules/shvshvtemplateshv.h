#ifndef __SHIVA_SHVTEMPLATESHV_SHVTEMPLATESHV_H
#define __SHIVA_SHVTEMPLATESHV_SHVTEMPLATESHV_H

#include "shiva/include/framework/shvmodule.h"

#define __SHVTEMPLATESHV_VERSION_MAJOR   0
#define __SHVTEMPLATESHV_VERSION_MINOR   1
#define __SHVTEMPLATESHV_VERSION_RELEASE 0


//-=========================================================================================================
/// SHVSHVTemplateSHV class
//-=========================================================================================================
/**
 * Documentation for SHVSHVTemplateSHV
 */

class SHVSHVTemplateSHV : public SHVModule
{
public:

	// Add stuff here

protected:
	inline SHVSHVTemplateSHV(SHVModuleList& modules) : SHVModule(modules,"SHVTemplateSHV") {}
	///\endcond
};


#endif
