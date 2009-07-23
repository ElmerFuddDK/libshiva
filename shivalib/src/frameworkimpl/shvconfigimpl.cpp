/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "../../../include/frameworkimpl/shvconfigimpl.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvstringutf8.h"
#include "../../../include/utils/shvfile.h"
#include "../../../include/utils/shvdir.h"
#include "../../../include/utils/shvmath.h"



//=========================================================================================================
// SHVConfigImpl class - implementation of config
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVConfigImpl::SHVConfigImpl()
{
	///\todo It may very well be nessecary to add thread safety to config
}

/*************************************
 * Find
 *************************************/
SHVConfigNode& SHVConfigImpl::Find(const SHVStringC& name, const SHVStringC& defVal)
{
	if (StringEntries.Find(name))
		return *StringEntries[name];
	else
		return Set(name,defVal);
}

/*************************************
 * FindInt
 *************************************/
SHVConfigNode& SHVConfigImpl::FindInt(const SHVStringC& name, SHVInt defVal)
{
	if (StringEntries.Find(name))
		return *StringEntries[name];
	else
		return Set(name,defVal);
}

/*************************************
 * FindPtr
 *************************************/
SHVConfigNode& SHVConfigImpl::FindPtr(const SHVStringC& name, void* defVal)
{
	if (StringEntries.Find(name))
		return *StringEntries[name];
	else
		return SetPtr(name,defVal);
}

/*************************************
 * FindRef
 *************************************/
SHVConfigNode& SHVConfigImpl::FindRef(const SHVStringC& name, SHVRefObject* defVal)
{
	if (StringEntries.Find(name))
		return *StringEntries[name];
	else
		return SetRef(name,defVal);
}

/*************************************
 * Contains
 *************************************/
SHVBool SHVConfigImpl::Contains(const SHVStringC& name)
{
	if (StringEntries.Find(name))
		return SHVBool::True;
	return SHVBool::False;
}

/*************************************
 * Get
 *************************************/
SHVBool SHVConfigImpl::Get(const SHVStringC& name, SHVString& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = Find(name).ToString();
	return retVal;
}

/*************************************
 * GetInt
 *************************************/
SHVBool SHVConfigImpl::GetInt(const SHVStringC& name, SHVInt& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindInt(name).ToInt();
	return retVal;
}

/*************************************
 * GetPtr
 *************************************/
SHVBool SHVConfigImpl::GetPtr(const SHVStringC& name, void*& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindPtr(name).ToPtr();
	return retVal;
}

/*************************************
 * GetRef
 *************************************/
SHVBool SHVConfigImpl::GetRef(const SHVStringC& name, SHVRefObject*& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindRef(name).ToRef();
	return retVal;
}

/*************************************
 * Set(string)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVStringC& name, const SHVStringC& val)
{
SHVConfigNodeImplString* retVal = new SHVConfigNodeImplString(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * Set(int)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVStringC& name, SHVInt val)
{
SHVConfigNodeImplInt* retVal = new SHVConfigNodeImplInt(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * SetPtr
 *************************************/
