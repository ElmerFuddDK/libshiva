#ifndef __SHIVA_UTILS_DYNARRAYREF_H
#define __SHIVA_UTILS_DYNARRAYREF_H

#include "shvrefcount.h"


// ========================================================================================================
///  SHVDynArrayRefBase class - containing reference counting pointers
/**
 * Use this class when you want a dynamic array that contains objects based on SHVRefObject.
 */

class SHVAPI SHVDynArrayRefBase : protected SHVDynArrayBase
{
private:
	inline void Destroy(void* val) { ((SHVRefObject*)val)->DestroyRef(); }
public:


	inline SHVDynArrayRef(int growSize = 50, int initSize=0) : SHVDynArrayBase(growSize,initSize) {}
	inline ~SHVDynArrayRef() { SHVDynArrayBase::ClearAndInit(0,Destroy); }


	// properties
	inline size_t GetCount()			{ return SHVDynArrayBase::GetCount(); }
	inline size_t GetArrayLen()			{ return SHVDynArrayBase::GetArrayLen(); }
	inline size_t GetGrowSize()			{ return SHVDynArrayBase::GetGrowSize(); }
	inline void   SetGrowSize(size_t s) { SHVDynArrayBase::SetGrowSize(s); }


	// array management
	inline void ClearAndInit(int initSize=0) { SHVDynArrayBase::ClearAndInit(initSize, Destroy); }
	inline bool Resize(int count)			 { return SHVDynArrayBase::Resize(count); }

	inline SHVRefObject* operator[](size_t index)	{ return (SHVRefObject*)(*(SHVDynArrayBase*)this)[index]; }
	inline size_t Add(SHVRefObject* item)			{ return SHVDynArrayBase::Add((void*)item->CreateRef()); }
	inline void   Remove(size_t index);				{ Destroy((SHVRefObject*)SHVDynArrayBase::Remove(index)); }

	inline void Truncate()	{ SHVDynArrayBase::Truncate(); }
	inline void Compress()	{ SHVDynArrayBase::Compress(); }
};

#endif
