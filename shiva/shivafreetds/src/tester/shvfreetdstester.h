#ifndef __SHIVA_MODULES_SHIVAFREETDSTESTER_H
#define __SHIVA_MODULES_SHIVAFREETDSTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/freetds/shvfreetdswrapper.h"
#include "shiva/include/threadutils/shvthread.h"

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

	void ExecQuery(SHVStringUTF8C sql);
	static void ParseResult(SHVFreeTDSResultset* result);
	static void ParseMessages(SHVFreeTDSResultset* result);
	
	SHVFreeTDSWrapperRef FreeTDS;
	SHVFreeTDSConnectionRef Connection;
	SHVFreeTDSTransactionRef Transaction;
	
	// Debug stuff for testing interruptability of queries - NOT THREAD SAFE
	inline void DoInterrupt() { SHVThreadBase::Sleep(500); if (!Transaction.IsNull()) Transaction->Interrupt(); else Connection->Interrupt(); }
	SHVThread<SHVFreeTDSTester> InterruptThread;
	inline void StartInterrupt() { InterruptThread.Start(this,&SHVFreeTDSTester::DoInterrupt); }
};

#endif
