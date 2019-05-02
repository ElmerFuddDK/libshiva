/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/framework/shveventdata.h"

#include "shvcontrolimplementerprogressbarwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"

#ifndef PBM_SETRANGE32
# define PBM_SETRANGE32 (WM_USER+6)
#endif
#ifndef PBM_SETPOS
# define PBM_SETPOS (WM_USER+2)
#endif


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerProgressBarWin32::SHVControlImplementerProgressBarWin32() : SHVControlImplementerWin32<SHVControlImplementerProgressBar>()
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerProgressBarWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		
		SetHandle(CreateWindowW(L"msctls_progress32", L"", WS_CHILD|Win32::MapFlags(flags),
			0, 0, 0, GetSystemMetrics(SM_CYVSCROLL), Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));
	
		if (IsCreated())
		{
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);

			owner->SetFont(NULL,true);
		}

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerProgressBarWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * SetRange
 *************************************/
void SHVControlImplementerProgressBarWin32::SetRange(SHVControlProgressBar* owner, int start, int end)
{
	SHVUNUSED_PARAM(owner);

	SHVASSERT(start < end);
	if ( start >= end || (RangeStart == SHVInt(start) && RangeEnd == SHVInt(end)) )
		return;

	RangeStart = start;
	RangeEnd = end;

	if (Progress.IsNull() || Progress < start)
		Progress = start;
	else if (Progress > end)
		Progress = end;

	::SendMessage(GetHandle(), PBM_SETRANGE32, RangeStart, RangeEnd);
	::SendMessage(GetHandle(), PBM_SETPOS, Progress, 0);
}

/*************************************
 * GetRangeStart
 *************************************/
int SHVControlImplementerProgressBarWin32::GetRangeStart()
{
	return RangeStart;
}

/*************************************
 * GetRangeEnd
 *************************************/
int SHVControlImplementerProgressBarWin32::GetRangeEnd()
{
	return RangeEnd;
}

/*************************************
 * SetProgress
 *************************************/
void SHVControlImplementerProgressBarWin32::SetProgress(SHVControlProgressBar* owner, int progress)
{
	SHVUNUSED_PARAM(owner);
	if (!RangeStart.IsNull() && SHVInt(progress) != Progress)
	{
		Progress = progress;
		::SendMessage(GetHandle(), PBM_SETRANGE32, RangeStart, RangeEnd);
		::SendMessage(GetHandle(), PBM_SETPOS, Progress, 0);
	}
}

/*************************************
 * IncrementProgress
 *************************************/
void SHVControlImplementerProgressBarWin32::IncrementProgress(SHVControlProgressBar* owner, int by)
{
	SHVUNUSED_PARAM(owner);

	if (!RangeStart.IsNull())
	{
	SHVInt newProgress = Progress + by;
		if (newProgress < RangeStart)
			newProgress = RangeStart;
		if (newProgress > RangeEnd)
			newProgress = RangeEnd;

		if (newProgress != Progress)
		{
			Progress = newProgress;
			::SendMessage(GetHandle(), PBM_SETRANGE32, RangeStart, RangeEnd);
			::SendMessage(GetHandle(), PBM_SETPOS, Progress, 0);
		}
	}
}

/*************************************
 * GetProgress
 *************************************/
int SHVControlImplementerProgressBarWin32::GetProgress()
{
	return Progress;
}
