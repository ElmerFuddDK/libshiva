#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "shvxmlstreamtester.h"


void SHVXmlStreamTester::PerformTest(SHVTestResult* result)
{
SHVBool ok;
	Result = result;
SHVString FilePath;
	FilePath = Modules.GetConfig().Find(_T("applicationpath")).ToString();
	FilePath += _T("test.xml");
SHVStreamOutTest testout(FilePath);
	if (testout.IsOk())
	{
	SHVXmlWriter* writer = XmlStream->CreateWriter(SHVXmlWriter::WriterEncodingUTF8);
		writer->WriteStartElementUTF8(testout, "persons");

		writer->WriteStartElementUTF8(testout, "person");
		writer->WriteAttributeUTF8(testout, "firstName", "Mog\tens\r\n");
		writer->WriteAttributeUTF8(testout, "middleName", "Bak");
		writer->WriteAttributeUTF8(testout, "lastName", "Nielsen");
		writer->WriteEndElement(testout);

		writer->WriteStartElementUTF8(testout, "person");
		writer->WriteAttributeUTF8(testout, "firstName", "Hanne");
		writer->WriteAttributeUTF8(testout, "middleName", "Birkemose");
		writer->WriteAttributeUTF8(testout, "lastName", "Nielsen");
		writer->WriteEndElement(testout);

		writer->WriteText(testout, _T("This is j\tust a test\r\n"));
		writer->WriteEndElement(testout);
		writer->WriteStartElementUTF8(testout, "persons");

		writer->WriteStartElementUTF8(testout, "person");
		writer->WriteAttributeUTF8(testout, "firstName", "Mog\tens\r\n");
		writer->WriteAttributeUTF8(testout, "middleName", "Bak");
		writer->WriteAttributeUTF8(testout, "lastName", "Nielsen");
		writer->WriteEndElement(testout);

		writer->WriteStartElementUTF8(testout, "person");
		writer->WriteAttributeUTF8(testout, "firstName", "Hanne");
		writer->WriteAttributeUTF8(testout, "middleName", "Birkemose");
		writer->WriteAttributeUTF8(testout, "lastName", "Nielsen");
		writer->WriteEndElement(testout);

		writer->WriteText(testout, _T("This is j\tust a test\r\n"));
		writer->WriteEndElement(testout);
		testout.Close();
	}
SHVStreamInTest test(FilePath);
	if (test.IsOk())
	{
	SHVXmlReader *reader = XmlStream->CreateReader(SHVXmlReader::ParserEncodingUTF8);
		reader->SetMultidocument(true);
		reader->SetStartElementCallBack(new SHVXmlReaderCallback<SHVXmlStreamTester>(this, &SHVXmlStreamTester::StartElem));
		reader->SetEndElementCallBack(new SHVXmlReaderCallback<SHVXmlStreamTester>(this, &SHVXmlStreamTester::EndElem));
		reader->SetValueCallback(new SHVXmlReaderCallback<SHVXmlStreamTester>(this, &SHVXmlStreamTester::CharData));
		ok = reader->Parse(test);
		delete reader;
	}
	else
		ok = SHVBool::False;
	*result = ok;
}


SHVBool SHVXmlStreamTester::Register()
{
	XmlStream = (SHVXmlStream*) Modules.ResolveModule("XmlStream");
	return XmlStream != NULL && SHVTestModule::Register();
}

void SHVXmlStreamTester::PostRegister()
{
}

void SHVXmlStreamTester::StartElem(SHVXmlReader& reader)
{
size_t attrCount = reader.CalculateAttributeCount();
	Result->AddLog(_T("Element <%s>"), reader.GetElementName().GetSafeBuffer());

	for (size_t i = 0; i < attrCount; i++)
	{
		Result->AddLog(_T("%s = %s"), reader.GetAttributeName(i).GetSafeBuffer(), reader.GetAttributeValue(i));
	}
}
void SHVXmlStreamTester::EndElem(SHVXmlReader& reader)
{
	Result->AddLog(_T("Element </%s>"), reader.GetElementName().GetSafeBuffer());
}

