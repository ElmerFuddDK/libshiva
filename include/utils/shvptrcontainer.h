#ifndef __SHIVA_UTILS_PTRCONTAINER_H
#define __SHIVA_UTILS_PTRCONTAINER_H



//-=========================================================================================================
/// SHVPtrContainer class template
/**
 * Contains a reference to a pointer. It will keep track of the pointer during stack copying and assignments,
 * making sure there is only one reference active.\n
 * If the container is destroyed when holding the pointer it will destroy the object pointed to. An example:
\code

SHVPtrContainer<SHVChar> AllocString()
{
SHVPtrContainer<SHVChar> retVal((SHVChar*)::malloc(12));
	strcpy(retVal,"Hello World");
	return retVal; // copies over the pointer to the container returned
}

void main()
{
SHVPtrContainer<SHVChar> ptr1;

	if (ptr1.IsNull())
	{
	SHVPtrContainer<SHVChar> ptr2;

		AllocString();			// the allocated string is destroyed by the container
								// since the pointer was not copied

		ptr2 = AllocString();	// the allocated string is assigned to ptr1 and not
								// destroyed

		ptr1 = ptr2;			// the valid reference is moved to ptr1, and ptr2 is cleared
	} // the string in ptr2 is not deallocated since the valid reference is in ptr1 now

	printf(ptr1); // the container can be used as a normal pointer

} // now the data in ptr1 will be destroyed
\endcode
 */

template <class T>
class SHVAPI SHVPtrContainer
{
public:


	// constructors
	inline SHVPtrContainer();
	inline SHVPtrContainer(T* obj);
	inline SHVPtrContainer(const SHVPtrContainer<T> &r);
	inline ~SHVPtrContainer();


	inline bool IsNull() const;


	// pointer access functions
	inline T* operator ->() const;
	inline operator T*();
	inline const T* AsConst() const;


	// assignment
	inline void Clear();
	inline const SHVPtrContainer<T>& operator =(const SHVPtrContainer<T> &r);
	inline SHVPtrContainer<T>& operator =(T* obj);


	// comparison
	inline bool operator ==(const T* r) const;


	// Release the valid reference to the object
	inline T* ReleaseReference();


private:
	///\cond INTERNAL
	T* Obj;
	bool RefValid;
	
	inline void ClearPtr() const;
	///\endcond
};



// ============================================ implementation ============================================ //

/*************************************
 * Constructors
 *************************************/
template <class T>
SHVPtrContainer<T>::SHVPtrContainer() : RefValid(true) { Obj = NULL; }
template <class T>
SHVPtrContainer<T>::SHVPtrContainer(T* obj) : RefValid(true) { Obj = obj; }
template <class T>
SHVPtrContainer<T>::SHVPtrContainer(const SHVPtrContainer<T> &r) : Obj((T*)r.Obj) { RefValid=r.RefValid; r.ClearPtr(); }


/*************************************
 * Destructor
 *************************************/
template <class T>
SHVPtrContainer<T>::~SHVPtrContainer() { if (Obj && RefValid) delete Obj; }

/*************************************
 * IsNull
 *************************************/
/// Check if the container is null
/**
 \return true if internal pointer is null
 *
 * Use this function to check the pointer state. Using the pointer
 * cast as null check does not work on all platforms:
\code
SHVPtrContainer<int> intPtr = new int;

	if (intPtr) // the WRONG way
		*intPtr = 1;

	if (!intPtr.IsNull()) // the right way
		*intPtr = 1;
\endcode
 */
template <class T>
bool SHVPtrContainer<T>::IsNull() const { return (Obj ? false : true); }

/*************************************
 * Pointer operator
 *************************************/
/// Pointer operator
/**
 \return Pointer to object contained
 */
template <class T>
T* SHVPtrContainer<T>::operator ->() const { return Obj; }

/*************************************
 * Operator T
 *************************************/
/// T* operator
/**
 \return pointer to object contained
 */
template <class T>
SHVPtrContainer<T>::operator T*() { return Obj; }

/*************************************
 * AsConst
 *************************************/
/// T* const function
/**
 \return pointer to object contained as const
 */
template <class T>
const T* SHVPtrContainer<T>::AsConst() const { return (const T*)Obj; }


/*************************************
 * Clear
 *************************************/
/// Deletes the referenced object, setting the pointer to NULL
template <class T>
void SHVPtrContainer<T>::Clear() { if (Obj && RefValid) {delete Obj;} Obj = NULL; }

/*************************************
 * assignment operator
 *************************************/
/// Will clear the pointer in the container assigned from
/**
 \see ReleaseReference
 */
template <class T>
const SHVPtrContainer<T>& SHVPtrContainer<T>::operator =(const SHVPtrContainer<T> &r)
	{ if (Obj && RefValid) delete Obj; Obj=(T*)r.Obj; RefValid=r.RefValid; r.ClearPtr(); return *this; }
/// Assigns the pointer to the container
template <class T>
SHVPtrContainer<T>& SHVPtrContainer<T>::operator =(T* obj)
	{ if (Obj && RefValid) delete Obj; Obj = obj; RefValid = true; return *this; }

/*************************************
 * comparison operator
 *************************************/
template <class T>
bool SHVPtrContainer<T>::operator ==(const T* r) const { return r == Obj; }

/*************************************
 * ReleaseReference
 *************************************/
/// Releases the reference to the object
/**
 \return Pointer to object
 *
 * Use this if you want to return the pointer, but still keep it in the container:
\code
SHVPtrContainer<int> ptr1 = new int;
SHVPtrContainer<int> ptr2;

	ptr2 = ptr1;

	if (ptr1.IsNull()) // this will be true now due to the assignment
		ptr1 = ptr2.ReleaseReference();

	if (ptr1 == ptr2) // this will be true due to use of ReleaseReference
		ptr1 = NULL; // will delete the object, rendering the pointer in ptr2 INVALID
\endcode
 */
template <class T>
T* SHVPtrContainer<T>::ReleaseReference() { RefValid = false; return Obj; }

///\cond INTERNAL
/*************************************
 * ClearPtr
 *************************************/
template <class T>
void SHVPtrContainer<T>::ClearPtr() const { ((SHVPtrContainer<T>*)this)->Obj = NULL; }
///\endcond

#endif
