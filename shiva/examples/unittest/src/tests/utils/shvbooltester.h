#ifndef __SHIVA_TESTS_UTILS_BOOLTESTER_H
#define __SHIVA_TESTS_UTILS_BOOLTESTER_H

#include "shiva/include/unittest/src/shvtest.h"


class SHVBoolTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionReturnVal = 0x01,
		ActionAssignment = 0x02
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestReturnVal(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestAssignment(SHVModuleList& modules, SHVTestBase* self, int flag);

	// helpers
private:
	static SHVBool ReturnValInt(int err);
	static SHVBool ReturnValBool(bool b);
};

#endif
