#ifndef __SHIVA_TYPES
#define __SHIVA_TYPES

#include "utils/shvhashvalue.h"

typedef char SHVByte;


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
	if (!(val = TestString(nullstr))
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
	inline SHVInt& operator =(const int* val);
	inline bool operator==(const SHVInt& val) const;

private:
	///\cond INTERNAL
	int Val;
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
	inline SHVDouble& operator =(const double* val);
	inline bool operator==(const SHVDouble& val) const;

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

/*************************************
 * Properties
 *************************************/
int  SHVBool::GetError() const { return Val; }
void SHVBool::SetError(int err) { Val = err; }

bool SHVInt::IsNull() const { return Null; }
void SHVInt::SetToNull() { Null = true; Val = -1; }

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
SHVInt& SHVInt::operator =(const int* val) { if (val) { Null = false; Val = *val; } else SetToNull(); return *this; }
bool SHVInt::operator==(const SHVInt& val) const { return ( (val.Null && Null) || ( val.Null == Null && val.Val == Val ) ); }

SHVDouble::operator double() { return Val; }
SHVDouble::operator SHVHashValue() const { unsigned char* ptr = (unsigned char*)this; size_t retVal = 0; for (size_t i=0; i<sizeof(SHVDouble); i++,ptr++) retVal += *ptr; return retVal; }
SHVDouble& SHVDouble::operator =(const SHVDouble& val) { Null = val.Null; Val = val.Val; return *this; }
SHVDouble& SHVDouble::operator =(const double* val) { if (val) { Null = false; Val = *val; } else SetToNull(); return *this; }
bool SHVDouble::operator==(const SHVDouble& val) const { return ( (val.Null && Null) || ( val.Null == Null && val.Val == Val ) ); }

#endif
