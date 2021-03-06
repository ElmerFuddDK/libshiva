#ifndef __SHIVA_TESTS_UTILS_TIMETESTER_H
#define __SHIVA_TESTS_UTILS_TIMETESTER_H

#include "shiva/include/unittest/src/shvtest.h"



class SHVTimeTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionUTC = 0x01,
		ActionConversion = 0x02,
		ActionLocalTime = 0x04,
		ActionRelativeTime = 0x08,
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
	static bool TestRelativeTime(SHVModuleList& modules, SHVTestBase* self, int flag);

};

#endif
