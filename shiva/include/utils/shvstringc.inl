#if !defined(__SHIVA_UTILS_STRINGC_INL) && !defined(__SHIVA_UTILS_STRING_CPP)
#define __SHIVA_UTILS_STRINGC_INL


// ====================================== implementation - SHVStringC ======================================= //

const SHVString8C SHVString8C::AsStr8C() const             { return *this; }
const SHVStringBuffer16 SHVString8C::AsStr16C() const      { return ToStr16(); }
const SHVStringBufferUTF8 SHVString8C::AsStrUTF8C() const  { return ToStrUTF8(); }

const SHVStringBuffer8 SHVString16C::AsStr8C() const       { return ToStr8(); }
const SHVString16C SHVString16C::AsStr16C() const          { return *this; }
const SHVStringBufferUTF8 SHVString16C::AsStrUTF8C() const { return ToStrUTF8(); }

const SHVStringBuffer8 SHVStringUTF8C::AsStr8C() const     { return ToStr8(); }
const SHVStringBuffer16 SHVStringUTF8C::AsStr16C() const   { return ToStr16(); }
const SHVStringUTF8C SHVStringUTF8C::AsStrUTF8C() const    { return *this; }

SHVStringUTF8C::SHVStringUTF8C(const SHVChar* buffer) { Buffer = (SHVChar*)buffer; }
const SHVChar* SHVStringUTF8C::GetBufferConst() const { return Buffer; }
bool SHVStringUTF8C::IsNull() const { return Buffer == NULL; }
bool SHVStringUTF8C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
#ifdef __SHIVA_EPOC
TPtrC8 SHVStringUTF8C::ToPtr() const { return TPtrC8((TUint8*)Buffer,GetLength()); }
#endif
size_t SHVStringUTF8C::GetLengthInBytes() const { return GetLength(); }
size_t SHVStringUTF8C::StrLenInBytes(const SHVChar* str) { return SHVStringUTF8C::StrLen(str); }


/*************************************
 * ToStrT
 *************************************/
SHVStringBuffer SHVString8C::ToStrT() const
{
#if __SHVSTRINGDEFAULT == 8
	return ToStr8();
#elif __SHVSTRINGDEFAULT == 16
	return ToStr16();
#elif __SHVSTRINGDEFAULT == utf8
	return ToStrUTF8();
#endif
}
SHVStringBuffer SHVStringUTF8C::ToStrT() const
{
#if __SHVSTRINGDEFAULT == 8
	return ToStr8();
#elif __SHVSTRINGDEFAULT == 16
	return ToStr16();
#elif __SHVSTRINGDEFAULT == utf8
	return ToStrUTF8();
#endif
}


// ===================================== implementation - SHVStringCRef ===================================== //
SHVStringUTF8CRef::SHVStringUTF8CRef() { Buffer = NULL; }
bool SHVStringUTF8CRef::operator==(const SHVChar* str) const { return SHVStringUTF8C::operator==(str); }
bool SHVStringUTF8CRef::operator!=(const SHVChar* str) const { return SHVStringUTF8C::operator!=(str); }
bool SHVStringUTF8CRef::operator<(const SHVChar* str) const  { return SHVStringUTF8C::operator<(str); }
bool SHVStringUTF8CRef::operator<=(const SHVChar* str) const { return SHVStringUTF8C::operator<=(str); }
bool SHVStringUTF8CRef::operator>(const SHVChar* str) const  { return SHVStringUTF8C::operator>(str); }
bool SHVStringUTF8CRef::operator>=(const SHVChar* str) const { return SHVStringUTF8C::operator>=(str); }
bool SHVStringUTF8CRef::operator==(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator==(str); }
bool SHVStringUTF8CRef::operator!=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator!=(str); }
bool SHVStringUTF8CRef::operator<(const SHVStringUTF8C str) const  { return SHVStringUTF8C::operator<(str); }
bool SHVStringUTF8CRef::operator<=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator<=(str); }
bool SHVStringUTF8CRef::operator>(const SHVStringUTF8C str) const  { return SHVStringUTF8C::operator>(str); }
bool SHVStringUTF8CRef::operator>=(const SHVStringUTF8C str) const { return SHVStringUTF8C::operator>=(str); }


// ==================================== implementation - SHVStringBuffer ==================================== //
///\cond INTERNAL
SHVStringBufferUTF8::SHVStringBufferUTF8() { Buffer = NULL; }
///\endcond


// ==================================== implementation - SHVString16C ==================================== //

/*************************************
 * ToStrT
 *************************************/
#ifndef __SHVSTRING_EXCLUDE_UNICODE
SHVStringBuffer SHVString16C::ToStrT() const
{
#if __SHVSTRINGDEFAULT == 8
	return ToStr8();
#elif __SHVSTRINGDEFAULT == 16
	return ToStr16();
#elif __SHVSTRINGDEFAULT == utf8
	return ToStrUTF8();
#endif
}
#endif

#endif
