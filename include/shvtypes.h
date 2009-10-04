#ifndef __SHIVA_TYPES
#define __SHIVA_TYPES

#include "utils/shvhashvalue.h"

typedef char SHVByte;
#if defined(__SHIVA_WINCE) && (_WIN32_WCE < 500)
 typedef __int64 SHVInt64Val;
#else
 typedef long long SHVInt64Val;
#endif
#define SHVChar  char
#ifdef __SHIVA_WIN32
# define SHVWChar wchar_t
#else
# define SHVWChar short
#endif
#if defined(UNICODE)
# define SHVTChar SHVWChar
# define SHVForeignChar SHVChar
#else
# define SHVTChar SHVChar 
# define SHVForeignChar SHVWChar
#endif



//-=========================================================================================================
/// SHVBool bool replacement
//-=========================================================================================================
/**
 * Use this class for functions that can fail with complex reasons. It is also encouraged to use it in virtual
 * functions since returning bool from a virtual has issues on some platforms.\n
 * The idea with this bool is you can use it as a normal bool if you don't care about the reason of failure,
 * and still use it to check for an error reason. An example:
\code
#define ErrNull 1
#define ErrEmpty 2

SHVBool TestString(const SHVChar* str)
{
	if (!str)
		return ErrNull;
	if (str[0] == '\0')
		return ErrEmpty;

	return true;
}

void main()
{
const SHVChar* nullstr = NULL;
const SHVChar* str = "ok string\n";
SHVBool val;

	if (TestString(str)) // we only care if this is a valid string
		printf(str);

	// here we care about the error type
	if (!(val = TestString(nullstr)))
	{
		switch (val.GetError())
		{
		case ErrNull:  fprintf(stderr, "String is null\n");  break;
		case ErrEmpty: fprintf(stderr, "String is empty\n"); break;
		}
	}
	else
		printf(nullstr);
}
\endcode
 */

class SHVAPI SHVBool
{
public:

	// defines
	enum BoolVal { True = 0, False = -1 };


	// constructors
	inline SHVBool(bool val);
	inline SHVBool(int err);
	inline SHVBool();


	// Error property
	inline int  GetError() const;
	inline void SetError(int err);


	// operators
	inline operator bool () const;
	inline operator bool ();

	inline SHVBool& operator=(BoolVal val);
	inline SHVBool& operator=(bool val);
	inline bool operator==(const SHVBool& val) const;
	inline bool operator==(const int& err) const;
	inline bool operator!() const;


private:
	///\cond INTERNAL
	int Val;
	///\endcond
};



//-=========================================================================================================
/// SHVInt int replacement
//-=========================================================================================================
/**
 * Nullable integer with a hashing function. This is useful where integers are used in hash tables, or
 * places where nullable objects are used.
 */

class SHVAPI SHVInt
{
public:


	// constructors
	inline SHVInt();
	inline SHVInt(int val);


	// Properties
	inline bool IsNull() const;
	inline void SetToNull();


	// operators
	inline operator int();
	inline operator SHVHashValue() const;
	inline SHVInt& operator =(const SHVInt& val);
	inline bool operator==(const SHVInt& val) const;


	// statics
	inline static SHVInt FromIntPtr(const int* val);


private:
	///\cond INTERNAL
	int Val;
	bool Null;
	///\endcond
};



//-=========================================================================================================
/// SHVInt64 int64 replacement
//-=========================================================================================================
/**
 * Nullable integer with a hashing function. This is useful where integers are used in hash tables, or
 * places where nullable objects are used.
 */

class SHVAPI SHVInt64
{
public:


	// constructors
	inline SHVInt64();
	inline SHVInt64(SHVInt64Val val);


	// Properties
	inline bool IsNull() const;
	inline void SetToNull();


	// operators
	inline operator SHVInt64Val();
	inline operator SHVHashValue() const;
	inline SHVInt64& operator =(const SHVInt64& val);
	inline bool operator==(const SHVInt64& val) const;


	// statics
	inline static SHVInt64 FromInt64Ptr(const SHVInt64Val* val);


private:
	///\cond INTERNAL
	SHVInt64Val Val;
	bool Null;
	///\endcond
};



//-=========================================================================================================
/// SHVDouble double replacement
//-=========================================================================================================
/**
 * Nullable double with a hashing function. This is useful where doubles are used in hash tables, or
 * places where nullable objects are used.
 */

class SHVAPI SHVDouble
{
public:


