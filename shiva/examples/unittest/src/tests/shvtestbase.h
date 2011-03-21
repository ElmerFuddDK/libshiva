#ifndef __SHIVA_TESTS_TESTBASE_H
#define __SHIVA_TESTS_TESTBASE_H

#include "shiva/include/framework/shvmodulelist.h"
#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvrefobject.h"
#include "shiva/include/utils/shvvector.h"
#include "shiva/include/threadutils/shvmutex.h"
#include "../logger/shvtestlogger.h"


//-=========================================================================================================
/// SHVTestBase class
/**
 * Inherit from this class to create a unit test.\n
 * You will need to override at least 3 functions : GetID, GetTitle and GetActions.
 */

class SHVTestBase : public SHVRefObject
{
public:

	typedef bool (*PerformFunc)(SHVModuleList& modules, SHVTestBase* self, int flag);
	
	enum { FlagAll = 0x7FFFFFFF };

	struct Action
	{
		int Flag;
		const SHVChar* ActionID;
		const SHVTChar* Name;
		const SHVTChar* Description;
		PerformFunc Func;
	};


	virtual const SHVString8C GetGroup() const = 0;
	virtual const SHVString8C GetID() const = 0;
	virtual const SHVStringC GetTitle() const = 0;
	virtual const Action* GetActions() const = 0;


	virtual SHVBool Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result);

	virtual bool IsPerforming();
	virtual void DisplayResult(SHVTestLogger* logger);


	// For use within the tests to add log to the end result
	void AddHeader(const SHVStringC str);
	void AddHeader(const SHVTChar* s, ...);
	void AddLine(const SHVStringC str);
	void AddLine(const SHVTChar* s, ...);
	SHVStringBuffer Success(SHVModuleList& modules, bool ok);


	inline static SHVTestBase* FromTestEvent(SHVEvent* event);


protected:

	SHVTestBase();

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


// ============================================= implementation ============================================= //

SHVTestBase* SHVTestBase::FromTestEvent(SHVEvent* event)
{
	return (SHVTestBase*)event->GetObject();
}

#endif
