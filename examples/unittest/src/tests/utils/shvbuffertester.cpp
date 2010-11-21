#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvbuffertester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVBufferTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVBufferTester::GetID() const
{
	return "buffer";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVBufferTester::GetTitle() const
{
	return _S("SHVBuffer and friends test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVBufferTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionRead, "read", _S("Read"), _S("This will test Read"), &SHVBufferTester::TestRead },
	{ ActionWrite, "write", _S("Write"), _S("This will test Write"), &SHVBufferTester::TestWrite },
	{ ActionChunk, "chunk", _S("Chunk"), _S("This will test Chunk"), &SHVBufferTester::TestChunk },
	{ ActionStream, "stream", _S("Stream"), _S("This will test Stream"), &SHVBufferTester::TestStream },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVBufferTester::TestRead(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
const char bufArray[] = { 127, 0, 0, 0, 'h', 'e', 'y', 0, 3, 'y', 'o' , 'u', 'm', 0, 'o', 0, 'o' , 0  };
SHVBufferCPtr testBuffer(SHVBufferCPtr::VoidToBuffer(bufArray,18));

	ok = PerformRead(testBuffer);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVBufferTester::TestWrite(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVBufferPtr testBuffer;
SHVBufferIterator itr(testBuffer);

	testBuffer.SetBufferSize(4);

	while (itr && ok)
	{
		switch (itr.GetField())
		{
		case 0:
			ok = itr.WriteInt(127);
			testBuffer.Expand(6);
			break;
		case 1:
			ok = itr.WriteNullString8("hey");
			testBuffer.Expand(-1);
			break;
		case 2:
			ok = itr.WriteChar(3);
			testBuffer.Expand(3);
			break;
		case 3:
			ok = itr.WriteString8("you");
			testBuffer.Expand(6);
			break;
		case 4:
			ok = itr.WriteString16(_SHVS16("moo"));
			break;
		default:
			ok = itr.SetError();
		}
	}
	
	ok = (ok && testBuffer.GetSize() == 18); 
	ok = (ok && PerformRead(testBuffer)); 

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVBufferTester::TestChunk(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVBufferChunk<18> testBuffer;
SHVBufferIterator itr(testBuffer);

	while (itr && ok)
	{
		switch (itr.GetField())
		{
		case 0:
			ok = itr.WriteInt(127);
			break;
		case 1:
			ok = itr.WriteNullString8("hey");
			break;
		case 2:
			ok = itr.WriteChar(3);
			break;
		case 3:
			ok = itr.WriteString8("you");
			break;
		case 4:
			ok = itr.WriteString16(_SHVS16("moo"));
			break;
		default:
			ok = itr.SetError();
		}
	}
	
	ok = (ok && testBuffer.GetSize() == 18); 
	ok = (ok && PerformRead(testBuffer)); 

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVBufferTester::TestStream(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
const char bufArray1[] = { 127, 0 }; // 2 bytes
const char bufArray2[] = { 0, 0, 'h', 'e', 'y', 0, 3, 'y' }; // 8 bytes
const char bufArray3[] = { 'o' , 'u', 'm', 0, 'o', 0, 'o' , 0  }; // 8 bytes
SHVBufferCStream testBuffer;

	testBuffer.AddBuffer(new SHVBufferCPtr(SHVBufferCPtr::VoidToBuffer(bufArray1,2)));
	testBuffer.AddBuffer(new SHVBufferCPtr(SHVBufferCPtr::VoidToBuffer(bufArray2,8)));
	testBuffer.AddBuffer(new SHVBufferCPtr(SHVBufferCPtr::VoidToBuffer(bufArray3,8)));

	ok = PerformRead(testBuffer);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}


/*************************************
 * helpers
 *************************************/
bool SHVBufferTester::PerformRead(const SHVBufferC& testBuffer)
{
bool ok = true;
SHVBufferCIterator itr(testBuffer);
int i;
char c;
SHVString8 str;
SHVString16 str16;

	while (itr && ok)
	{
		switch (itr.GetField())
		{
		case 0:
			ok = (itr.ReadInt(i) && i == 127);
			break;
		case 1:
			ok = (itr.ReadNullString8(str) && str == "hey");
			break;
		case 2:
			if ((ok = itr.ReadChar(c)))
			{
				ok = (itr.ReadString8(str,c) && str == "you");
			}
			break;
		case 4:
			ok = (itr.ReadString16(str16,3) && str16 == _SHVS16("moo"));
			break;
		default:
			itr.SetError();
			ok = false;
		}
	}
	
	return (ok && itr.GetField() == 5);
}
