#ifndef __SHIVA_STRING_SQLITE_H
#define __SHIVA_STRING_SQLITE_H

#include "../utils/shvstring.h"
#include "../utils/shvstringutf8.h"

//-=========================================================================================================
/// SHVStringSQLite class - Constant sqlite string
/**
 * This class holds a sqlite string for which sqlite is responsible for allocating and freeing.
 * In reality it's a SHVStringUTF8C, but with operator restrictions. This is done to prevent
 * automatic conversions to SHVStringUTF8 which would be fatal, as this class owns the 
 * embedded string.
 */
class SHVStringSQLite
{
public:
	inline SHVStringSQLite(const SHVChar* str): string(str) {}
	inline operator const SHVStringUTF8C() const;
	inline SHVStringBuffer8 ToStr8() const;
	inline SHVStringBuffer16 ToStr16() const;
	inline SHVStringBuffer ToStrT() const;
	inline bool IsNull() const;

	inline const SHVChar* GetBufferConst() const;
	inline const SHVChar* GetSafeBuffer() const; 

	inline bool operator==(const SHVStringSQLite& val) const;
	inline bool operator!=(const SHVStringSQLite& val) const;
	inline SHVStringBufferUTF8 Right(size_t len) const;
	inline SHVStringBufferUTF8 Left(size_t len) const;
	inline SHVStringBufferUTF8 Mid(size_t first,size_t length = SIZE_T_MAX) const;
	inline long Find(const SHVStringUTF8C& str,long offset=0) const;
	inline long ReverseFind(const SHVStringUTF8C& str) const;
private:
	SHVStringUTF8C string;
};

// ========================================= implementation ================================================= //

/*************************************
 * operator const SHVStringUTF8C
 *************************************/
SHVStringSQLite::operator const SHVStringUTF8C() const 
{ 
	return string; 
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBuffer8 SHVStringSQLite::ToStr8() const
{
	return string.ToStr8();
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBuffer16 SHVStringSQLite::ToStr16() const
{
	return string.ToStr16();
}

/*************************************
 * ToStrT
 *************************************/
SHVStringBuffer SHVStringSQLite::ToStrT() const
{
	return string.ToStrT();
}

/*************************************
 * IsNull
 *************************************/
bool SHVStringSQLite::IsNull() const
{
	return string.IsNull();
}

/*************************************
 * GetBufferConst
 *************************************/
const SHVChar* SHVStringSQLite::GetBufferConst() const
{
	return string.GetBufferConst();
}

/*************************************
 * GetSafeBuffer
 *************************************/
const SHVChar* SHVStringSQLite::GetSafeBuffer() const
{
	return string.GetSafeBuffer();
}

/*************************************
 * operator==
 *************************************/
bool SHVStringSQLite::operator==(const SHVStringSQLite& val) const
{
	return string == val.string;
}

/*************************************
 * operator!=
 *************************************/
bool SHVStringSQLite::operator!=(const SHVStringSQLite& val) const
{
	return string != val.string;
}

/*************************************
 * Right
 *************************************/
SHVStringBufferUTF8 SHVStringSQLite::Right(size_t len) const
{
	return string.Right(len);
}

/*************************************
 * Left
 *************************************/
SHVStringBufferUTF8 SHVStringSQLite::Left(size_t len) const
{
	return string.Left(len);
}

/*************************************
 * Mid
 *************************************/
SHVStringBufferUTF8 SHVStringSQLite::Mid(size_t first,size_t length) const
{
	return string.Mid(first, length);
}

/*************************************
 * Find
 *************************************/
long SHVStringSQLite::Find(const SHVStringUTF8C& str,long offset) const
{
	return string.Find(str, offset);
}

/*************************************
 * ReverseFind
 *************************************/
long SHVStringSQLite::ReverseFind(const SHVStringUTF8C& str) const
{
	return string.ReverseFind(str);
}

#endif