#ifndef __SHIVA_UNITTEST_SRC_TEST_H
#define __SHIVA_UNITTEST_SRC_TEST_H

#include "../shvtestbase.h"
#include "../../framework/shvmodulelist.h"
#include "../../utils/shvvector.h"
#include "../../threadutils/shvmutex.h"
#include "../shvtestlogger.h"


//-=========================================================================================================
/// SHVTest class
/**
 * Inherit from this class to create a unit test.\n
 * You will need to override at least 3 functions : GetID, GetTitle and GetActions.
 */

class SHVTest : public SHVTestBase
{
public:

	virtual const SHVString8C GetGroup() const = 0;
	virtual const SHVString8C GetID() const = 0;
	virtual const SHVStringC GetTitle() const = 0;
	virtual const Action* GetActions() const = 0;


	virtual SHVBool Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result);

	virtual bool IsPerforming();
	virtual void DisplayResult(SHVTestLogger* logger);


	// For use within the tests to add log to the end result
	virtual void AddHeader(const SHVStringC str);
	virtual void AddHeader(const SHVTChar* s, ...);
	virtual void AddLine(const SHVStringC str);
	virtual void AddLine(const SHVTChar* s, ...);
	virtual SHVStringBuffer Success(SHVModuleList& modules, bool ok);


protected:

	SHVTest();

	bool InitializePerform();

	virtual void PerformAction(SHVModuleList& modules, const Action* action);
	void EmitPerformedEvent(SHVModuleList& modules, SHVEventSubscriberBase* result);

	SHVMutex LogLock;

private:
	enum LogTypes { LogTypeTitle, LogTypeHeader, LogTypeLine };
	struct LogString
	{
		LogTypes Type;
		SHVString String;
		inline LogString(LogTypes type, SHVStringBuffer string) : Type(type), String(string) {}
	};
	SHVVector<LogString> Log;
	bool OK;
};

#endif
