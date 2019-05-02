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

#include "../../../include/utils/shvuuid.h"
#include "../../../include/utils/shvmd5sum.h"
#include "../../../include/utils/shvbuffer.h"

extern "C"
{
#include "../uuid/sysdep.h"
#include "../uuid/uuid.h"
#include "../uuid/globals.h"
}


//=========================================================================================================
// SHVUUID class
//=========================================================================================================
/// \class SHVUUID shvuuid.h "shiva/include/utils/shvuuid.h"

/*************************************
 * Constructor
 *************************************/
SHVUUID::SHVUUID()
{
	Globals = init_globals();

	// debug code check for size of uuid_t being equal to our fake type
	SHVASSERT(sizeof(SHVUUID::ID) == sizeof(shv_uuid_t));
}

/*************************************
 * Destructor
 *************************************/
SHVUUID::~SHVUUID()
{
	::free(Globals);
}

/*************************************
 * SetNodeData
 *************************************/
void SHVUUID::SetNodeData()
{
uuid_globals_t* glob = (uuid_globals_t*)Globals;
uuid_node_t tmpNode;

	glob->get_ieee_node_identifier_inited = 0;
	get_ieee_node_identifier(Globals,&tmpNode);
}

/*************************************
 * SetNodeData
 *************************************/
void SHVUUID::SetNodeData(void* data, size_t sz)
{
	set_ieee_node_identifier(Globals,data,sz);
}

/*************************************
 * SetNodeDataAsMd5
 *************************************/
void SHVUUID::SetNodeDataAsMd5(void* data, size_t sz)
{
SHVMd5Sum md5;
SHVBufferCRef md5data = new SHVBufferCPtr((SHVByte*)data,sz);
SHVBufferRef buffer;

	md5.Calculate(md5data);
	buffer = md5.GetMd5AsBinary();

	SetNodeData(buffer->GetBuffer(),SHVMd5Sum::Md5BinaryLen);
}

/*************************************
 * Null
 *************************************/
SHVUUID::ID SHVUUID::Null()
{
SHVUUID::ID retVal;
	::memset(&retVal,0,sizeof(SHVUUID::ID));
	return retVal;
}

/*************************************
 * Create
 *************************************/
SHVUUID::ID SHVUUID::Create()
{
SHVUUID::ID retVal;
	uuid_create(Globals,(shv_uuid_t*)&retVal);
	return retVal;
}

/*************************************
 * CreateMd5FromNamespace8
 *************************************/
SHVUUID::ID SHVUUID::CreateMd5FromNamespace8(const SHVUUID::ID& namespc, const SHVString8C name)
{
SHVUUID::ID retVal;
shv_uuid_t n;
	::memcpy(&n,&namespc,sizeof(shv_uuid_t));
	uuid_create_md5_from_name(Globals,(shv_uuid_t*)&retVal,n, (void*)name.GetSafeBuffer(), (int)name.GetLengthInBytes());
	return retVal;
}

/*************************************
 * CreateMd5FromNamespaceUTF8
 *************************************/
SHVUUID::ID SHVUUID::CreateMd5FromNamespaceUTF8(const SHVUUID::ID& namespc, const SHVStringUTF8C name)
{
SHVUUID::ID retVal;
shv_uuid_t n;
	::memcpy(&n,&namespc,sizeof(shv_uuid_t));
	uuid_create_md5_from_name(Globals,(shv_uuid_t*)&retVal,n, (void*)name.GetSafeBuffer(), (int)name.GetLengthInBytes());
	return retVal;
}

/*************************************
 * CreateMd5FromNamespace16
 *************************************/
SHVUUID::ID SHVUUID::CreateMd5FromNamespace16(const SHVUUID::ID& namespc, const SHVString16C name)
{
SHVUUID::ID retVal;
shv_uuid_t n;
	::memcpy(&n,&namespc,sizeof(shv_uuid_t));
	uuid_create_md5_from_name(Globals,(shv_uuid_t*)&retVal,n, (void*)name.GetSafeBuffer(), (int)name.GetLengthInBytes());
	return retVal;
}

/*************************************
 * FromString8
 *************************************/
