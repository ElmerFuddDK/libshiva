#ifndef __SHIVA_JSONSTREAMIMPL_H
#define __SHIVA_JSONSTREAMIMPL_H

#include "shiva/include/modules/jsonstream/shvjsonstream.h"

//-=========================================================================================================
/// SHVJsonStreamImpl class - Implementation for the shiva json streaming module
/**
 * Factory for the SHVJsonReader & SHVJsonWriter objects
 */
class SHVJsonStreamImpl: public SHVJsonStream
{
public:
	SHVJsonStreamImpl(SHVModuleList& modules);
	virtual SHVJsonReader* CreateReader();
	virtual SHVJsonWriter* CreateWriter();
};

#endif // __SHIVA_JSONSTREAMIMPL_H
