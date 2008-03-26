#ifndef __SHIVA_XMLSTREAM_TEST_H
#define __SHIVA_XMLSTREAM_TEST_H

#include "shvtestmodule.h"
#include "../../../../include/modules/xmlstream/shvxmlstream.h"
#include "../../../../include/threadutils/shvthread.h"
#include "../../../../include/utils/shvstreamin.h"
#include "../../../../include/utils/shvfile.h"


class SHVXmlStreamTester: public SHVTestModule
{
public:
	inline SHVXmlStreamTester(SHVModuleList& modules): SHVTestModule(modules, "XmlStreamTester") {}
protected:
	virtual void PerformTest(SHVTestResult* result);
	virtual SHVBool Register();
	virtual void PostRegister();
private:
	SHVTestResult* Result;
	SHVXmlStream* XmlStream;
	void StartElem(SHVXmlReader& reader);
	void EndElem(SHVXmlReader& reader);
	void CharData(SHVXmlReader& reader);
};


class SHVStreamInTest: public SHVStreamIn
{
public:
	SHVStreamInTest(const SHVStringC& fileName);
	virtual bool Eof() const;
	virtual SHVBool IsOk() const;
	virtual SHVBool ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength);

	virtual SHVBool ReadString16(SHVWChar* buffer, size_t maxlen);
	virtual SHVWChar ReadChar16();

	virtual SHVBool ReadString8(SHVChar* buffer, size_t maxlen);
	virtual SHVChar ReadChar8();
	virtual void Close();
private:
	SHVFile File;
	SHVBool Ok;
	bool IsEof;
};

class SHVStreamOutTest: public SHVStreamOut
{
public:
	SHVStreamOutTest(const SHVStringC& fileName);
	virtual ~SHVStreamOutTest();
	virtual SHVBool IsOk() const;
	virtual SHVBool WriteBuffer(const void* buffer, size_t len);

	virtual SHVBool WriteString16(const SHVWChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar16(SHVWChar ch);

	virtual SHVBool WriteString8(const SHVChar* buffer, size_t maxlen = SIZE_T_MAX);
	virtual SHVBool WriteChar8(const SHVChar ch);
	virtual void Close();
private:
	SHVFile File;
	SHVBool Ok;
};

#endif