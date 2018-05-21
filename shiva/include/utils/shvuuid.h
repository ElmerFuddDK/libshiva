#ifndef __SHIVA_UTILS_UUID_H
#define __SHIVA_UTILS_UUID_H

#include "shvstring.h"


//-=========================================================================================================
/// SHVUUID class
/**
 * This class is used for generating UUID's. This class holds the seeds for
 * generating a proper unique ID, so use the same object to generate all the
 * UUID's with.
 */

class SHVAPI SHVUUID
{
public:


	// defines
	struct SHVAPI ID
	{
		SHVChar Bytes[16];

		operator SHVHashValue() const;
		bool operator==(const ID& id) const;

		SHVStringBuffer8 ToString8();
		SHVStringBufferUTF8 ToStringUTF8();
		SHVStringBuffer16 ToString16();
		inline SHVStringBuffer ToString();
	};


	// constructor
	SHVUUID();
	~SHVUUID();


	// initialization
	void SetNodeData();
	void SetNodeData(void* data, size_t sz);
	void SetNodeDataAsMd5(void* data, size_t sz);
	inline void SetNodeData(int data);
	inline void SetNodeDataString8AsMd5(const SHVString8C data);
	inline void SetNodeDataStringUTF8AsMd5(const SHVStringUTF8C data);
	inline void SetNodeDataString16AsMd5(const SHVString16C data);
	inline void SetNodeDataStringAsMd5(const SHVStringC data);


	// properties
	static ID Null();

	ID Create();
	ID CreateMd5FromNamespace8(const ID& namespc, const SHVString8C name);
	ID CreateMd5FromNamespaceUTF8(const ID& namespc, const SHVStringUTF8C name);
	ID CreateMd5FromNamespace16(const ID& namespc, const SHVString16C name);
	inline ID CreateMd5FromNamespace(const ID& namespc, const SHVStringC name);

	static ID FromString8(const SHVString8C uuid);
	static ID FromStringUTF8(const SHVStringUTF8C uuid);
	static ID FromString16(const SHVString16C uuid);
	inline static ID FromString(const SHVStringC uuid);


private:
	///\cond INTERNAL
	void* Globals;
	static int CharToInt(bool& ok, const char ch);
	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVUUID);
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * ID::ToString
 *************************************/
SHVStringBuffer SHVUUID::ID::ToString()
{
#if __SHVSTRINGDEFAULT == 8
	return ToString8();
#elif __SHVSTRINGDEFAULT == 16
	return ToString16();
#elif __SHVSTRINGDEFAULT == utf8
	return ToStringUTF8();
#endif
}

/*************************************
 * CreateMd5FromNamespace
 *************************************/
SHVUUID::ID SHVUUID::CreateMd5FromNamespace(const ID& namespc, const SHVStringC name)
{
#if __SHVSTRINGDEFAULT == 8
	return CreateMd5FromNamespace8(namespc,name);
#elif __SHVSTRINGDEFAULT == 16
	return CreateMd5FromNamespace16(namespc,name);
#elif __SHVSTRINGDEFAULT == utf8
	return CreateMd5FromNamespaceUTF8(namespc,name);
#endif
}

/*************************************
 * FromString
 *************************************/
SHVUUID::ID SHVUUID::FromString(const SHVStringC uuid)
{
#if __SHVSTRINGDEFAULT == 8
	return FromString8(uuid);
#elif __SHVSTRINGDEFAULT == 16
	return FromString16(uuid);
#elif __SHVSTRINGDEFAULT == utf8
	return FromStringUTF8(uuid);
#endif
}

/*************************************
 * SetNodeData*
 *************************************/
void SHVUUID::SetNodeData(int data)
{
	SetNodeData(&data,sizeof(int));
}
void SHVUUID::SetNodeDataString8AsMd5(const SHVString8C data)
{
	SetNodeDataAsMd5((void*)data.GetSafeBuffer(),data.GetSizeInBytes());
}
void SHVUUID::SetNodeDataStringUTF8AsMd5(const SHVStringUTF8C data)
{
	SetNodeDataAsMd5((void*)data.GetSafeBuffer(),data.GetSizeInBytes());
}
void SHVUUID::SetNodeDataString16AsMd5(const SHVString16C data)
{
	SetNodeDataAsMd5((void*)data.GetSafeBuffer(),data.GetSizeInBytes());
}
void SHVUUID::SetNodeDataStringAsMd5(const SHVStringC data)
{
	SetNodeDataAsMd5((void*)data.GetSafeBuffer(),data.GetSizeInBytes());
}

#endif