SHVUUID::ID SHVUUID::FromString8(const SHVString8C uuid)
{
SHVUUID::ID retVal(Null());
shv_uuid_t* id = (shv_uuid_t*)&retVal.Bytes;
size_t count = uuid.GetLength();
bool ok = ( count == 36);
const SHVChar* str = uuid.GetSafeBuffer();
	
	for (size_t i=0; ok && i<count; i++)
	{
		if (i==8 || i==13 || i==18 || i==23)
		{
			ok = (str[i] == '-');
		}
		else if (i<8)
		{
			id->time_low <<= 4;
			id->time_low += CharToInt(ok,str[i]);
		}
		else if (i<13)
		{
			id->time_mid <<= 4;
			id->time_mid += CharToInt(ok,str[i]);
		}
		else if (i<18)
		{
			id->time_hi_and_version <<= 4;
			id->time_hi_and_version += CharToInt(ok,str[i]);
		}
		else if (i<21)
		{
			id->clock_seq_hi_and_reserved <<= 4;
			id->clock_seq_hi_and_reserved += CharToInt(ok,str[i]);
		}
		else if (i<23)
		{
			id->clock_seq_low <<= 4;
			id->clock_seq_low += CharToInt(ok,str[i]);
		}
		else
		{
		size_t idx = (i-24)/2;
			id->node[idx] <<= 4;
			id->node[idx] += CharToInt(ok,str[i]);
		}

	}

	return (ok ? retVal : Null());
}

/*************************************
 * FromStringUTF8
 *************************************/
SHVUUID::ID SHVUUID::FromStringUTF8(const SHVStringUTF8C uuid)
{
	return FromString8(SHVString8C(uuid.GetBufferConst())); // lazy mans solution
}

/*************************************
 * FromString16
 *************************************/
SHVUUID::ID SHVUUID::FromString16(const SHVString16C uuid)
{
	return FromString8(uuid.ToStr8()); // lazy mans solution
}

///\cond INTERNAL
/*************************************
 * CharToInt
 *************************************/
int SHVUUID::CharToInt(bool& ok, const char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	else if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	else if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	else
		ok = false;

	return 0;
}
///\endcond


//=========================================================================================================
// SHVUUID::ID struct
//=========================================================================================================
/// \struct SHVUUID::ID shvuuid.h "shiva/include/utils/shvuuid.h"

/*************************************
 * Operators
 *************************************/
SHVUUID::ID::operator SHVHashValue() const
{
size_t retVal = 0;
	for(size_t i = 0; i < 16; i++)
		retVal = Bytes[i] + retVal * 5;
	return retVal;
}
bool SHVUUID::ID::operator==(const SHVUUID::ID& id) const
{
	return memcmp(Bytes,id.Bytes,16) == 0;
}

/*************************************
 * ToString8
 *************************************/
SHVStringBuffer8 SHVUUID::ID::ToString8()
{
shv_uuid_t* id = (shv_uuid_t*)&Bytes;
	return SHVString8C::Format("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
								id->time_low,
								id->time_mid,
								id->time_hi_and_version,
								id->clock_seq_hi_and_reserved,
								id->clock_seq_low,
								id->node[0],
								id->node[1],
								id->node[2],
								id->node[3],
								id->node[4],
								id->node[5]);
}

/*************************************
 * ToStringUTF8
 *************************************/
SHVStringBufferUTF8 SHVUUID::ID::ToStringUTF8()
{
shv_uuid_t* id = (shv_uuid_t*)&Bytes;
	return SHVStringUTF8C::Format("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
								id->time_low,
								id->time_mid,
								id->time_hi_and_version,
								id->clock_seq_hi_and_reserved,
								id->clock_seq_low,
								id->node[0],
								id->node[1],
								id->node[2],
								id->node[3],
								id->node[4],
								id->node[5]);
}

/*************************************
 * ToString16
 *************************************/
SHVStringBuffer16 SHVUUID::ID::ToString16()
{
#ifdef __SHIVA_POSIX
	return ToString8().ToStr16();
#else
shv_uuid_t* id = (shv_uuid_t*)&Bytes;
	return SHVString16C::Format((const SHVWChar*) L"%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
								id->time_low,
								id->time_mid,
								id->time_hi_and_version,
								id->clock_seq_hi_and_reserved,
								id->clock_seq_low,
								id->node[0],
								id->node[1],
								id->node[2],
								id->node[3],
								id->node[4],
								id->node[5]);
#endif
}
