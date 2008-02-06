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
SHVBool SHVControl::CreateInternal(SHVControlContainer* newParent)
{
	return GetImplementor()->Create(this, newParent->GetImplementor());
}

///\cond INTERNAL
/*************************************
 * SetParentInternal
 *************************************/
SHVBool SHVControl::SetParentInternal(SHVControlContainer* parent)
{
SHVBool retVal(SHVBool::True);

	if (parent == Parent)
		return retVal;

	if (GetImplementor() && GetImplementor()->IsCreated())
	{
		if (parent)
			retVal = GetImplementor()->Reparent(this, parent->GetImplementor());
		else
			retVal = GetImplementor()->Destroy(this);
	}
	else if (parent)
	{
		retVal = CreateInternal(parent);
	}

	if (retVal)
	{
	SHVControlRef selfRef(this); // ensures the validity of our object during the parenting thingy

		if (Parent)
			Parent->RemoveControl(this);

		if (parent)
			parent->AddControl(this);

		Parent = parent;
	}

	return retVal;
}
///\endcond
