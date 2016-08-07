#ifndef __SHIVA_UTILS_VECTORBASE_H
#define __SHIVA_UTILS_VECTORBASE_H

///\cond INTERNAL
class SHVVectorBufferBase;
///\endcond


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
	SHVVectorBase(const SHVVectorBufferBase& buffer);
	~SHVVectorBase();
	
	SHVVectorBase& operator=(const SHVVectorBufferBase& buffer);


	// properties
	size_t CalculateCount() const;
	bool IsEmpty();
	inline bool Eof(size_t index);


	// array management
	void Clear(DestroyFunc func = NULL);

	void* operator[](size_t index);
	size_t Add(void* item, size_t growSize);
	SHVVectorBase& AddFromBuffer(const SHVVectorBufferBase& buffer, size_t growSize);
	void* Remove(size_t index);
	void* Replace(size_t index, void* item);
	void* Pop(size_t growSize);
	inline void* Get(size_t index);

	void Truncate(size_t growSize);
	void Compress(size_t growSize);
	
	SHVVectorBufferBase ReleaseBuffer();


private:
	void** Array;
	void** ArrayEnd;
	///\cond INTERNAL
	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVVectorBase);
	///\endcond
};



// ============================================ implementation ============================================ //

///\cond INTERNAL
//-=========================================================================================================
/// SHVVectorBufferBase class

class SHVAPI SHVVectorBufferBase
{
friend class SHVVectorBase;
	inline SHVVectorBufferBase() {}
public:
	SHVVectorBufferBase(const SHVVectorBufferBase& buffer);
	virtual ~SHVVectorBufferBase();

	void Clear(SHVVectorBase::DestroyFunc func = NULL);
private:
	void** Array;
	void** ArrayEnd;
};
///\endcond

bool SHVVectorBase::Eof(size_t index)
{
	return ( (Array+index) < ArrayEnd );
}

void* SHVVectorBase::Get(size_t index)
{
	return (*this)[index];
}
#endif
