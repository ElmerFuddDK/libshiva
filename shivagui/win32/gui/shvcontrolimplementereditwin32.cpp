#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvcontrolimplementereditwin32.h"
#include "shvwin32.h"
#include "utils/shvfontwin32.h"


//=========================================================================================================
// SHVControlImplementerEditWin32 - Edit box implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerEditWin32::SHVControlImplementerEditWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerEdit>(), SubType(subType)
{
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerEditWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerEditWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
SHVBool retVal(parent && owner && !IsCreated());

	SHVASSERT(owner && owner->GetImplementor() == this);

	if (retVal)
	{
	int style = WS_BORDER|ES_LEFT|WS_TABSTOP|WS_CHILD|Win32::MapFlags(flags);

		switch (GetSubType(owner))
		{
		case SHVControlEdit::SubTypeMultiLine:
			style |= ES_MULTILINE|WS_VSCROLL|ES_WANTRETURN|ES_AUTOVSCROLL;
			break;
		case SHVControlEdit::SubTypeSingleLine:
		default:
			style |= ES_AUTOHSCROLL;
			break;
		}
		
		SetHandle(::CreateWindowEx(WS_EX_CLIENTEDGE,_T("EDIT"), _T(""), style,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		retVal = IsCreated();

		SetFont(owner,owner->GetManager()->GetFont(SHVGUIManager::CfgFontNormal));
	}

	return retVal;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerEditWin32::GetText()
{
SHVString retVal;

	SHVASSERT(IsCreated());

	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),(TCHAR*)retVal.GetBuffer(), (int)retVal.GetBufferLen());

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerEditWin32::SetText(const SHVStringC& text)
{
	SHVASSERT(IsCreated());

	SetWindowText(GetHandle(),text.GetSafeBuffer());
}

/*************************************
 * GetLimit
 *************************************/
int SHVControlImplementerEditWin32::GetLimit()
{
	SHVASSERT(IsCreated());

	return (int)::SendMessage(GetHandle(),EM_GETLIMITTEXT,0,0);
}

/*************************************
 * SetLimit
 *************************************/
void SHVControlImplementerEditWin32::SetLimit(int limit)
{
	SHVASSERT(IsCreated());

	::SendMessage(GetHandle(),EM_SETLIMITTEXT,(WPARAM)limit,0);
}
