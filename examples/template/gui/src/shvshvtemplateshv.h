#ifndef __SHIVA_MODULES_SHVTEMPLATESHV_H
#define __SHIVA_MODULES_SHVTEMPLATESHV_H

#include "shiva/include/framework/shvmodule.h"
#include "forms/shvformmain.h"


//-=========================================================================================================
/// SHVSHVTemplateSHV class
/**
 */

class SHVSHVTemplateSHV : public SHVModule
{
public:

	SHVSHVTemplateSHV(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


private:

	SHVGUIManager* GUIManager;
	SHVFormMainRef FormMain;
};

#endif
