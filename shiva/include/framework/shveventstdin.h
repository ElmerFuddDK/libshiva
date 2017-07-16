#ifndef __SHIVA_FRAMEWORK_EVENTSTDIN_H
#define __SHIVA_FRAMEWORK_EVENTSTDIN_H

#include "../framework/shveventstring.h"
#include "../framework/shvconsole.h"

#define __EVENT_GLOBAL_STDIN  "Stdin"



//-=========================================================================================================
/// SHVEventStdin class - String based event for distributing stdin lines from console
/**
 */

class SHVAPI SHVEventStdin : public SHVEventString
{
public:


	SHVEventStdin(SHVEventTarget* caller, SHVStringBufferUTF8 str);

	static SHVStringBuffer StdinFromEvent(const SHVEvent* event);
	static SHVStringBuffer8 StdinFromEvent8(const SHVEvent* event);
	static const SHVStringUTF8C StdinFromEventUTF8(const SHVEvent* event);
	static SHVStringBuffer16 StdinFromEvent16(const SHVEvent* event);


protected:
	///\cond INTERNAL
	SHVStringUTF8 StdinStr;
	///\endcond
};
typedef SHVRefObjectContainer<SHVEventStdin> SHVEventStdinRef;





#endif
