#ifndef __SHIVA_TESTS_UTILS_STRINGTESTER_H
#define __SHIVA_TESTS_UTILS_STRINGTESTER_H

#include "shiva/include/unittest/src/shvtest.h"

#include "shiva/include/utils/shvstring.h"


class SHVStringTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionComparison = 0x01,
		ActionConversion = 0x02,
		ActionRightLeftMid = 0x04,
		ActionTokenizeTrim = 0x08,
		ActionFindLocate = 0x10,
		ActionReplaceFormat = 0x20,
		ActionUTF8Encoding = 0x40,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestComparison(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestConversion(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestRightLeftMid(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestTokenizeTrim(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestFindLocate(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestReplaceFormat(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestUTF8Encoding(SHVModuleList& modules, SHVTestBase* self, int flag);

	// helpers
private:
};
#endif
