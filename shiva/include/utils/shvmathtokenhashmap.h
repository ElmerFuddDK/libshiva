#ifndef __SHIVA_UTILS_MATHTOKENHASHMAP_H
#define __SHIVA_UTILS_MATHTOKENHASHMAP_H

#include "shvmath.h"
#include "shvhashtable.h"

//-=========================================================================================================
/// SHVMathTokenHashMap class
/**
 * SHVMathTokenMap implementation using a hash table
 */

class SHVAPI SHVMathTokenHashMap : public SHVMathTokenMap
{
public:
	
	// publicly available data
	SHVHashTableString<double> Map;
	
	
	// constructor
	SHVMathTokenHashMap();
	
	virtual bool LookupValue(const SHVStringC token, double& result);
};

#endif
