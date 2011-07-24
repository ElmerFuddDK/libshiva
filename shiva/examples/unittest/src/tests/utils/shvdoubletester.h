#ifndef __SHIVA_TESTS_UTILS_DOUBLETESTER_H
#define __SHIVA_TESTS_UTILS_DOUBLETESTER_H

#include "shiva/include/unittest/src/shvtest.h"


class SHVDoubleTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionNormal = 0x01
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestNormal(SHVModuleList& modules, SHVTestBase* self, int);

	// helpers
private:
	static double ReturnValDouble(double i);
	static SHVDouble ReturnValSHVDouble(double i);
};

#endif
