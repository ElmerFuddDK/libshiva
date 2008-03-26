#ifndef __SHIVA_WIN32GUI_CONTROLCONTAINERWRAPPER_H
#define __SHIVA_WIN32GUI_CONTROLCONTAINERWRAPPER_H

#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvcontrolimplementerwin32.h"


//-=========================================================================================================
/// SHVControlWrapperWin32 - base control interface
/**
 * This class wraps an already created native control
 */

class SHVControlContainerWrapperWin32 : public SHVControlContainer
{
public:


	// Destructor
	SHVControlContainerWrapperWin32(SHVGUIManager* manager, HWND hWnd = NULL);
	virtual ~SHVControlContainerWrapperWin32();

    virtual SHVControlContainerWrapperWin32* SetHandle(HWND hWnd);

	virtual int GetType();

	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	virtual SHVBool Create(int flags);
};
typedef SHVRefObjectContainer<SHVControlContainerWrapperWin32> SHVControlContainerWrapperWin32Ref;



//-=========================================================================================================
/// SHVControlImplementerWrapperWin32
/**
 */

class SHVControlImplementerContainerWrapperWin32: public SHVControlImplementerWin32<SHVControlImplementerContainer>
{
public:

	SHVControlImplementerContainerWrapperWin32();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	// From SHVControlImplementerContainer
	virtual SHVRect GetRegionRect();
	virtual void SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode);

	virtual SHVStringBuffer GetTitle();
	virtual void SetTitle(const SHVStringC& title);

	virtual SHVColor* GetColor(SHVControlContainer* owner);
	virtual void SetColor(SHVControlContainer* owner, SHVColor* color);

	virtual void SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels);
	virtual SHVPoint GetMinimumSizeInPixels(SHVControlContainer* owner);

protected:
	// Will be called when a layout engine is attached/detached
	virtual void SetResizable(bool resizable);
};

#endif
