#ifndef __SHIVA_XMLSTREAM_IMPL_H
#define __SHIVA_XMLSTREAM_IMPL_H

#include "../../../include/modules/xmlstream/shvxmlstream.h"

#define __XMLSTREEAM_VERSION_MAJOR     0
#define __XMLSTREEAM_VERSION_MINOR     1
#define __XMLSTREEAM_VERSION_RELEASE   0
#define __XMLSTREEAM_VERSION_BUILD     0

//-=========================================================================================================
/// SHVXmlStream class - Interface for the shiva xml streaming module
/**
 * Factory for the SHVXmlReader & SHVXmlWriter objects
 */

class SHVXmlStreamImpl: public SHVXmlStream
{
public:
	SHVXmlStreamImpl(SHVModuleList& modules);
	virtual SHVXmlReader* CreateReader(SHVXmlReader::ParserEncoding enc, size_t bufferSize = 1024);
	virtual SHVXmlWriter* CreateWriter(SHVXmlWriter::WriterEncoding enc);
};

#endif
