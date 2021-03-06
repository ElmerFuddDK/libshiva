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

#include "../../../include/utils/shvmd5sum.h"
#include "../../../include/utils/shvbuffer.h"

extern "C"
{
#include "../md5/md5.h"
}


//=========================================================================================================
// SHVMd5Sum class - calculates md5 sums
//=========================================================================================================
/// \class SHVMd5Sum shvmd5sum.h "shiva/include/utils/shvmd5sum.h"

/*************************************
 * Constructor
 *************************************/
SHVMd5Sum::SHVMd5Sum()
{
	Md5Context = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVMd5Sum::~SHVMd5Sum()
{
 	if (Md5Context)
		::free(Md5Context);
}

/*************************************
 * Initialize
 *************************************/
/// Initializes the md5 sum
void SHVMd5Sum::Initialize()
{
	if (!Md5Context)
		Md5Context = ::malloc(sizeof(MD5_CTX));

	::MD5Init((MD5_CTX*)Md5Context);
}

/*************************************
 * IsInitialized
 *************************************/
/// Returns true if initialized
bool SHVMd5Sum::IsInitialized()
{
	return (Md5Context ? true : false);
}

/*************************************
 * Calculate
 *************************************/
/// Partial calculation of md5 sum
/**
 \param buffer Buffer to calculate md5 data for
 *
 * Adds to the md5 sum calculation. Finalize with either GetMd5 or GetMd5AsBinary.
 */
void SHVMd5Sum::Calculate(const SHVBufferC* buffer)
{
	if (!IsInitialized())
		Initialize();
	::MD5Update( (MD5_CTX*)Md5Context, (unsigned char*)buffer->GetBufferConst(), (int)buffer->GetSize()); 
}

/*************************************
 * GetMd5
 *************************************/
/// Finalizes and creates the md5 sum
/**
 \return Returns the md5 sum as string
 *
 \note Will uninitialize the object. This means you can only call
       either this or SHVMd5Sum::GetMd5AsBinary once.
 */
SHVStringBuffer SHVMd5Sum::GetMd5()
{
SHVString retVal;

	if (IsInitialized())
	{
	const SHVTChar hexCiphers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'  };
	SHVByte md5[16];
	SHVByte* md5Ptr;
	int i, j;

		::MD5Final( (unsigned char*)md5, (MD5_CTX*)Md5Context );
		::free(Md5Context);
		Md5Context = NULL;

		retVal.SetBufferSize(33);
		md5Ptr = md5;
		for (i=j=0; i<16; i++, md5Ptr++)
		{
			retVal.GetBuffer()[j++] = hexCiphers[(md5[i]&0xF0) >> 4];
			retVal.GetBuffer()[j++] = hexCiphers[md5[i]&0x0F];
		}
		retVal.GetBuffer()[j] = 0;
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * GetMd5AsBinary
 *************************************/
/// Finalizes and creates the md5 sum as binary data
/**
 \return Returns a buffer with the md5 sum as binary
 *
 \note Will uninitialize the object. This means you can only call
       either this or SHVMd5Sum::GetMd5 once.
 */
SHVBuffer* SHVMd5Sum::GetMd5AsBinary()
{
SHVBuffer* retVal = NULL;

	if (IsInitialized())
	{

		retVal = new SHVBufferChunk<Md5BinaryLen>();

		::MD5Final( (unsigned char*)retVal->GetBuffer(), (MD5_CTX*)Md5Context );
		::free(Md5Context);
		Md5Context = NULL;
	}

	return retVal;
}
