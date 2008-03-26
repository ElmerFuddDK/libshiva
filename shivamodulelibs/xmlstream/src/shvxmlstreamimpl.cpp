#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvxmlstreamimpl.h"
#include "../include/shvxmlreaderimpl.h"
#include "../include/shvxmlwriterimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVXmlStreamImpl::SHVXmlStreamImpl(SHVModuleList& modules): SHVXmlStream(modules)
{
}

/*************************************
 * CreateReader
 *************************************/
SHVXmlReader* SHVXmlStreamImpl::CreateReader(SHVXmlReader::ParserEncoding enc, size_t bufferSize)
{
	return new SHVXmlReaderImpl(enc, bufferSize);
}

/*************************************
 * CreateWriter
 *************************************/
SHVXmlWriter* SHVXmlStreamImpl::CreateWriter(SHVXmlWriter::WriterEncoding enc)
{
	return new SHVXmlWriterImpl(enc);
}
