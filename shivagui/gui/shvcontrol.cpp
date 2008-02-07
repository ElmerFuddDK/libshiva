#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/shvcontrol.h"
#include "../include/shvcontrolcontainer.h"
#include "../include/shvguimanager.h"

//=========================================================================================================
// SHVControl - base control interface
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControl::~SHVControl()
{
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
