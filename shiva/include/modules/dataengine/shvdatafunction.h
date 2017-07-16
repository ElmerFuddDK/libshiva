#ifndef __SHIVA_DATAFUNCTION_H
#define __SHIVA_DATAFUNCTION_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"

// forward declares
class SHVDataSession;
class SHVDataFactory;
class SHVDataRowListC;
//-=========================================================================================================
/// SHVDataFunction class - Interface for SHVDataFunction
/**
 * Interface for a generic shiva data function
 */
class SHVDataFunction: public SHVRefObject
{
public:

	inline SHVBool SetParameterIntUTF8(const SHVStringUTF8C& name, int val);
	inline SHVBool SetParameterInt(const SHVStringC& name, int val);

	virtual SHVBool SetParameterLongUTF8(const SHVStringUTF8C& name, long val) = 0;
	inline SHVBool SetParameterLong(const SHVStringC& name, long val);

	virtual SHVBool SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val) = 0;
	inline SHVBool SetParameterInt64(const SHVStringC& name, SHVInt64Val val);

	virtual SHVBool SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val) = 0;
	inline SHVBool SetParameterDouble(const SHVStringC& name, double val);

	virtual SHVBool SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val) = 0;
	inline SHVBool SetParameterString(const SHVStringC& name, const SHVStringC& val);

	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name) = 0;
	inline SHVBool SetParameterNull(SHVStringC& name);

	virtual SHVDataRowListC* Exec() = 0;
	virtual SHVBool GetLastError() = 0;

	virtual bool GetInUse() = 0;

	virtual void ClearAllParameters() = 0;
};
typedef SHVRefObjectContainer<SHVDataFunction> SHVDataFunctionRef;


// ============================================ implementation ============================================ //
/*************************************
 * SetParameter
 *************************************/
inline SHVBool SHVDataFunction::SetParameterIntUTF8(const SHVStringUTF8C& name, int val)
{
	return SetParameterLongUTF8(name, val);
}

inline SHVBool SHVDataFunction::SetParameterInt(const SHVStringC& name, int val)
{
	return SetParameterLong(name, val);
}

SHVBool SHVDataFunction::SetParameterLong(const SHVStringC& name, long val)
{
	return SetParameterLongUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataFunction::SetParameterInt64(const SHVStringC& name, SHVInt64Val val)
{
	return SetParameterInt64UTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataFunction::SetParameterDouble(const SHVStringC& name, double val)
{
	return SetParameterDoubleUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataFunction::SetParameterString(const SHVStringC& name, const SHVStringC& val)
{
	return SetParameterStringUTF8(name.ToStrUTF8(), val.ToStrUTF8());
}

SHVBool SHVDataFunction::SetParameterNull(SHVStringC& name)
{
	return SetParameterNullUTF8(name.ToStrUTF8());
}


#endif // __SHIVA_DATAFUNCTION_H
