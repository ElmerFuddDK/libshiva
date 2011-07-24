#ifndef __SHIVA_TESTS_UTILS_INTTESTER_H
#define __SHIVA_TESTS_UTILS_INTTESTER_H

#include "shiva/include/unittest/src/shvtest.h"



class SHVIntTester : public SHVTest
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

	// helpers
private:
	static int ReturnValInt(int i);
	static SHVInt ReturnValSHVInt(int i);
};
#endif
