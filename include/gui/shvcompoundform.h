#ifndef __SHIVA_GUI_SHVFORM_COMPOUND_H
#define __SHIVA_GUI_SHVFORM_COMPOUND_H

#include "shvform.h"
#include "../../include/utils/shvvectorref.h"

//-=========================================================================================================
/// SHVCompoundForm - master form to coordinate 1..n child forms
/**
 * This class is used to bind a master form with multible child forms.
 */

class SHVCompoundForm: public SHVForm
{
public:
	SHVCompoundForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);

	virtual void InitializeForm(SHVControlLayout* layout) = 0;

	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	virtual SHVBool PreClose();

private:
	SHVVectorRef<SHVForm> Forms;
};

#endif