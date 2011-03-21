#ifndef __SHIVA_JSONSTREAM_H
#define __SHIVA_JSONSTREAM_H

#include "../../../include/framework/shvmodule.h"
#include "shvjsonreader.h"
#include "shvjsonwriter.h"

#define __JSONSTREAM_VERSION_MAJOR   0
#define __JSONSTREAM_VERSION_MINOR   1
#define __JSONSTREAM_VERSION_RELEASE 0

//-=========================================================================================================
/// SHVJsonStream class - Interface for the shiva json streaming module
/**
 * Factory for the SHVJsonReader & SHVJsonWriter objects
 */
class SHVJsonStream: public SHVModule
{
public:
	virtual SHVJsonReader* CreateReader() = 0;
	virtual SHVJsonWriter* CreateWriter() = 0;

protected:
	SHVJsonStream(SHVModuleList& modules): SHVModule(modules, "JsonStream") {}
};


#endif // __SHIVA_JSONSTREAM_H
