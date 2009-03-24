#ifndef __SHIVA_FRAMEWORK_EVENTSTDIN_H
#define __SHIVA_FRAMEWORK_EVENTSTDIN_H

#include "../framework/shveventstring.h"

#define __EVENT_GLOBAL_STDIN  "Stdin"



//-=========================================================================================================
/// SHVEventStdin class - String based event for distributing stdin lines from console
/**
 */

class SHVAPI SHVEventStdin : public SHVEventString
{
public:


	SHVEventStdin(SHVEventTarget* caller, SHVStringBuffer8 str);

	static const SHVString8C StdinFromEvent(const SHVEvent* event);


protected:
	///\cond INTERNAL
	SHVString8 StdinStr;
	///\endcond
};
typedef SHVRefObjectContainer<SHVEventStdin> SHVEventStdinRef;





#endif
