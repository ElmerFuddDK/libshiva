#ifndef __SHIVA_TESTS_UTILS_FILETESTER_H
#define __SHIVA_TESTS_UTILS_FILETESTER_H

#include "shiva/include/unittest/src/shvtest.h"



class SHVFileTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionCreation = 0x01,
		ActionStorage = 0x02,
		ActionTextHandling = 0x04,
		ActionUnicodeFilenames = 0x08,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestCreation(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestStorage(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestTextHandling(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestUnicodeFilenames(SHVModuleList& modules, SHVTestBase* self, int flag);

	// helpers
private:
};
#endif
