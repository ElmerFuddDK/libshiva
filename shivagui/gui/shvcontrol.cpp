#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontrol.h"
#include "../../include/gui/shvcontrolcontainer.h"
#include "../../include/gui/shvguimanager.h"

//=========================================================================================================
// SHVControl - base control interface
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControl::~SHVControl()
{
	GUIManager->ClearControlEvents(this);
	GetImplementor()->Destroy(this);
	Implementor = NULL;
}

/*************************************
 * SetRect
 *************************************/
void SHVControl::SetRect(const SHVRect& rect)
{
	if (GetImplementor())
		GetImplementor()->SetRect(this, rect);
}

/*************************************
 * CreatePopupMenu
 *************************************/
SHVMenu* SHVControl::CreatePopupMenu(SHVEventSubscriberBase* subscriber)
{
	return GUIManager->CreatePopupMenu(subscriber,this);
}

/*************************************
 * SetFont
 *************************************/
SHVBool SHVControl::SetFont(SHVFont* font, bool resetHeight)
{
	if (font == NULL)
		font = GetManager()->GetFont(SHVGUIManager::CfgFontNormal);

	return GetImplementor()->SetFont(this, font, resetHeight);
}

/*************************************
 * GetModuleList
 *************************************/
SHVModuleList& SHVControl::GetModuleList()
{
	return GUIManager->GetModuleList();
}

/*************************************
 * CreateInternal
 *************************************/
/// Create the internal control - before parent is set
SHVBool SHVControl::CreateInternal(SHVControlContainer* newParent, int flags)
{
	return GetImplementor()->Create(this, newParent->GetImplementor(), flags);
}

///\cond INTERNAL
/*************************************
 * SetParentInternal
 *************************************/
void SHVControl::SetParentInternal(SHVControlContainer* parent, int flags)
{
SHVBool ok(SHVBool::True);

	SHVASSERT(parent != Parent);

	// you may not create or reparent a tab page - this is a "hidden" container class used by a tab control
	SHVASSERT(GetType() != SHVControl::TypeContainer || GetImplementor()->GetSubType(this) != SHVControlContainer::SubTypeTabPage);

	if (parent == Parent)
	{
		SetFlag(flags);
		return;
	}

	if (GetImplementor() && GetImplementor()->IsCreated())
	{
		if (parent)
			ok = GetImplementor()->Reparent(this, parent->GetImplementor(), flags);
		else
			ok = GetImplementor()->Destroy(this);
	}
	else if (parent)
	{
		ok = CreateInternal(parent, flags);
	}

	SHVASSERT(ok);

	if (ok)
	{
	SHVControlRef selfRef(this); // ensures the validity of our object during the parenting thingy

		if (Parent)
			Parent->RemoveControl(this);

		if (parent)
			parent->AddControl(this);

		Parent = parent;
	}
}
///\endcond

/*************************************
 * EmitControlEvent
 *************************************/
void SHVControlImplementer::EmitControlEvent(SHVControl* owner, SHVInt controlEvent)
{
	owner->EmitControlEvent(controlEvent);
}
void SHVControl::EmitControlEvent(SHVInt controlEvent)
{
	if (!ObjectIsDeleting())
		GetManager()->EmitControlEvent(this,controlEvent);
}

