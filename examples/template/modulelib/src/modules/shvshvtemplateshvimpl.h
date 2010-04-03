#ifndef __SHIVA_SHVTEMPLATESHVIMPL_SHVTEMPLATESHV_H
#define __SHIVA_SHVTEMPLATESHVIMPL_SHVTEMPLATESHV_H

#include "../../include/modules/shvshvtemplateshv.h"


//-=========================================================================================================
/// SHVSHVTemplateSHVImpl class
//-=========================================================================================================
/**
 * Implementation of SHVSHVTemplateSHV
 */

class SHVSHVTemplateSHVImpl : public SHVSHVTemplateSHV
{
public:


	// constructor
	SHVSHVTemplateSHVImpl(SHVModuleList& modules);
	~SHVSHVTemplateSHVImpl();


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// Insert inherited methods here

private:

	// Insert member variables here

};

#endif
