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
#include "../../../include/utils/shvstringutf8.h"

#include "shvcontrolimplementerprogressbargtk.h"
#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerProgressBarGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerProgressBarGtk::SHVControlImplementerProgressBarGtk() : SHVControlImplementerGtkWidget<SHVControlImplementerProgressBar>()
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerProgressBarGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_progress_bar_new());
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerProgressBarGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControl::SubTypeDefault;
}

/*************************************
 * SetRange
 *************************************/
void SHVControlImplementerProgressBarGtk::SetRange(SHVControlProgressBar* owner, int start, int end)
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

	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (GetHandle()), double(Progress.IfNull(RangeStart))/double(RangeEnd-RangeStart));
}

/*************************************
 * GetRangeStart
 *************************************/
int SHVControlImplementerProgressBarGtk::GetRangeStart()
{
	return RangeStart;
}

/*************************************
 * GetRangeEnd
 *************************************/
int SHVControlImplementerProgressBarGtk::GetRangeEnd()
{
	return RangeEnd;
}

/*************************************
 * SetProgress
 *************************************/
void SHVControlImplementerProgressBarGtk::SetProgress(SHVControlProgressBar* owner, int progress)
{
	SHVUNUSED_PARAM(owner);
	if (!RangeStart.IsNull() && SHVInt(progress) != Progress)
	{
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (GetHandle()), double(Progress.IfNull(RangeStart))/double(RangeEnd-RangeStart));
	}
}

/*************************************
 * IncrementProgress
 *************************************/
void SHVControlImplementerProgressBarGtk::IncrementProgress(SHVControlProgressBar* owner, int by)
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
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (GetHandle()), double(Progress.IfNull(RangeStart))/double(RangeEnd-RangeStart));
		}
	}
}

/*************************************
 * GetProgress
 *************************************/
int SHVControlImplementerProgressBarGtk::GetProgress()
{
	return Progress;
}
