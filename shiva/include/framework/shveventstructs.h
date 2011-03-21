#ifndef __SHIVA_FRAMEWORK_EVENTSTRUCTS_H
#define __SHIVA_FRAMEWORK_EVENTSTRUCTS_H

#include "shveventdata.h"

//-=========================================================================================================
/// SHVEventDataBool - Event data class to handle bool events
/**
 * Used for confirmation purposes.
 */

class SHVEventDataBool
{
public:
	inline SHVEventDataBool(SHVBool InitValue);
	static inline void SetValue(SHVEvent* event, SHVBool value);
	inline SHVBool GetValue();

private:
	bool Set;
	SHVBool Value;
};


// ============================================= implementation ============================================= //
/// \class SHVEventDataBool shveventstructs.h "shiva/include/framework/shveventstructs.h"

/*************************************
 * Constructor
 *************************************/
SHVEventDataBool::SHVEventDataBool(SHVBool InitValue) : Value(InitValue) 
{ 
	Set=false; 
}

/*************************************
 * SetValue
 *************************************/
void SHVEventDataBool::SetValue(SHVEvent* event, SHVBool value)
{
SHVEventDataBool* self = (SHVEventDataBool*)event->GetData();

	if (!self->Set && self->Value != value)
	{
		self->Value = value;
		self->Set = true;
	}
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVEventDataBool::GetValue()
{
	return Value;
}

#endif
