#ifndef __SHIVA_GUITESTER_CONTROLTESTER_H
#define __SHIVA_GUITESTER_CONTROLTESTER_H

#include "../../../../../include/framework/shvmodule.h"
#include "../../../../include/shvguimanager.h"

//-=========================================================================================================
/// SHVControlTester - Module for testing controls
/**
 * Implements a module for testing the generic controls
 */

class SHVControlTester : public SHVModule
{
public:


	// default constructor
	SHVControlTester(SHVModuleList& modules);


	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

private:

	SHVGUIManager* GUIManager;
	SHVControlRef Label;
	SHVControlRef EditBox;
	SHVControlRef Button;

};

#endif
