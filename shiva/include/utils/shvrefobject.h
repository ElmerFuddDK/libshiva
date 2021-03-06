#ifndef __SHIVA_UTILS_REFCOUNT_H
#define __SHIVA_UTILS_REFCOUNT_H



//-=========================================================================================================
/// SHVRefObject - base reference counting class
/**
 * This is the base class for all reference counting objects.
 */

class SHVAPI SHVRefObject
{
public:

	struct SHVAPI RefData
	{
		volatile int References;
		bool DeleteInProgress;
		
		inline void Initialize(int refs = 0);
		
		inline void LockedIncrement();
		inline void LockedDecrement();
		bool LockedDecrementAndCheckDelete();
		
		bool RefInvalid();
	};
	

	// property
	inline int GetRefCount();


	// Refcount management
	SHVRefObject* CreateRef();
	void ReleaseRef();
	void DestroyRef();
	inline void ValidateRefCount();


	static void LockedIncrement(volatile int* ref);
	static void LockedDecrement(volatile int* ref);


protected:
	///\cond INTERNAL
	// protect the base class
	inline SHVRefObject();
	inline bool ObjectIsDeleting() const;
	virtual ~SHVRefObject() {}
	inline SHVRefObject(const SHVRefObject&);
	inline SHVRefObject& operator=(const SHVRefObject&);
private:
	RefData References;
	///\endcond
};



//-=========================================================================================================
/// SHVRefObjectTemplate - refcounting template class
/**
 * Use this class to encapsulate a non refcounting in a refcounting structure:
\code
typedef SHVRefObjectTemplate<int> SHVIntObj;
typedef SHVRefObjectContainer<SHVIntObj> SHVIntRef;

void main()
{
SHVIntRef i1, i2;

	i1 = new SHVIntObj;

	i2 = i1; // i1 and i2 points to the same object

	i1 = NULL; // the object is not deleted yet, i2 has the final reference
	i2 = NULL; // the object is now deleted
}
\endcode
 */

template <class T>
class SHVRefObjectTemplate : public SHVRefObject
{
public:


	// constructor
	inline SHVRefObjectTemplate();
	inline SHVRefObjectTemplate(const T& obj);


	// Operators
	inline operator T&();
	inline T& Object();

private:
	///\cond INTERNAL
	T Obj;
	///\endcond
};



//-=========================================================================================================
/// SHVRefObjectContainer - template class for automating refcounting
/**
 * This class will keep a reference to an object up-to-date.
 */

template <class T>
class SHVRefObjectContainer
{
public:


	// constructors
	inline SHVRefObjectContainer();
	inline SHVRefObjectContainer(T *obj);
	inline SHVRefObjectContainer(const SHVRefObjectContainer<T> &r);
	inline ~SHVRefObjectContainer();


	inline bool IsNull() const;


	// pointer access functions
	inline T* operator ->() const;
	inline operator T*();
	inline const T* AsConst() const;


	// assignment
	inline void Clear();
	inline const SHVRefObjectContainer<T>& operator =(const SHVRefObjectContainer<T> &r);
	inline SHVRefObjectContainer<T>& operator =(T* obj);



	// Release the valid reference to the object
	inline T* ReleaseReference();


private:
	///\cond INTERNAL
	T* Obj;
	///\endcond
};
typedef SHVRefObjectContainer<SHVRefObject> SHVObjectRef;



// ================================ implementation - SHVRefObject::RefData ================================ //

///\cond INTERNAL
void SHVRefObject::RefData::Initialize(int refs) { References = refs; DeleteInProgress = false; }
void SHVRefObject::RefData::LockedIncrement() { SHVRefObject::LockedIncrement(&References); }
void SHVRefObject::RefData::LockedDecrement() { SHVRefObject::LockedDecrement(&References); }
///\endcond


// ===================================== implementation - SHVRefObject ==================================== //

///\cond INTERNAL
/*************************************
 * Constructor
 *************************************/
SHVRefObject::SHVRefObject() { References.Initialize(); }

/*************************************
 * ObjectIsDeleting
 *************************************/
bool SHVRefObject::ObjectIsDeleting() const { return References.DeleteInProgress; }

/*************************************
 * Copy constructor
 *************************************/
SHVRefObject::SHVRefObject(const SHVRefObject&) { References.Initialize(); }

/*************************************
 * Assignment operator
 *************************************/
SHVRefObject& SHVRefObject::operator=(const SHVRefObject&) { return *this; }
///\endcond

/*************************************
 * GetRefCount
 *************************************/
/// Returns the number of references to the object
int SHVRefObject::GetRefCount() { return References.References; }

/*************************************
 * ValidateRefCount
 *************************************/
