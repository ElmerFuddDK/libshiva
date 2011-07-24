#ifndef __SHIVA_TESTS_UTILS_VECTORTESTER_H
#define __SHIVA_TESTS_UTILS_VECTORTESTER_H

#include "shiva/include/unittest/src/shvtest.h"

#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvvector.h"


class SHVVectorTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionInsert = 0x01,
		ActionRemove = 0x02,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestInsert(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestRemove(SHVModuleList& modules, SHVTestBase* self, int flag);

};

#endif
