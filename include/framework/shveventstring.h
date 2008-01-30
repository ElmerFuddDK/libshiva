#ifndef __SHIVA_FRAMEWORK_EVENTSTRING_H
#define __SHIVA_FRAMEWORK_EVENTSTRING_H

#include "shvevent.h"
#include "../utils/shvstring.h"


//-=========================================================================================================
/// SHVEventString class - String based event
/**
 * As base events are used for events sent from an event target, the SHVEventString class is intended for
 * events of more global characteristics.
 */

class SHVAPI SHVEventString : public SHVEvent
{
public:


	SHVEventString(SHVEventTarget* caller, const SHVChar* name, SHVInt id = SHVInt(), SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);


	virtual Types GetType() const;

	// properties
	inline const SHVString8C& GetName() const;


	// comparison
	static bool Equals(const SHVEvent* event, const char* name);


protected:
	///\cond INTERNAL
	SHVString8C Name;
	///\endcond
};
typedef SHVRefObjectContainer<SHVEvent> SHVEventRef;



// ============================================= implementation ============================================= //

/*************************************
 * GetName
 *************************************/
/// Returns name identifying the event
const SHVString8C& SHVEventString::GetName() const
{
	return Name;
}

#endif
