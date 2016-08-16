#ifndef __SHIVA_MODULES_SHIVAFREETDSTESTER_H
#define __SHIVA_MODULES_SHIVAFREETDSTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/freetds/shvfreetdswrapper.h"

//-=========================================================================================================
/// SHVshivafreetdsTester class
/**
 */

class SHVFreeTDSTester : public SHVModule
{
public:

	SHVFreeTDSTester(SHVModuleList& modules, SHVFreeTDSWrapper* wrapper);

	virtual SHVBool Register();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);


private:

	SHVFreeTDSWrapperRef FreeTDS;
	SHVFreeTDSConnectionRef Connection;
};

#endif