	// constructors
	inline SHVDouble();
	inline SHVDouble(double val);


	// Properties
	inline bool IsNull() const;
	inline void SetToNull();


	// operators
	inline operator double();
	inline operator SHVHashValue() const;
	inline SHVDouble& operator =(const SHVDouble& val);
	inline bool operator==(const SHVDouble& val) const;


	// statics
	inline static SHVDouble FromDoublePtr(const double* val);


private:
	///\cond INTERNAL
	double Val;
	bool Null;
	///\endcond

};



// ============================================ implementation ============================================ //

/*************************************
 * Constructors
 *************************************/
SHVBool::SHVBool(const bool val) { Val = ( val ? True : False ); }
SHVBool::SHVBool(const int err) { Val = err; }
SHVBool::SHVBool() { Val = False; }

SHVInt::SHVInt() { SetToNull(); }
SHVInt::SHVInt(int val) : Val(val), Null(false) {}

SHVInt64::SHVInt64() { SetToNull(); }
SHVInt64::SHVInt64(SHVInt64Val val) : Val(val), Null(false) {}

/*************************************
 * Properties
 *************************************/
int  SHVBool::GetError() const { return Val; }
void SHVBool::SetError(int err) { Val = err; }

bool SHVInt::IsNull() const { return Null; }
void SHVInt::SetToNull() { Null = true; Val = -1; }

bool SHVInt64::IsNull() const { return Null; }
void SHVInt64::SetToNull() { Null = true; Val = -1; }

SHVDouble::SHVDouble() { SetToNull(); }
SHVDouble::SHVDouble(double val) : Val(val), Null(false) {}

bool SHVDouble::IsNull() const { return Null; }
void SHVDouble::SetToNull() { Null = true; Val = 0.0; }

/*************************************
 * Operators
 *************************************/
SHVBool::operator bool () const { return Val == True; }
SHVBool::operator bool ()       { return Val == True; }

SHVBool& SHVBool::operator=(BoolVal val) { Val = val; return *this; }
SHVBool& SHVBool::operator=(bool val) { Val = ( val ? True : False ); return *this; }
bool SHVBool::operator==(const SHVBool& val) const { return Val == val.Val; }
bool SHVBool::operator==(const int& err) const { return Val == err; }
bool SHVBool::operator!() const { return Val != True; }

SHVInt::operator int() { return Val; }
SHVInt::operator SHVHashValue() const { return ( Null ? 0 : Val ); } // makes sure the null integer is smaller than whatever matches its hash key (zero)
SHVInt& SHVInt::operator =(const SHVInt& val) { Null = val.Null; Val = val.Val; return *this; }
bool SHVInt::operator==(const SHVInt& val) const { return ( (val.Null && Null) || ( val.Null == Null && val.Val == Val ) ); }

SHVInt SHVInt::FromIntPtr(const int* val) { if (val) return SHVInt(*val); return SHVInt(); }

SHVInt64::operator SHVInt64Val() { return Val; }
SHVInt64::operator SHVHashValue() const
{
size_t retVal = 0;
SHVByte* buf = (SHVByte*)&Val;
	for(size_t i = 0; !Null && i < sizeof(SHVInt64Val); i++)
		retVal = buf[i] + retVal * 5;
	return retVal;
}
SHVInt64& SHVInt64::operator =(const SHVInt64& val) { Null = val.Null; Val = val.Val; return *this; }
bool SHVInt64::operator==(const SHVInt64& val) const { return ( (val.Null && Null) || ( val.Null == Null && val.Val == Val ) ); }

SHVInt64 SHVInt64::FromInt64Ptr(const SHVInt64Val* val) { if (val) return SHVInt64(*val); return SHVInt64(); }

SHVDouble::operator double() { return Val; }
SHVDouble::operator SHVHashValue() const { unsigned char* ptr = (unsigned char*)this; size_t retVal = 0; for (size_t i=0; i<sizeof(SHVDouble); i++,ptr++) retVal += *ptr; return retVal; }
SHVDouble& SHVDouble::operator =(const SHVDouble& val) { Null = val.Null; Val = val.Val; return *this; }
bool SHVDouble::operator==(const SHVDouble& val) const { return ( (val.Null && Null) || ( val.Null == Null && val.Val == Val ) ); }

SHVDouble SHVDouble::FromDoublePtr(const double* val) { if (val) return SHVDouble(*val); return SHVDouble(); }

#endif
