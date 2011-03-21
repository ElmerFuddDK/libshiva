#ifndef __SHIVA_UTILS_HASHKEY_H
#define __SHIVA_UTILS_HASHKEY_H


//-=========================================================================================================
/// SHVHashValue class - hash key container
/**
 */

class SHVAPI SHVHashValue
{
public:


	inline SHVHashValue(size_t value);
	inline SHVHashValue(void* value);

	inline size_t GetValue() const;

	inline bool operator==(const SHVHashValue& value) const;

private:
	///\cond INTERNAL
	size_t Value;
	///\endcond
};


// ============================================ implementation ============================================ //
/// \class SHVHashValue shvhashvalue.h "shiva/include/utils/shvhashvalue.h"

/*************************************
 * Constructor
 *************************************/
SHVHashValue::SHVHashValue(size_t value) : Value(value)
{
}
SHVHashValue::SHVHashValue(void* value) : Value((size_t)value)
{
}

/*************************************
 * GetValue
 *************************************/
size_t SHVHashValue::GetValue() const
{
	return Value;
}

/*************************************
 * operator==
 *************************************/
bool SHVHashValue::operator==(const SHVHashValue& value) const
{
	return Value == value.Value;
}

#endif