/// Validates if the object needs to be deleted, and deletes if so
void SHVRefObject::ValidateRefCount() { if (References.RefInvalid()) delete this; }


// ================================= implementation - SHVRefObjectTemplate ================================ //
/// \class SHVRefObjectTemplate shvrefobject.h "shiva/include/utils/shvrefobject.h"

/*************************************
 * Constructors
 *************************************/
template <class T>
SHVRefObjectTemplate<T>::SHVRefObjectTemplate() {}
template <class T>
SHVRefObjectTemplate<T>::SHVRefObjectTemplate(const T& obj) : Obj(obj) {}

/*************************************
 * Operator T
 *************************************/
template <class T>
SHVRefObjectTemplate<T>::operator T&() { return Obj; }
/*************************************
 * Object
 *************************************/
template <class T>
T& SHVRefObjectTemplate<T>::Object() { return Obj; }


// ================================ implementation - SHVRefObjectContainer ================================ //
/// \class SHVRefObjectContainer shvrefobject.h "shiva/include/utils/shvrefobject.h"

/*************************************
 * Constructors
 *************************************/
template <class T>
SHVRefObjectContainer<T>::SHVRefObjectContainer() { Obj = NULL; }
template <class T>
SHVRefObjectContainer<T>::SHVRefObjectContainer(T *obj)  { Obj = ( obj ? (T *) obj->CreateRef() : NULL );  }
template <class T>
SHVRefObjectContainer<T>::SHVRefObjectContainer(const SHVRefObjectContainer<T> &r) { Obj = (!r.IsNull() ? (T *) r->CreateRef() : (T *)NULL); }
template <class T>
SHVRefObjectContainer<T>::~SHVRefObjectContainer() { if (Obj) Obj->DestroyRef(); }


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
typedef SHVRefObjectTemplate<int> SHVIntObj;
typedef SHVRefObjectContainer<SHVIntObj> SHVIntRef;

	...

SHVIntRef i = new SHVIntObj;

	if (i) // the WRONG way
		i->Object() = 1;

	if (!i.IsNull()) // the right way
		i->Object() = 1;
\endcode
 */
template <class T>
bool SHVRefObjectContainer<T>::IsNull() const { return (Obj ? false : true); }


/*************************************
 * Pointer operator
 *************************************/
/// Pointer operator
/**
 \return Pointer to object contained
 */
template <class T>
T* SHVRefObjectContainer<T>::operator ->() const { return Obj; }
/*************************************
 * Operator T
 *************************************/
/// T* operator
/**
 \return pointer to object contained
 */
template <class T>
SHVRefObjectContainer<T>::operator T*() { return Obj; }
/*************************************
 * AsConst
 *************************************/
/// T* const function
/**
 \return pointer to object contained as const
 */
template <class T>
const T* SHVRefObjectContainer<T>::AsConst() const { return (const T*)Obj; }


/*************************************
 * Clear
 *************************************/
/// Deletes the referenced object, setting the pointer to NULL
template <class T>
void SHVRefObjectContainer<T>::Clear() { if (Obj) { Obj->DestroyRef(); } Obj = NULL; }

/*************************************
 * assignment operator
 *************************************/
/// Assigns an object to the container
/**
 \see ReleaseReference
 */
template <class T>
const SHVRefObjectContainer<T>& SHVRefObjectContainer<T>::operator =(const SHVRefObjectContainer<T> &r)
	{ if (r.Obj == Obj) return *this; if (Obj) Obj->DestroyRef(); Obj = ( !r.IsNull() ? (T *) r->CreateRef() : NULL ); return *this; }
/// Assigns the refcounted object to the container
template <class T>
SHVRefObjectContainer<T>& SHVRefObjectContainer<T>::operator =(T* obj)
	{ if (obj == Obj) return *this; if (Obj) Obj->DestroyRef(); Obj = ( obj ? (T *) obj->CreateRef() : NULL); return *this; }



/*************************************
 * ReleaseReference
 *************************************/
/// Releases the reference to the object
/**
 \return Pointer to refcounting object
 *
 * Use this if you want to return the pointer from a container without deleting it
\code
typedef SHVRefObjectTemplate<int> SHVIntObj;
typedef SHVRefObjectContainer<SHVIntObj> SHVIntRef;

SHVIntObj* CreateInt()
{
SHVIntRef i = new SHVIntObj;

	i->Object() = 42;

	return i.ReleaseReference(); // the correct way to release it
}
\endcode
 */
template <class T>
T* SHVRefObjectContainer<T>::ReleaseReference() { T* retVal = Obj; if (Obj) Obj->ReleaseRef(); Obj = NULL; return retVal; }
#endif
