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
SHVConfigImpl::SHVConfigImpl(SHVMutex& lock) : Lock(lock)
{
}

/*************************************
 * Find
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Find(const SHVStringC name, const SHVStringC defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (StringEntries.Find(name))
		retVal = StringEntries[name];
	else
		retVal = Set(name,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindInt
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindInt(const SHVStringC name, SHVInt defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (StringEntries.Find(name))
		retVal = StringEntries[name];
	else
		retVal = Set(name,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindPtr
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindPtr(const SHVStringC name, void* defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (StringEntries.Find(name))
		retVal = StringEntries[name];
	else
		retVal = SetPtr(name,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindRef
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindRef(const SHVStringC name, SHVRefObject* defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (StringEntries.Find(name))
		retVal = StringEntries[name];
	else
		retVal = SetRef(name,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Contains
 *************************************/
SHVBool SHVConfigImpl::Contains(const SHVStringC name)
{
SHVBool retVal;
	
	Lock.Lock();
	retVal = (StringEntries.Find(name) ? SHVBool::True : SHVBool::False);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Get
 *************************************/
SHVBool SHVConfigImpl::Get(const SHVStringC name, SHVString& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = Find(name)->ToString();
	return retVal;
}

/*************************************
 * GetInt
 *************************************/
SHVBool SHVConfigImpl::GetInt(const SHVStringC name, SHVInt& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindInt(name)->ToInt();
	return retVal;
}

/*************************************
 * GetPtr
 *************************************/
SHVBool SHVConfigImpl::GetPtr(const SHVStringC name, void*& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindPtr(name)->ToPtr();
	return retVal;
}

/*************************************
 * GetRef
 *************************************/
SHVBool SHVConfigImpl::GetRef(const SHVStringC name, SHVRefObject*& value)
{
SHVBool retVal(Contains(name));
	if (retVal)
		value = FindRef(name)->ToRef();
	return retVal;
}

/*************************************
 * Set(string)
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Set(const SHVStringC name, const SHVStringC val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplString(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	Lock.Lock();
	StringEntries[name] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Set(int)
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Set(const SHVStringC name, SHVInt val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplInt(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	Lock.Lock();
	StringEntries[name] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * SetPtr
 *************************************/
SHVConfigNodeRef SHVConfigImpl::SetPtr(const SHVStringC name, void* val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplPointer(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	Lock.Lock();
	StringEntries[name] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * SetRef
 *************************************/
SHVConfigNodeRef SHVConfigImpl::SetRef(const SHVStringC name, SHVRefObject* val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplRefObj(val);

	SHVASSERT( SHVString(name).ToLower() == name );

	Lock.Lock();
	StringEntries[name] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Remove
 *************************************/
void SHVConfigImpl::Remove(const SHVStringC name)
{
	Lock.Lock();
	StringEntries.Remove(name);
	Lock.Unlock();
}

/*************************************
 * LookupValue
 *************************************/
bool SHVConfigImpl::LookupValue(const SHVStringC token, double& result)
{
bool retVal = false;
	
	Lock.Lock();
	if (Contains(token))
	{
	SHVDouble val(Find(token)->ToDouble());
		if (!val.IsNull())
		{
			retVal = true;
			result = val;
		}
	}
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Load
 *************************************/
SHVBool SHVConfigImpl::Load(const SHVStringC fileName)
{
SHVBool retVal(SHVBool::True);
SHVFile file;

	Lock.Lock();
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
	Lock.Unlock();

	return retVal;
}

/*************************************
 * Save
 *************************************/
SHVBool SHVConfigImpl::Save(const SHVStringC newFileName)
{
SHVBool retVal(SHVBool::True);
SHVFile file;

	Lock.Lock();
	if (newFileName.IsNull())
		retVal = !FileName.IsNull();
	else
		FileName = newFileName;

	if (retVal && file.Open(FileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite|SHVFileBase::FlagOverride|SHVFileBase::FlagOpen))
	{
	SHVHashTableStringIterator<SHVConfigNodeImplRef> itr(StringEntries);
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
	Lock.Unlock();

	return retVal;
}

/*************************************
 * Find
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Find(const SHVInt& enumerator, const SHVStringC defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (EnumEntries.Find(enumerator))
		retVal = EnumEntries[enumerator];
	else
		retVal = Set(enumerator,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindInt
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindInt(const SHVInt& enumerator, SHVInt defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (EnumEntries.Find(enumerator))
		retVal = EnumEntries[enumerator];
	else
		retVal = Set(enumerator,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindPtr
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindPtr(const SHVInt& enumerator, void* defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (EnumEntries.Find(enumerator))
		retVal = EnumEntries[enumerator];
	else
		retVal = SetPtr(enumerator,defVal);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * FindRef
 *************************************/
SHVConfigNodeRef SHVConfigImpl::FindRef(const SHVInt& enumerator, SHVRefObject* defVal)
{
SHVConfigNodeRef retVal;
	
	Lock.Lock();
	if (EnumEntries.Find(enumerator))
		retVal = EnumEntries[enumerator];
	else
		retVal = SetRef(enumerator,defVal);
	Lock.Unlock();
	
	return retVal;
}


/*************************************
 * Set(string)
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Set(const SHVInt& enumerator, const SHVStringC val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplString(val);

	Lock.Lock();
	EnumEntries[enumerator] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Contains
 *************************************/
SHVBool SHVConfigImpl::Contains(const SHVInt& enumerator)
{
SHVBool retVal;
	
	Lock.Lock();
	retVal = (EnumEntries.Find(enumerator) ? SHVBool::True : SHVBool::False);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Get
 *************************************/
SHVBool SHVConfigImpl::Get(const SHVInt& enumerator, SHVString& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = Find(enumerator)->ToString();
	return retVal;
}

/*************************************
 * GetInt
 *************************************/
SHVBool SHVConfigImpl::GetInt(const SHVInt& enumerator, SHVInt& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindInt(enumerator)->ToInt();
	return retVal;
}

/*************************************
 * GetPtr
 *************************************/
SHVBool SHVConfigImpl::GetPtr(const SHVInt& enumerator, void*& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindPtr(enumerator)->ToPtr();
	return retVal;
}

/*************************************
 * GetRef
 *************************************/
SHVBool SHVConfigImpl::GetRef(const SHVInt& enumerator, SHVRefObject*& value)
{
SHVBool retVal(Contains(enumerator));
	if (retVal)
		value = FindRef(enumerator)->ToRef();
	return retVal;
}

/*************************************
 * Set(int)
 *************************************/
SHVConfigNodeRef SHVConfigImpl::Set(const SHVInt& enumerator, SHVInt val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplInt(val);

	Lock.Lock();
	EnumEntries[enumerator] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * SetPtr
 *************************************/
SHVConfigNodeRef SHVConfigImpl::SetPtr(const SHVInt& enumerator, void* val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplPointer(val);

	Lock.Lock();
	EnumEntries[enumerator] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * SetRef
 *************************************/
SHVConfigNodeRef SHVConfigImpl::SetRef(const SHVInt& enumerator, SHVRefObject* val)
{
SHVConfigNodeImpl* entry;
SHVConfigNodeRef retVal = entry = new SHVConfigNodeImplRefObj(val);

	Lock.Lock();
	EnumEntries[enumerator] = entry;
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Remove
 *************************************/
void SHVConfigImpl::Remove(const SHVInt& enumerator)
{
	Lock.Lock();
	EnumEntries.Remove(enumerator);
	Lock.Unlock();
}

/*************************************
 * Clear
 *************************************/
void SHVConfigImpl::Clear()
{
	Lock.Lock();
	StringEntries.Clear();
	EnumEntries.Clear();
	Lock.Unlock();
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
SHVConfigNodeImplString::SHVConfigNodeImplString(const SHVStringC val) : SHVConfigNodeImpl(), Value(val)
{}
SHVConfigNodeImplPointer::SHVConfigNodeImplPointer(void* val) : SHVConfigNodeImpl(), Value(val)
{}
SHVConfigNodeImplRefObj::SHVConfigNodeImplRefObj(SHVRefObject* val) : SHVConfigNodeImpl(), Value(val)
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
SHVInt SHVConfigNodeImplRefObj::ToInt()	{ return ( !Value.IsNull() ? 1 : 0 ); }

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
SHVDouble SHVConfigNodeImplRefObj::ToDouble()	{ return ( !Value.IsNull() ? 1.0 : 0.0 ); }

/*************************************
 * ToString
 *************************************/
SHVStringBuffer SHVConfigNodeImplInt::ToString() { return ( !Value.IsNull() ? SHVStringC::LongToString(Value) : SHVString().ReleaseBuffer() ); }
SHVStringBuffer SHVConfigNodeImplString::ToString() { SHVString retVal(Value); return retVal.ReleaseBuffer(); }
SHVStringBuffer SHVConfigNodeImplPointer::ToString() {SHVString retVal; if (Value) retVal.Format(_T("%p"), Value); return retVal.ReleaseBuffer(); }
SHVStringBuffer SHVConfigNodeImplRefObj::ToString()	 {SHVString retVal; if (!Value.IsNull()) retVal.Format(_T("%p"), Value.AsConst()); return retVal.ReleaseBuffer(); }

/*************************************
 * ToPtr
 *************************************/
void* SHVConfigNodeImplInt::ToPtr() { return NULL; }
void* SHVConfigNodeImplString::ToPtr() { return NULL; }
void* SHVConfigNodeImplPointer::ToPtr() { return Value; }
void* SHVConfigNodeImplRefObj::ToPtr() { return NULL; }


/*************************************
 * ToRef
 *************************************/
SHVRefObject* SHVConfigNodeImplInt::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplString::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplPointer::ToRef() { return NULL; }
SHVRefObject* SHVConfigNodeImplRefObj::ToRef() { return Value; }


/*************************************
 * IsNull
 *************************************/
SHVBool SHVConfigNodeImplInt::IsNull() { return Value.IsNull(); }
SHVBool SHVConfigNodeImplString::IsNull() { return Value.IsNull(); }
SHVBool SHVConfigNodeImplPointer::IsNull() { return ( Value == NULL ? true : false ); }
SHVBool SHVConfigNodeImplRefObj::IsNull() { return Value.IsNull(); }


/*************************************
 * GetStorageString
 *************************************/
SHVStringBuffer SHVConfigNodeImplInt::GetStorageString(const SHVStringC name)
{
SHVString retVal;

	if (!IsNull())
		retVal.Format( _T("%s = \"%d\""), name.GetBufferConst(), (int)Value);
	else
		retVal.Format( _T("%s"), name.GetBufferConst());

	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplString::GetStorageString(const SHVStringC name)
{
SHVString retVal;

	if (!IsNull())
		retVal.Format( _T("%s = \"%s\""), name.GetBufferConst(), Value.GetBufferConst());
	else
		retVal.Format( _T("%s"), name.GetBufferConst());

	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplPointer::GetStorageString(const SHVStringC name)
{
SHVString retVal;
	SHVUNUSED_PARAM(name);
	return retVal.ReleaseBuffer();
}
SHVStringBuffer SHVConfigNodeImplRefObj::GetStorageString(const SHVStringC name)
{
SHVString retVal;
	SHVUNUSED_PARAM(name);
	return retVal.ReleaseBuffer();
}

///\endcond
