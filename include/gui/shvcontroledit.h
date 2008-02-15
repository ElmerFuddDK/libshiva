#ifndef __SHIVA_GUI_CONTROLEDIT_H
#define __SHIVA_GUI_CONTROLEDIT_H

#include "../include/shvcontrol.h"


// forward declare
class SHVControlImplementerEdit;


//-=========================================================================================================
/// SHVControlEdit - base edit control class
/**
 */

class SHVControlEdit : public SHVControl
{
public:


	// defines and constants
	enum SubTypes {
		SubTypeSingleLine = SHVControl::SubTypeDefault,
		SubTypeMultiLine
	};


	// constructor
	inline SHVControlEdit(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlEdit();


	virtual int GetType();

	inline SHVControlEdit* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVControlData* data);
	virtual SHVBool SetData(SHVControlData* data);

	inline SHVStringBuffer GetText();
	inline SHVControlEdit* SetText(const SHVStringC& title);

	inline int GetLimit();
	inline SHVControlEdit* SetLimit(int limit);

	inline void SetHeight(int lines); // only applicable on multiline boxes


	// obtain pointer to the implementor
	inline SHVControlImplementerEdit* GetImplementor();

};
typedef SHVRefObjectContainer<SHVControlEdit> SHVControlEditRef;


//-=========================================================================================================
/// SHVControlImplementerEdit - base edit control implementerclass
/**
 */

class SHVControlImplementerEdit : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVStringBuffer GetText() = 0;
	virtual void SetText(const SHVStringC& text) = 0;

	virtual int GetLimit() = 0;
	virtual void SetLimit(int limit) = 0;

	virtual void SetHeight(SHVControlEdit* owner, int lines) = 0; // only applicable on multiline boxes

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlEdit::SHVControlEdit(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlEdit* SHVControlEdit::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlEdit::GetText()
{
	return GetImplementor()->GetText();
}

/*************************************
 * SetText
 *************************************/
SHVControlEdit* SHVControlEdit::SetText(const SHVStringC& title)
{
	GetImplementor()->SetText(title);
	return this;
}

/*************************************
 * GetLimit
 *************************************/
int SHVControlEdit::GetLimit()
{
	return GetImplementor()->GetLimit();
}

/*************************************
 * SetLimit
 *************************************/
SHVControlEdit* SHVControlEdit::SetLimit(int limit)
{
	GetImplementor()->SetLimit(limit);
	return this;
}

/*************************************
 * SetHeight
 *************************************/
void SHVControlEdit::SetHeight(int lines)
{
	GetImplementor()->SetHeight(this,lines);
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerEdit* SHVControlEdit::GetImplementor()
{
	return (SHVControlImplementerEdit*)(SHVControlImplementer*)Implementor;
}


#endif
