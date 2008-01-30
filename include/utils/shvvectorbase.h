#ifndef __SHIVA_UTILS_VECTORBASE_H
#define __SHIVA_UTILS_VECTORBASE_H


//-=========================================================================================================
/// SHVVectorBase class
/**
 * This class implements basic functionality of a vector. This class needs to be cleared on destroy,
 * since it doesn't know how to destroy its elements.\n
 * You will need to always provide the same grow size to the functions, otherwise unpredictable things
 * will happen.
 \note This class is not intended for basic usage. Please use one of the derived classes instead.
 */

class SHVAPI SHVVectorBase
{
public:

	// defines
	typedef void (*DestroyFunc)(void*);


	// constructor
	SHVVectorBase();
	~SHVVectorBase();


	// properties
	size_t CalculateCount();
	bool IsEmpty();
	inline bool Eof(size_t index);


	// array management
	void Clear(DestroyFunc func = NULL);

	void* operator[](size_t index);
	size_t Add(void* item, size_t growSize);
	void* Remove(size_t index);
	void* Replace(size_t index, void* item);
	void* Pop(size_t growSize);

	void Truncate(size_t growSize);
	void Compress(size_t growSize);


private:
	void** Array;
	void** ArrayEnd;
};


// ============================================ implementation ============================================ //

bool SHVVectorBase::Eof(size_t index)
{
	return ( (Array+index) < ArrayEnd );
}
#endif
