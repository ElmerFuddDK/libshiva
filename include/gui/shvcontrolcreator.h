#ifndef __SHIVA_GUI_CONTROLCREATOR_H
#define __SHIVA_GUI_CONTROLCREATOR_H

#include "shvcontrolfactory.h"


//-=========================================================================================================
/// SHVControlCreator - Pointer to a control creating method
/**
 * Use this template class to register a control creating function on an object as a control factory.
 */

template<class T>
class SHVControlCreator : public SHVControlCreatorBase
{
public:

	// constructor
	SHVControlCreator();

	// creation method
	virtual SHVControlBase* New(SHVGUIManager* manager);
};



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template<class T>
SHVControlCreator<T>::SHVControlCreator()
{}

/*************************************
 * New
 *************************************/
template<class T>
SHVControlBase* SHVControlCreator<T>::New(SHVGUIManager* manager)
{
	return new T(manager);
}

#endif