SHVConfigNode& SHVConfigImpl::SetPtr(const SHVStringC& name, void* val)
{
SHVConfigNodeImplPointer* retVal = new SHVConfigNodeImplPointer(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * SetRef
 *************************************/
SHVConfigNode& SHVConfigImpl::SetRef(const SHVStringC& name, SHVRefObject* val)
{
SHVConfigNodeImplRef* retVal = new SHVConfigNodeImplRef(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * Remove
 *************************************/
void SHVConfigImpl::Remove(const SHVStringC& name)
{
	StringEntries.Remove(name);
}

/*************************************
 * LookupValue
 *************************************/
bool SHVConfigImpl::LookupValue(const SHVStringC token, double& result)
{
bool retVal = false;
	
	if (Contains(token))
	{
	SHVDouble val(Find(token).ToDouble());
		if (!val.IsNull())
		{
			retVal = true;
			result = val;
		}
	}
	
	return retVal;
}

/*************************************
 * Load
 *************************************/
SHVBool SHVConfigImpl::Load(const SHVStringC fileName)
{
SHVBool retVal(SHVBool::True);
SHVFile file;

	if (fileName.IsNull())
		retVal = !FileName.IsNull();
	else
		FileName = fileName;

	if (retVal && SHVDir::FileExist(FileName) && file.Open(FileName,SHVFileBase::FlagOpen|SHVFileBase::FlagRead))
	{
	SHVString8 line;
	SHVString8 name;
	SHVString8 strValue;
	SHVInt intValue;
	int eqPos;
	SHVChar* endch;
	const SHVChar utf8Header[] = { (const char)0xEF, (const char)0xBB, (const char)0xBF, (const char)0 };
	SHVChar header[4];

		file.Read(header,3);
		header[3] = 0;

		if (SHVString8C(utf8Header) != SHVString8C(header))
			file.SetPosition(0);

		while (file.ReadLine8(line))
		{
			eqPos = line.Find("=");

			if (eqPos > 0)
			{
				// obtain name
				name = line.Left((size_t)eqPos);
				name.Trim();
				name.MakeLower();

				if (name == "")
				{
					; // do nothing
				}
				else if (name.Left(1) != "#" && name.Left(2) != "//") // not a comment
				{
					// move past '='
					eqPos++;
					strValue = line.Mid((size_t)eqPos);
					strValue.Trim();

					intValue = strValue.ToLong(&endch);

					if (strValue == "null" || strValue.IsEmpty())
					{
						Set(name.ToStrT(),SHVInt());
					}
					else if (*endch == 0)
					{
						Set(name.ToStrT(),intValue);
					}
					else if (strValue.Left(1) == "\"" && strValue.Right(1) == "\"")
					{
						Set(name.ToStrT(),SHVStringUTF8C(strValue.Mid(1,strValue.GetLength()-2).GetSafeBuffer()).ToStrT());
					}
					else
					{
						Set(name.ToStrT(),SHVStringUTF8C(strValue.GetSafeBuffer()).ToStrT());
					}
				}
			}
		}
	}

	return retVal;
}

/*************************************
 * Save
 *************************************/
SHVBool SHVConfigImpl::Save(const SHVStringC newFileName)
{
SHVBool retVal(SHVBool::True);
SHVFile file;

	if (newFileName.IsNull())
		retVal = !FileName.IsNull();
	else
		FileName = newFileName;

	if (retVal && file.Open(FileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite|SHVFileBase::FlagOverride|SHVFileBase::FlagOpen))
	{
	SHVHashTableStringIterator<SHVConfigNodeImplPtr> itr(StringEntries);
	SHVString line;
	const SHVChar utf8Header[] = { (const char)0xEF, (const char)0xBB, (const char)0xBF, (const char)0 };

		file.Write(utf8Header,3);

		while (itr.MoveNext() && retVal)
		{
			line = itr.GetData()->GetStorageString(itr.GetKey());
			if (!line.IsNull())
				retVal = file.WriteLine8(line.ToStrUTF8().GetSafeBuffer());
		}
	}

	return retVal;
}

/*************************************
 * Find
 *************************************/
SHVConfigNode& SHVConfigImpl::Find(const SHVInt& enumerator, const SHVStringC& defVal)
{
	if (EnumEntries.Find(enumerator))
		return *EnumEntries[enumerator];
	else
		return Set(enumerator,defVal);
}

/*************************************
 * FindInt
 *************************************/
SHVConfigNode& SHVConfigImpl::FindInt(const SHVInt& enumerator, SHVInt defVal)
{
	if (EnumEntries.Find(enumerator))
		return *EnumEntries[enumerator];
	else
		return Set(enumerator,defVal);
}

/*************************************
 * FindPtr
 *************************************/
SHVConfigNode& SHVConfigImpl::FindPtr(const SHVInt& enumerator, void* defVal)
{
	if (EnumEntries.Find(enumerator))
		return *EnumEntries[enumerator];
	else
		return SetPtr(enumerator,defVal);
}

/*************************************
 * FindRef
 *************************************/
SHVConfigNode& SHVConfigImpl::FindRef(const SHVInt& enumerator, SHVRefObject* defVal)
{
	if (EnumEntries.Find(enumerator))
		return *EnumEntries[enumerator];
	else
		return SetRef(enumerator,defVal);
}


/*************************************
 * Set(string)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVInt& enumerator, const SHVStringC& val)
{
SHVConfigNodeImplString* retVal = new SHVConfigNodeImplString(val);

	EnumEntries[enumerator] = retVal;
	return *retVal;
}

/*************************************
 * Contains
 *************************************/
SHVBool SHVConfigImpl::Contains(const SHVInt& enumerator)
{
	if (EnumEntries.Find(enumerator))
		return SHVBool::True;
	return SHVBool::False;
}

/*************************************
 * Get
 *************************************/
SHVBool SHVConfigImpl::Get(const SHVInt& enumerator, SHVString& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = Find(enumerator).ToString();
	return retVal;
}

/*************************************
 * GetInt
 *************************************/
SHVBool SHVConfigImpl::GetInt(const SHVInt& enumerator, SHVInt& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindInt(enumerator).ToInt();
	return retVal;
}

/*************************************
 * GetPtr
 *************************************/
SHVBool SHVConfigImpl::GetPtr(const SHVInt& enumerator, void*& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindPtr(enumerator).ToPtr();
	return retVal;
}

/*************************************
 * GetRef
 *************************************/
SHVBool SHVConfigImpl::GetRef(const SHVInt& enumerator, SHVRefObject*& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindRef(enumerator).ToRef();
	return retVal;
}

/*************************************
 * Set(int)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVInt& enumerator, SHVInt val)
{
SHVConfigNodeImplInt* retVal = new SHVConfigNodeImplInt(val);

	EnumEntries[enumerator] = retVal;
	return *retVal;
}

/*************************************
 * SetPtr
 *************************************/
SHVConfigNode& SHVConfigImpl::SetPtr(const SHVInt& enumerator, void* val)
{
SHVConfigNodeImplPointer* retVal = new SHVConfigNodeImplPointer(val);

	EnumEntries[enumerator] = retVal;
	return *retVal;
}

/*************************************
 * SetRef
 *************************************/
SHVConfigNode& SHVConfigImpl::SetRef(const SHVInt& enumerator, SHVRefObject* val)
{
SHVConfigNodeImplRef* retVal = new SHVConfigNodeImplRef(val);

	EnumEntries[enumerator] = retVal;
	return *retVal;
}

/*************************************
 * Remove
 *************************************/
void SHVConfigImpl::Remove(const SHVInt& enumerator)
{
	EnumEntries.Remove(enumerator);
}

/*************************************
 * Clear
 *************************************/
void SHVConfigImpl::Clear()
{
	StringEntries.Clear();
	EnumEntries.Clear();
}



///\cond INTERNAL

//=========================================================================================================
// SHVConfigNodeImpl classes
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVConfigNodeImpl::SHVConfigNodeImpl()
{}
SHVConfigNodeImplInt::SHVConfigNodeImplInt(SHVInt val) : SHVConfigNodeImpl(), Value(val)
{}
SHVConfigNodeImplString::SHVConfigNodeImplString(const SHVStringC& val) : SHVConfigNodeImpl(), Value(val)
{}
SHVConfigNodeImplPointer::SHVConfigNodeImplPointer(void* val) : SHVConfigNodeImpl(), Value(val)
{}
SHVConfigNodeImplRef::SHVConfigNodeImplRef(SHVRefObject* val) : SHVConfigNodeImpl(), Value(val)
{}

/*************************************
 * ToInt*
 *************************************/
SHVInt SHVConfigNodeImplInt::ToInt()	{ return Value; }
SHVInt SHVConfigNodeImplString::ToInt()
{
SHVTChar* endp = NULL;
SHVInt retVal(!Value.IsNull() ? SHVInt(Value.ToLong(&endp)) : SHVInt());
	if (endp != NULL && *endp)
	{
	SHVString err;
	double eval = SHVMath::Eval(Value,err);
		if (err.IsNull())
			retVal = (int)eval;
		else
			SHVASSERT(false); // failed converting the value completely
	}
	return retVal;
}
SHVInt SHVConfigNodeImplPointer::ToInt(){ return ( Value != NULL ? SHVInt(*(int*)&Value) : SHVInt() ); }
SHVInt SHVConfigNodeImplRef::ToInt()	{ return ( !Value.IsNull() ? 1 : 0 ); }

/*************************************
 * ToDouble
 *************************************/
SHVDouble SHVConfigNodeImplInt::ToDouble()	{ return (double)Value; }
SHVDouble SHVConfigNodeImplString::ToDouble()
{
SHVTChar* endp = NULL;
SHVDouble retVal(!Value.IsNull() ? SHVDouble(Value.ToDouble(&endp)) : SHVDouble());
	if (endp != NULL && *endp)
	{
	SHVString err;
	double eval = SHVMath::Eval(Value,err);
		if (err.IsNull())
			retVal = eval;
		else
			SHVASSERT(false); // failed converting the value completely
	}
	return retVal;
}
SHVDouble SHVConfigNodeImplPointer::ToDouble()	{ return ( Value != NULL ? SHVDouble((double)*(int*)&Value) : SHVDouble() ); }
SHVDouble SHVConfigNodeImplRef::ToDouble()	{ return ( !Value.IsNull() ? 1.0 : 0.0 ); }

/*************************************
 * ToString
 *************************************/
SHVStringBuffer SHVConfigNodeImplInt::ToString() { return ( !Value.IsNull() ? SHVStringC::LongToString(Value) : SHVString().ReleaseBuffer() ); }
SHVStringBuffer SHVConfigNodeImplString::ToString() { SHVString retVal(Value); return retVal.ReleaseBuffer(); }
SHVStringBuffer SHVConfigNodeImplPointer::ToString() {SHVString retVal; if (Value) retVal.Format(_T("%p"), Value); return retVal.ReleaseBuffer(); }
SHVStringBuffer SHVConfigNodeImplRef::ToString()	 {SHVString retVal; if (!Value.IsNull()) retVal.Format(_T("%p"), Value.AsConst()); return retVal.ReleaseBuffer(); }

/*************************************
 * ToPtr
 *************************************/
void* SHVConfigNodeImplInt::ToPtr() { return NULL; }
void* SHVConfigNodeImplString::ToPtr() { return NULL; }
void* SHVConfigNodeImplPointer::ToPtr() { return Value; }
void* SHVConfigNodeImplRef::ToPtr() { return NULL; }


/*************************************
 * ToRef
 *************************************/
SHVRefObject* SHVConfigNodeImplInt::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplString::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplPointer::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplRef::ToRef() { return Value; }


/*************************************
 * IsNull
 *************************************/
SHVBool SHVConfigNodeImplInt::IsNull() { return Value.IsNull(); }
SHVBool SHVConfigNodeImplString::IsNull() { return Value.IsNull(); }
SHVBool SHVConfigNodeImplPointer::IsNull() { return ( Value == NULL ? true : false ); }
SHVBool SHVConfigNodeImplRef::IsNull() { return Value.IsNull(); }


/*************************************
 * GetStorageString
 *************************************/
SHVStringBuffer SHVConfigNodeImplInt::GetStorageString(const SHVStringC& name)
{
SHVString retVal;

	if (!IsNull())
		retVal.Format( _T("%s = \"%d\""), name.GetBufferConst(), (int)Value);
	else
		retVal.Format( _T("%s"), name.GetBufferConst());

	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplString::GetStorageString(const SHVStringC& name)
{
SHVString retVal;

	if (!IsNull())
		retVal.Format( _T("%s = \"%s\""), name.GetBufferConst(), Value.GetBufferConst());
	else
		retVal.Format( _T("%s"), name.GetBufferConst());

	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplPointer::GetStorageString(const SHVStringC& name)
{
SHVString retVal;
	SHVUNUSED_PARAM(name);
	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplRef::GetStorageString(const SHVStringC& name)
{
SHVString retVal;
	SHVUNUSED_PARAM(name);
	return retVal.ReleaseBuffer();
}

///\endcond
