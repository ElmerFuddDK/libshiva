#ifndef __SHIVA_GUI_CONTROLLAYOUT_H
#define __SHIVA_GUI_CONTROLLAYOUT_H

#include "shvcontrol.h"


// forward declare
class SHVControlContainer;


//-=========================================================================================================
/// SHVControlLayout - Layout engine
/**
 */

class SHVControlLayout : public SHVRefObject
{
public:

	virtual void ResizeControls(SHVControlContainer* container) = 0;

};
typedef SHVRefObjectContainer<SHVControlLayout> SHVControlLayoutRef;



//-=========================================================================================================
/// SHVControlLayoutCallback - Layout engine
/**
 */

template <class T>
class SHVControlLayoutCallback : public SHVControlLayout
{
public:


	// defines
	typedef void (T::*OnResize)(SHVControlContainer* container, SHVControlLayout* Layout);


	// Constructor
	SHVControlLayoutCallback(T* target, OnResize func);


	// Resize
	virtual void ResizeControls(SHVControlContainer* container);


protected:

	T* Target;
	OnResize Func;
};
typedef SHVRefObjectContainer<SHVControlLayout> SHVControlLayoutRef;



// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template <class T>
SHVControlLayoutCallback<T>::SHVControlLayoutCallback(T* target, OnResize func)
	: Target(target), Func(func)
{
}

/*************************************
 * ResizeControls
 *************************************/
template <class T>
void SHVControlLayoutCallback<T>::ResizeControls(SHVControlContainer* container)
{
	(Target->*Func)(container,this);
}
#endif
