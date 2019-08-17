#ifndef __SHIVA_MODULES_XMLSTREAMTESTER_H
#define __SHIVA_MODULES_XMLSTREAMTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/modules/xmlstream/shvxmlstream.h"

//-=========================================================================================================
/// SHVXmlStreamTester class
/**
 */

class SHVXmlStreamTester : public SHVModule
{
public:

	SHVXmlStreamTester(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);


private:

	SHVXmlStream* XmlStream;
};

#endif
