#ifndef __SHIVA_MODULES_UNITTEST_H
#define __SHIVA_MODULES_UNITTEST_H

#include "../framework/shvmodule.h"
#include "shvtestlogger.h"
#include "shvtestbase.h"


//-=========================================================================================================
/// SHVUnitTest class
/**
 * Interface for a unit test module. Implemented in the
 * example project of the same name.
 */

class SHVUnitTest : public SHVModule
{
public:

	virtual void RegisterTest(SHVTestBase* test) = 0;
	virtual bool IsPerforming() = 0;

protected:
	inline SHVUnitTest(SHVModuleList& modules) : SHVModule(modules,"UnitTest") {}
};


// ============================================== documentation ============================================= //

/// \class SHVUnitTest shvunittest.h "shiva/include/unittest/shvunittest.h"

#endif
