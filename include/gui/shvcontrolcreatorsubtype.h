#ifndef __SHIVA_GUI_CONTROLCREATORSUBTYPE_H
#define __SHIVA_GUI_CONTROLCREATORSUBTYPE_H

#include "shvguimanager.h"


//-=========================================================================================================
/// SHVControlCreator - Pointer to a control creating method
/**
 * Use this template class to register a control creating function on an object as a control factory.
 */

template<class C, class I, int SubType>
class SHVControlCreatorSubType : public SHVControlCreatorBase
{
public:

	// constructor
	SHVControlCreatorSubType();

	// creation method
	virtual SHVControl* New(SHVGUIManager* manager);
};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template<class C, class I, int SubType>
SHVControlCreatorSubType<C,I,SubType>::SHVControlCreatorSubType()
{}

/*************************************
 * New
 *************************************/
template<class C, class I, int SubType>
SHVControl* SHVControlCreatorSubType<C,I,SubType>::New(SHVGUIManager* manager)
{
	return new C(manager,new I(SubType));
}

#endif
