#ifndef __SHIVA_XMLSTREAM_H
#define __SHIVA_XMLSTREAM_H

#include "../../../include/framework/shvmodule.h"
#include "../../../include/utils/shvdll.h"
#include "shvxmlreader.h"
#include "shvxmlwriter.h"

#define __XMLSTREAM_VERSION_MAJOR     0
#define __XMLSTREAM_VERSION_MINOR     1
#define __XMLSTREAM_VERSION_RELEASE   0
#define __XMLSTREAM_VERSION_BUILD     0

//-=========================================================================================================
/// SHVXmlStream class - Interface for the shiva xml streaming module
/**
 * Factory for the SHVXmlReader & SHVXmlWriter objects
 */

class SHVXmlStream: public SHVModule
{
public:
	enum SHVXMLError {
	// Copy from Expat.. please syncronize when opgrading expat version
	  XML_ERROR_NONE,
	  XML_ERROR_NO_MEMORY,
	  XML_ERROR_SYNTAX,
	  XML_ERROR_NO_ELEMENTS,
	  XML_ERROR_INVALID_TOKEN,
	  XML_ERROR_UNCLOSED_TOKEN,
	  XML_ERROR_PARTIAL_CHAR,
	  XML_ERROR_TAG_MISMATCH,
	  XML_ERROR_DUPLICATE_ATTRIBUTE,
	  XML_ERROR_JUNK_AFTER_DOC_ELEMENT,
	  XML_ERROR_PARAM_ENTITY_REF,
	  XML_ERROR_UNDEFINED_ENTITY,
	  XML_ERROR_RECURSIVE_ENTITY_REF,
	  XML_ERROR_ASYNC_ENTITY,
	  XML_ERROR_BAD_CHAR_REF,
	  XML_ERROR_BINARY_ENTITY_REF,
	  XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF,
	  XML_ERROR_MISPLACED_XML_PI,
	  XML_ERROR_UNKNOWN_ENCODING,
	  XML_ERROR_INCORRECT_ENCODING,
	  XML_ERROR_UNCLOSED_CDATA_SECTION,
	  XML_ERROR_EXTERNAL_ENTITY_HANDLING,
	  XML_ERROR_NOT_STANDALONE,
	  XML_ERROR_UNEXPECTED_STATE,
	  XML_ERROR_ENTITY_DECLARED_IN_PE,
	  XML_ERROR_FEATURE_REQUIRES_XML_DTD,
	  XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING
	};
	virtual SHVXmlReader* CreateReader(SHVXmlReader::ParserEncoding enc, size_t bufferSize = 1024) = 0;
	virtual SHVXmlWriter* CreateWriter(SHVXmlWriter::WriterEncoding enc) = 0;
protected:
	inline SHVXmlStream(SHVModuleList& modules): SHVModule(modules, "XmlStream") { }
};

#endif