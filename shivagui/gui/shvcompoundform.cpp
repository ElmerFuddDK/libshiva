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
#include "../../include/platformspc.h"


#include "../include/shvcompoundform.h"

//=========================================================================================================
// SHVCompoundForm - base master/child form interface
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVCompoundForm::SHVCompoundForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName):
	SHVForm(manager, controlContainer, entityName)
{
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVCompoundForm::GetData(SHVControlData* data)
{
size_t count = Forms.CalculateCount();
size_t i;
SHVBool retVal(SHVBool::True);

	if (SHVForm::GetData(data))
	{
		for (i=count;i>0 && retVal;)
		{
			if (data->GetRow())
				retVal = Forms[i]->GetData(data->GetRow()->GetSubData(Forms[i]->GetEntityName().ToStrT()));
		}
	}
	return retVal;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVCompoundForm::SetData(SHVControlData* data)
{
size_t count = Forms.CalculateCount();
size_t i;
SHVBool retVal(SHVBool::True);

	if (SHVForm::SetData(data))
	{
		for (i=count;i>0 && retVal;)
		{
			i--;
			if (data->GetRow())
				retVal = Forms[i]->SetData(data->GetRow()->GetSubData(Forms[i]->GetEntityName().ToStrT()));
		}
	}
	else
		retVal == SHVBool::False;
	return retVal;
}

/*************************************
 * PreClose
 *************************************/
SHVBool SHVCompoundForm::PreClose()
{
size_t count = Forms.CalculateCount();
size_t i;
SHVBool retVal(SHVForm::PreClose());
	if (retVal)
	{
		for (i=count;i>0 && retVal;)
		{
			retVal = Forms[i]->PreClose();
		}
	}
	return retVal;
}

