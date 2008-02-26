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

