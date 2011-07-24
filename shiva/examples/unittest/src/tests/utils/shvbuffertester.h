#ifndef __SHIVA_TESTS_UTILS_BUFFERTESTER_H
#define __SHIVA_TESTS_UTILS_BUFFERTESTER_H

#include "shiva/include/unittest/src/shvtest.h"

#include "shiva/include/utils/shvbuffer.h"
#include "shiva/include/utils/shvbuffercstream.h"


class SHVBufferTester : public SHVTest
{
public:

	// test modes
	enum Actions {
		ActionRead = 0x01,
		ActionWrite = 0x02,
		ActionChunk = 0x04,
		ActionStream = 0x08,
	};

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;

	// sub tests
private:
	static bool TestRead(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestWrite(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestChunk(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestStream(SHVModuleList& modules, SHVTestBase* self, int flag);

	// helpers
private:
	static bool PerformRead(const SHVBufferC& testBuffer);
};

#endif
