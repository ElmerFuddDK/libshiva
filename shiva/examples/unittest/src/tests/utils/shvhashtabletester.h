#ifndef __SHIVA_TESTS_UTILS_HASHTABLETESTER_H
#define __SHIVA_TESTS_UTILS_HASHTABLETESTER_H

#include "shiva/include/unittest/src/shvtest.h"

#include "shiva/include/utils/shvhashtable.h"


class SHVHashTableTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionStringInsert = 0x01,
		ActionReleaseBuffer = 0x02,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestStringInsert(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestReleaseBuffer(SHVModuleList& modules, SHVTestBase* self, int flag);

	// helpers
private:
	static void InsertStrings(SHVHashTableString<SHVString,const SHVStringC>& table);
};
#endif
