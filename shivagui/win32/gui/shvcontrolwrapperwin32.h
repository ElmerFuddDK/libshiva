#ifndef __SHIVA_WIN32GUI_CONTROLWRAPPER_H
#define __SHIVA_WIN32GUI_CONTROLWRAPPER_H

#include "../../../include/gui/shvcontrol.h"
#include "shvcontrolimplementerwin32.h"


//-=========================================================================================================
/// SHVControlWrapperWin32 - base control interface
/**
 * This class wraps an already created native control
 */

class SHVControlWrapperWin32 : public SHVControl
{
public:


	// Destructor
	SHVControlWrapperWin32(SHVGUIManager* manager, HWND hWnd = NULL);
	virtual ~SHVControlWrapperWin32();

    virtual SHVControlWrapperWin32* SetHandle(HWND hWnd);

	virtual int GetType();

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

};
typedef SHVRefObjectContainer<SHVControlWrapperWin32> SHVControlWrapperWin32Ref;



//-=========================================================================================================
/// SHVControlImplementerWrapperWin32
/**
 */

class SHVControlImplementerWrapperWin32: public SHVControlImplementerWin32<SHVControlImplementer>
{
public:

	SHVControlImplementerWrapperWin32();

	// From SHVControlImplementer
	SHVRect GetRect(SHVControl* owner);

	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

};

#endif
