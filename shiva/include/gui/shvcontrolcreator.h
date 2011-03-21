#ifndef __SHIVA_GUI_CONTROLCREATOR_H
#define __SHIVA_GUI_CONTROLCREATOR_H

#include "shvguimanager.h"


//-=========================================================================================================
/// SHVControlCreator - Pointer to a control creating method
/**
 * Use this template class to register a control creating function on an object as a control factory.
 */

template<class C, class I>
class SHVControlCreator : public SHVControlCreatorBase
{
public:

	// constructor
	SHVControlCreator();

	// creation method
	virtual SHVControl* New(SHVGUIManager* manager);
};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template<class C, class I>
SHVControlCreator<C,I>::SHVControlCreator()
{}

/*************************************
 * New
 *************************************/
template<class C, class I>
SHVControl* SHVControlCreator<C,I>::New(SHVGUIManager* manager)
{
	return new C(manager,new I());
}

#endif
