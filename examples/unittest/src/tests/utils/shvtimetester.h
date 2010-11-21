#ifndef __SHIVA_TESTS_UTILS_TIMETESTER_H
#define __SHIVA_TESTS_UTILS_TIMETESTER_H

#include "../shvtestbase.h"



class SHVTimeTester : public SHVTestBase
{
public:

	// test modes
	enum Actions {
		ActionUTC = 0x01,
		ActionConversion = 0x02,
		ActionLocalTime = 0x04,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestUTC(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestConversion(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestLocalTime(SHVModuleList& modules, SHVTestBase* self, int flag);

};

#endif
