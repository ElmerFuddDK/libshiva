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



//=========================================================================================================
// SHVConfigImpl class - implementation of config
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVConfigImpl::SHVConfigImpl()
{
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
 * Set(string)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVStringC& name, const SHVStringC& val)
{
SHVConfigNodeImplString* retVal = new SHVConfigNodeImplString(val);

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * Set(int)
 *************************************/
SHVConfigNode& SHVConfigImpl::Set(const SHVStringC& name, SHVInt val)
{
SHVConfigNodeImplInt* retVal = new SHVConfigNodeImplInt(val);

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * SetPtr
 *************************************/
SHVConfigNode& SHVConfigImpl::SetPtr(const SHVStringC& name, void* val)
{
SHVConfigNodeImplPointer* retVal = new SHVConfigNodeImplPointer(val);

	StringEntries[name] = retVal;
	return *retVal;
}

/*************************************
 * SetRef
 *************************************/
SHVConfigNode& SHVConfigImpl::SetRef(const SHVStringC& name, SHVRefObject* val)
{
SHVConfigNodeImplRef* retVal = new SHVConfigNodeImplRef(val);

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
SHVInt SHVConfigNodeImplString::ToInt()	{ return ( !Value.IsNull() ? SHVInt(Value.ToLong()) : SHVInt() ); }
SHVInt SHVConfigNodeImplPointer::ToInt(){ return ( Value != NULL ? SHVInt(*(int*)&Value) : SHVInt() ); }
SHVInt SHVConfigNodeImplRef::ToInt()	{ return ( !Value.IsNull() ? 1 : 0 ); }

/*************************************
 * ToDouble
 *************************************/
SHVDouble SHVConfigNodeImplInt::ToDouble()	{ return (double)Value; }
SHVDouble SHVConfigNodeImplString::ToDouble()	{ return ( !Value.IsNull() ? SHVDouble((double)Value.ToLong()) : SHVDouble() ); }
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
