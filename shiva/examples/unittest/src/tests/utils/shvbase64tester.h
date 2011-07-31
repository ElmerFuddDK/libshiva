#ifndef __SHIVA_TESTS_UTILS_BASE64TESTER_H
#define __SHIVA_TESTS_UTILS_BASE64TESTER_H

#include "shiva/include/unittest/src/shvtest.h"


class SHVBase64Tester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionSimpleEncoding = 0x01,
		ActionSimpleDecoding = 0x02,
		ActionEncoding = 0x04,
		ActionDecoding = 0x08,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestSimpleEncoding(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestSimpleDecoding(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestEncoding(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestDecoding(SHVModuleList& modules, SHVTestBase* self, int flag);

};

#endif
