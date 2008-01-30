#ifndef __SHIVA_UTILS_DYNARRAYBASE_H
#define __SHIVA_UTILS_DYNARRAYBASE_H


//-=========================================================================================================
/// SHVDynArrayBase class - A basic dynamic array containing void pointers
/**
 * SHVDynArrayBase contains basic functionality for managing a dynamic array. It however doesn't handle
 * cleanup of the objects. If you use this class you will need to implement functionality to delete objects
 * in the list.\n
 * It uses pointers in order to provide fast resizing of the array.
 \note This class is not intended for basic usage. Please use one of the derived classes instead.
 */

class SHVAPI SHVDynArrayBase
{
public:

	// defines
	typedef int (*SortFunc)(void**,void**);
	typedef int (*MatchFunc)(void**,void**);
	typedef void (*DestroyFunc)(void*);
	enum { InvalidIndex = SIZE_T_MAX };


	SHVDynArrayBase(int growSize, int initSize=0, bool zeroed = false);
	~SHVDynArrayBase();


	// properties
	inline size_t GetCount() const;
	inline size_t GetArrayLen() const;
	inline int    GetGrowSize() const;
	inline void   SetGrowSize(int s);


	// array management
	void ClearAndInit(int initSize=0,DestroyFunc func=NULL);
	bool Resize(int count);

	void*  operator[](size_t index);
	size_t Add(void* item);
	bool   InsertAt(size_t index, void* item); ///< only works if zeroed and if the index is NULL  .. for now
	void*  Remove(size_t index);
	void*  Replace(size_t index, void* item);

	void Truncate();
	void Compress();


	// sorting
	void Sort(SortFunc func);
	void BubbleSort(size_t index, SortFunc func);
	bool Swap(size_t index1, size_t index2);


	// searching
	size_t FindMatchIndex(void* val, MatchFunc func);
	void* FindMatch(void* val, MatchFunc func);
	size_t FindFirstMatchIndex(void* val, MatchFunc func);
	void* FindFirstMatch(void* val, MatchFunc func);

private:
	void** Array;
	size_t Items,ArrayLen;
	int GrowSize;
	bool Zeroed;
};




// ============================================ implementation ============================================ //

size_t SHVDynArrayBase::GetCount() const
{ return Items; }
size_t SHVDynArrayBase::GetArrayLen() const
{ return ArrayLen; }
int    SHVDynArrayBase::GetGrowSize() const
{ return GrowSize; }
void   SHVDynArrayBase::SetGrowSize(int s)
{ GrowSize = s; }

#endif
