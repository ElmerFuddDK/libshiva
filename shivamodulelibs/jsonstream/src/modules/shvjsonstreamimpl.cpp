#include "../../../include/platformspc.h"
#include "shvjsonstreamimpl.h"
#include "shvjsonreaderimpl.h"
#include "shvjsonwriterimpl.h"


//=========================================================================================================
// SHVJsonStreamImpl - Json stream module implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVJsonStreamImpl::SHVJsonStreamImpl(SHVModuleList &modules): SHVJsonStream(modules)
{
}

/*************************************
 * CreateReader
 *************************************/
SHVJsonReader* SHVJsonStreamImpl::CreateReader()
{
	return new SHVJsonReaderImpl();
}

/*************************************
 * CreateWriter
 *************************************/
SHVJsonWriter* SHVJsonStreamImpl::CreateWriter()
{
	return new SHVJsonWriterImpl();
}
