#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvwin32.h"

//=========================================================================================================
// Win32
//=========================================================================================================

/*************************************
 * MapFlags
 *************************************/
int Win32::MapFlags(int shivaflags)
{
int retVal = 0;

	if (shivaflags & SHVControl::FlagVisible)
		retVal |= WS_VISIBLE;

	if (shivaflags & SHVControl::FlagDisabled)
		retVal |= WS_DISABLED;

	return retVal;
}
