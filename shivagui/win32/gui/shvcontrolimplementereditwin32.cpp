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
	Lines = 1;
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

		SetFont(owner,owner->GetManager()->GetFont(SHVGUIManager::CfgFontNormal),true);
	}

	return retVal;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerEditWin32::GetText()
{
	return SHVControlImplementerWin32Base::GetText();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerEditWin32::SetText(const SHVStringC& text)
{
	SHVControlImplementerWin32Base::SetText(text);
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

/*************************************
 * SetHeight
 *************************************/
void SHVControlImplementerEditWin32::SetHeight(SHVControlEdit* owner, int lines)
{
	Lines = lines;

	if (IsCreated())
	{
	SHVFontRef font = GetFont(owner);
		SHVControlImplementerWin32Base::SetFont(owner,font,CalculateNewHeight(owner,font));
	}
}

/*************************************
 * CalculateNewHeight
 *************************************/
int SHVControlImplementerEditWin32::CalculateNewHeight(SHVControl* owner, SHVFont* font)
{
	return SHVControlImplementerWin32<SHVControlImplementerEdit>::CalculateNewHeight(owner,font)
		+ font->GetCellHeight()*(Lines-1);
}
