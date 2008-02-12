#ifndef __SHIVA_GUI_CONTROLIMPLEMENTER_H
#define __SHIVA_GUI_CONTROLIMPLEMENTER_H

#include "utils/shvrect.h"
#include "utils/shvfont.h"
#include "../../include/utils/shvptrcontainer.h"


// forward declare
class SHVControl;

//-=========================================================================================================
/// SHVControlImplementer
/**
 */

class SHVControlImplementer
{
public:


	// Destructor
	virtual ~SHVControlImplementer() {}


	// Properties
	virtual int GetSubType(SHVControl* owner) = 0;

	virtual SHVBool IsCreated() = 0;
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags) = 0;
	virtual SHVBool Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags) = 0;
	virtual SHVBool Destroy(SHVControl* owner) = 0;

	virtual SHVRect GetRect(SHVControl* owner) = 0;
	virtual void SetRect(SHVControl* owner, const SHVRect& rect) = 0;

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable) = 0;
	virtual bool GetFlag(SHVControl* owner, int flag) = 0;

	virtual SHVFont* GetFont(SHVControl* owner) = 0;
	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font) = 0;

	virtual void* GetNative() = 0;

};
typedef SHVPtrContainer<SHVControlImplementer> SHVControlImplementerPtr;

#endif
