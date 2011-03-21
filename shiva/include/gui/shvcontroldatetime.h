#ifndef __SHIVA_GUI_CONTROLDATETIME_H
#define __SHIVA_GUI_CONTROLDATETIME_H

#include "shvcontrol.h"
#include "../utils/shvtime.h"


// forward declare
class SHVControlImplementerDateTime;


//-=========================================================================================================
/// SHVControlDateTime - base date time control class
/**
 */

class SHVControlDateTime : public SHVControl
{
public:


	// defines and constants
	enum SubTypes {
		SubTypeLongDate = SHVControl::SubTypeDefault,
		SubTypeShortDate,
		SubTypeTime
	};


	// constructor
	inline SHVControlDateTime(SHVGUIManager* manager, SHVControlImplementer* implementor);
	virtual ~SHVControlDateTime();


	virtual int GetType();

	inline SHVControlDateTime* SetParent(SHVControlContainer* parent, int flags = FlagVisible);

	// Data handling
	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	virtual SHVTime GetCombinedTime(SHVControlDateTime* otherCtrl);

	inline SHVTime GetTime();
	inline SHVControlDateTime* SetTime(const SHVTime& time);


	// obtain pointer to the implementor
	inline SHVControlImplementerDateTime* GetImplementor();

};
typedef SHVRefObjectContainer<SHVControlDateTime> SHVControlDateTimeRef;


//-=========================================================================================================
/// SHVControlImplementerDateTime - base date time control implementerclass
/**
 */

class SHVControlImplementerDateTime : public SHVControlImplementer
{
public:

	// Properties
	virtual SHVTime GetTime(SHVControlDateTime* owner) = 0;
	virtual void SetTime(SHVControlDateTime* owner, const SHVTime& time) = 0;

};


// ============================================= implementation ============================================= //


/*************************************
 * Constructor
 *************************************/
SHVControlDateTime::SHVControlDateTime(SHVGUIManager* manager, SHVControlImplementer* implementor)
  : SHVControl(manager,implementor)
{}

/*************************************
 * SetParent
 *************************************/
SHVControlDateTime* SHVControlDateTime::SetParent(SHVControlContainer* parent, int flags)
{
	SHVControl::SetParent(parent,flags);
	return this;
}

/*************************************
 * GetTime
 *************************************/
SHVTime SHVControlDateTime::GetTime()
{
	return GetImplementor()->GetTime(this);
}

/*************************************
 * SetTime
 *************************************/
SHVControlDateTime* SHVControlDateTime::SetTime(const SHVTime& time)
{
	GetImplementor()->SetTime(this,time);
	return this;
}

/*************************************
 * GetImplementor
 *************************************/
SHVControlImplementerDateTime* SHVControlDateTime::GetImplementor()
{
	return (SHVControlImplementerDateTime*)(SHVControlImplementer*)Implementor;
}

#endif
