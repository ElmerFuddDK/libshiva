#ifndef __SHIVA_UTILS_STRINGUTF8C_INL
#define __SHIVA_UTILS_STRINGUTF8C_INL


// ====================================== implementation - SHVStringC ======================================= //

SHVStringUTF8C::SHVStringUTF8C(const SHVChar* buffer) { Buffer = (SHVChar*)buffer; }
const SHVChar* SHVStringUTF8C::GetBufferConst() const { return Buffer; }
bool SHVStringUTF8C::IsNull() const { return Buffer == NULL; }
bool SHVStringUTF8C::IsEmpty() const { return Buffer == NULL || *Buffer == 0; }
size_t SHVStringUTF8C::GetSizeInChars() const { return SHVString8C::StrLen(Buffer); }
size_t SHVStringUTF8C::GetSizeInBytes() const { return SHVString8C::StrLen(Buffer); }
#ifdef __SHIVA_EPOC
TPtrC8 SHVStringUTF8C::ToPtr() const { return TPtrC8((TUint8*)Buffer,GetLength()); }
#endif
size_t SHVStringUTF8C::StrSizeInChars(const SHVChar* str) { return SHVString8C::StrLen(str); }
size_t SHVStringUTF8C::StrSizeInBytes(const SHVChar* str) { return SHVString8C::StrLen(str); }

/*************************************
 * ToStrT
 *************************************/
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
