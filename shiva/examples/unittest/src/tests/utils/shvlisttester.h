#ifndef __SHIVA_TESTS_UTILS_LISTTESTER_H
#define __SHIVA_TESTS_UTILS_LISTTESTER_H

#include "../shvtestbase.h"

#include "shiva/include/utils/shvlist.h"


class SHVListTester : public SHVTestBase
{
public:
	
	// test modes
	enum Actions {
		ActionAll = 0x01,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestAll(SHVModuleList& modules, SHVTestBase* self, int flag);
};

#endif
