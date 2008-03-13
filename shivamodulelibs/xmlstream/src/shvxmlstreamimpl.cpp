#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/xmlstreamimpl/shvxmlstream_impl.h"
#include "../include/xmlstreamimpl/shvxmlreader_impl.h"
#include "../include/xmlstreamimpl/shvxmlwriter_impl.h"

/*************************************
 * Constructor
 *************************************/
SHVXmlStream_impl::SHVXmlStream_impl(SHVModuleList& modules): SHVXmlStream(modules)
{
}

/*************************************
 * CreateReader
 *************************************/
SHVXmlReader* SHVXmlStream_impl::CreateReader(SHVXmlReader::ParserEncoding enc, size_t bufferSize)
{
	return new SHVXmlReader_Impl(enc, bufferSize);
}

/*************************************
 * CreateWriter
 *************************************/
SHVXmlWriter* SHVXmlStream_impl::CreateWriter(SHVXmlWriter::WriterEncoding enc)
{
	return new SHVXmlWriter_Impl(enc);
}
