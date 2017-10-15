#ifndef __SHIVA_GUI_CONTROLBASE_H
#define __SHIVA_GUI_CONTROLBASE_H

#include "../../include/framework/shvmodulelist.h"
#include "../../include/utils/shvrefobject.h"
#include "../../include/utils/shvstring.h"
#include "../../include/databinder/shvdatabinder.h"
#include "shvcontrolimplementer.h"
#include "shvmenu.h"
#include "utils/shvrect.h"
#include "utils/shvfont.h"


// forward declare
class SHVControlContainer;
class SHVGUIManager;

//-=========================================================================================================
/// SHVControl - base control interface
/**
 * This class is the basis of all controls in SHIVA.
 */

class SHVControl : public SHVRefObject
{
public:


	// defines and constants
	enum Types {
		TypeLabel,
		TypeButton,
		TypeEdit,
		TypeList,
		TypeGrid,
		TypeTab,
		TypeDateTime,
		TypeComboBox,
		TypeProgressBar,
		TypeListView,
		TypeCheckbox,
		TypeCustom = 0xFE,
		TypeContainer = 0xFF
	};

	enum SubTypes {
		SubTypeDefault
	};

	enum Flags {
		FlagVisible = 1,
		FlagDisabled = 2,
		FlagSubType = 512 // Sub type controls start from here
	};

	enum Events {
		EventDraw,
		EventMenu,
		EventSubClass = 1000, // Subclass event types start from here
		EventSubType = 2000 // SubType event types start from here
	};


	// Destructor
	virtual ~SHVControl();


	virtual int GetType() = 0;

	inline SHVBool IsCreated();
	inline SHVControl* SetParent(SHVControlContainer* parent, int flags = FlagVisible);
	inline SHVControlContainer* GetParent();

	virtual SHVBool GetData(SHVDataBinder* data) = 0;
	virtual SHVBool SetData(SHVDataBinder* data) = 0;

	virtual SHVMenu* CreatePopupMenu(SHVEventSubscriberBase* subscriber);


	// Properties
	inline SHVRect GetRect();
	virtual void SetRect(const SHVRect& rect);

	inline SHVBool SetFlag(int flag, bool enable = true);
	inline bool GetFlag(int flag);

	inline SHVFont* GetFont();
	virtual SHVBool SetFont(SHVFont* font, bool resetHeight);
	

	// obtain pointer to the implementor
	inline SHVControlImplementer* GetImplementor();

	// Get module list
	SHVModuleList& GetModuleList();
	inline SHVGUIManager* GetManager();


protected:

	// Create the internal control - before parent is set
	virtual SHVBool CreateInternal(SHVControlContainer* newParent, int flags);


	inline SHVControl(SHVGUIManager* manager, SHVControlImplementer* implementor);


	SHVGUIManager* GUIManager;
	SHVControlImplementerPtr Implementor;


private:
friend class SHVControlImplementer;

	///\cond INTERNAL
	virtual void SetParentInternal(SHVControlContainer* parent, int flags);
	virtual void EmitControlEvent(SHVInt controlEvent);

	SHVControlContainer* Parent;
	///\endcond
};
typedef SHVRefObjectContainer<SHVControl> SHVControlRef;



// ============================================ implementation ============================================ //

/*************************************
 * Constructor
 *************************************/
SHVControl::SHVControl(SHVGUIManager* manager, SHVControlImplementer* implementor) : GUIManager(manager), Implementor(implementor)
{
	Parent = NULL;
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVControl::IsCreated()
{
	return ( GetImplementor() ? GetImplementor()->IsCreated() : SHVBool::False );
}

/*************************************
 * SetParent
 *************************************/
SHVControl* SHVControl::SetParent(SHVControlContainer* parent, int flags)
{
	SetParentInternal(parent,flags);
	return this;
}

/*************************************
 * GetParent
 *************************************/
SHVControlContainer* SHVControl::GetParent()
{
	return Parent;
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControl::GetRect()
{
	if (GetImplementor())
		return GetImplementor()->GetRect(this);

	return SHVRect();
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControl::SetFlag(int flag, bool enable)
{
	if (GetImplementor())
		return GetImplementor()->SetFlag(this, flag, enable);

	return SHVBool::False;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControl::GetFlag(int flag)
{
	if (GetImplementor())
		return GetImplementor()->GetFlag(this, flag);

	return false;
}

/*************************************
 * GetFont
 *************************************/
SHVFont* SHVControl::GetFont()
{
	return GetImplementor()->GetFont(this);
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementer* SHVControl::GetImplementor()
{
	return Implementor;
}

/*************************************
 * GetManager
 *************************************/
SHVGUIManager* SHVControl::GetManager()
{
	return GUIManager;
}

#endif