void SHVXmlStreamTester::CharData(SHVXmlReader& reader)
{
	Result->AddLog(_T("Char data %s"), reader.GetValue().GetSafeBuffer());
}

SHVStreamInTest::SHVStreamInTest(const SHVStringC& fileName)
{
	Ok = File.Open(fileName, SHVFile::FlagOpen|SHVFile::FlagRead);
	IsEof = false;
}

bool SHVStreamInTest::Eof() const
{
	return IsEof;
}

SHVBool SHVStreamInTest::IsOk() const
{
	return Ok;
}

SHVBool SHVStreamInTest::ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength)
{
	actualLength = File.Read(buffer, (SHVFilePos) bufferSize);
	IsEof = actualLength < bufferSize;
	return actualLength != 0;
}

SHVBool SHVStreamInTest::ReadString16(SHVWChar* buffer, size_t maxlen)
{
size_t i = 0;
	while (i < maxlen - 1 && (IsEof = File.Read((void *) (buffer+i), sizeof(SHVWChar)) < sizeof(SHVWChar)) && buffer[i] && buffer[i] != '\n')
	{
		if (buffer[i] != L'\r')
			i++;
	}
	buffer[i] = 0;
	Ok = !IsEof;
	return Ok;
}

SHVWChar SHVStreamInTest::ReadChar16()
{
SHVWChar ch = 0;
	IsEof = File.Read(&ch, sizeof(SHVWChar)) < sizeof(SHVWChar);
	return ch;
}

SHVBool SHVStreamInTest::ReadString8(SHVChar* buffer, size_t maxlen)
{
size_t i = 0;
	while (i < maxlen - 1 && (IsEof = File.Read((void *) (buffer+i), sizeof(SHVChar)) < sizeof(SHVWChar)) && buffer[i] && buffer[i] != '\n')
	{
		if (buffer[i] != L'\r')
			i++;
	}
	buffer[i] = 0;
	Ok = !IsEof;
	return Ok;
}

SHVChar SHVStreamInTest::ReadChar8()
{
SHVChar ch = 0;
	IsEof = File.Read(&ch, sizeof(SHVChar)) < sizeof(SHVChar);
	return ch;
}

void SHVStreamInTest::Close()
{
	File.Close();
}

SHVStreamOutTest::SHVStreamOutTest(const SHVStringC& fileName)
{
	Ok = File.Open(fileName, SHVFile::FlagOpen|SHVFile::FlagOverride|SHVFile::FlagWrite|SHVFile::FlagCreate);
}

SHVStreamOutTest::~SHVStreamOutTest()
{
	File.Close();
}

SHVBool SHVStreamOutTest::IsOk() const
{
	return Ok;
}

SHVBool SHVStreamOutTest::WriteBuffer(const void* buffer, size_t len)
{
	if (Ok)
	{
		File.Write(buffer, (SHVFilePos) len);
	}
	return Ok;
}

SHVBool SHVStreamOutTest::WriteString16(const SHVWChar* buffer, size_t maxlen)
{
	if (Ok)
	{
	size_t writeLen = SHVString::StrLen(buffer);
		if (maxlen < writeLen)
			writeLen = maxlen;
		Ok = File.Write(buffer, (SHVFilePos) writeLen*2);
	}
	return Ok;
}

SHVBool SHVStreamOutTest::WriteChar16(SHVWChar ch)
{
	if (Ok)
	{
		Ok = File.Write(&ch, sizeof(SHVWChar));
	}
	return Ok;
}

SHVBool SHVStreamOutTest::WriteString8(const SHVChar* buffer, size_t maxlen)
{
	if (Ok)
	{
	size_t writeLen = SHVString8::StrLen(buffer);
		if (maxlen < writeLen)
			writeLen = maxlen;
		Ok = File.Write(buffer, (SHVFilePos) writeLen);
	}
	return Ok;
}

SHVBool SHVStreamOutTest::WriteChar8(const SHVChar ch)
{
	if (Ok)
	{
		Ok = File.Write(&ch, sizeof(SHVChar));
	}
	return Ok;
}

void SHVStreamOutTest::Close()
{
	File.Close();
}
