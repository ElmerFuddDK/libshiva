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

#include "../../../include/framework/shveventstring.h"


//=========================================================================================================
// SHVEventString class - String based event
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVEventString::SHVEventString(SHVEventTarget* caller, const SHVChar* name, SHVInt id, SHVInt subid, SHVRefObject* obj)
	: SHVEvent(caller,id,subid,obj), Name(name)
{
}

/*************************************
 * GetType
 *************************************/
SHVEvent::Types SHVEventString::GetType() const
{
	return SHVEvent::TypeString;
}

/*************************************
 * Equals
 *************************************/
/// Compares a string based id event with a name
/**
 \param event Event to compare with
 \param name Name of event
 \return True if event is of type String and has given name
 *
 * Use this function in the following way:
\code
void SHVMyModule::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,"Debug"))
	{
	SHVRefObjectTemplate<SHVString>* str = (SHVRefObjectTemplate<SHVString>*)event->GetObject();

		if (str)
			sprintf("Debug: %s\n", str->Object().GetSafeBuffer());
	}
}
\endcode
 */
bool SHVEventString::Equals(const SHVEvent* event, const char* name)
{
	return ( event->GetType() == SHVEvent::TypeString && !::strcmp(name,((SHVEventString*)event)->GetName().GetSafeBuffer()) );
}

