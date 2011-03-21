#ifndef __SHIVA_UTILS_MATHTOKENMAP_H
#define __SHIVA_UTILS_MATHTOKENMAP_H

#include "shvstring.h"

//-=========================================================================================================
/// SHVMathTokenMap class
/**
 * This is a base class for mapping math eval tokens to double.
 */

class SHVAPI SHVMathTokenMap
{
public:
	virtual bool LookupValue(const SHVStringC token, double& result) = 0;
};

#endif
