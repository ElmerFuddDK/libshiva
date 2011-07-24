#ifndef __SHIVA_TESTS_UTILS_DYNARRAYTESTER_H
#define __SHIVA_TESTS_UTILS_DYNARRAYTESTER_H

#include "shiva/include/unittest/src/shvtest.h"

#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvdynarray.h"


class SHVDynArrayTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionInsert = 0x01,
		ActionSort = 0x02,
		ActionRemove = 0x04,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestInsert(SHVModuleList& modules, SHVTestBase* self, int);
	static bool TestSort(SHVModuleList& modules, SHVTestBase* self, int);
	static bool TestRemove(SHVModuleList& modules, SHVTestBase* self, int);

	// helpers
private:
	static void InsertData(SHVDynArray<SHVString,const SHVStringC>& array);
	static int SortFunc(SHVString** s1, SHVString** s2);
	static int MatchFunc(const SHVStringC** s1, SHVString** s2);
	static int PerformMatch(const SHVStringC* s1, SHVString* s2);
};

#endif
