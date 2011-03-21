#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvfiletester.h"
#include "shiva/include/utils/shvfile.h"
#include "shiva/include/utils/shvdir.h"

# define _TESTFILE_  _S("tmpFileName.tmp")



/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVFileTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVFileTester::GetID() const
{
	return "file";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVFileTester::GetTitle() const
{
	return _S("SHVFile test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVFileTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionCreation, "creation", _S("Creation"), _S("This will test Creation"), &SHVFileTester::TestCreation },
	{ ActionStorage, "storage", _S("Storage"), _S("This will test Storage"), &SHVFileTester::TestStorage },
	{ ActionTextHandling, "texthandling", _S("TextHandling"), _S("This will test TextHandling"), &SHVFileTester::TestTextHandling },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVFileTester::TestCreation(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;
SHVFile file1;
SHVString fileName(modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _TESTFILE_);

	ok = true;

	if (SHVDir::FileExist(fileName))
		SHVDir::DeleteFile(fileName);

	ok = (ok && ( !SHVDir::FileExist(fileName) ));

	// test creation of a new file
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagOpen|SHVFileBase::FlagWrite) == SHVFileBase::ErrMissingFile ));
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite) ));
	ok = (ok && ( file1.WriteString8("1234") ));

	// Testing positioning
	ok = (ok && ( file1.GetSize() == 4 ));
	ok = (ok && ( file1.GetPosition() == 4 ));
	ok = (ok && ( file1.Seek(-2) && file1.GetPosition() == 2 ));
	ok = (ok && ( !file1.Seek(-3) && file1.GetPosition() == 2 ));
	ok = (ok && ( file1.Seek(-2) && file1.GetPosition() == 0 ));
	ok = (ok && ( !file1.Seek(5) && file1.GetPosition() == 0 ));
	ok = (ok && ( file1.SetPosition(file1.GetSize()) && file1.GetPosition() == 4 ));

	// testing open as create only
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite) == SHVFileBase::ErrExists ));

	// test position when opening read|write
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagOpen|SHVFileBase::FlagWrite|SHVFileBase::FlagRead) ));
	ok = (ok && ( file1.GetPosition() == 4 ));

	// testing override
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagOpen|SHVFileBase::FlagOverride|SHVFileBase::FlagWrite) ));
	ok = (ok && ( file1.GetSize() == 0 ));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVFileTester::TestStorage(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;
SHVInt nullInt,valInt(10);
SHVInt tmpInt;
SHVFile file1;
SHVString fileName(modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _TESTFILE_);

	ok = true;

	if (SHVDir::FileExist(fileName))
		SHVDir::DeleteFile(fileName);

	ok = (ok && ( !SHVDir::FileExist(fileName) ));

	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite|SHVFileBase::FlagRead) ));
	ok = (ok && ( file1.Write(&valInt,  sizeof(SHVInt)) ));
	ok = (ok && ( file1.Write(&nullInt, sizeof(SHVInt)) ));

	ok = (ok && ( file1.SetPosition(0) ));
	ok = (ok && ( file1.Read(&tmpInt, sizeof(SHVInt)) == sizeof(SHVInt) && !tmpInt.IsNull() && tmpInt == SHVInt(10) ) );
	ok = (ok && ( file1.Read(&tmpInt, sizeof(SHVInt)) == sizeof(SHVInt) && tmpInt.IsNull() ) );

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVFileTester::TestTextHandling(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;
SHVFile file1;
SHVString fileName(_TESTFILE_);
SHVString8 tmpStr;

	ok = true;

	if (SHVDir::FileExist(fileName))
		SHVDir::DeleteFile(fileName);

	ok = (ok && ( !SHVDir::FileExist(fileName) ));

	// testing String8 operations
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagCreate|SHVFileBase::FlagWrite|SHVFileBase::FlagRead) ));
	ok = (ok && ( file1.WriteString8(_SHVS8("testing ")) ));
	ok = (ok && ( file1.WriteString8(_SHVS8("testing ")) ));
	ok = (ok && ( file1.WriteLine8(_SHVS8("123")) ));
	ok = (ok && ( file1.WriteLine8(_SHVS8("line 2")) ));
	ok = (ok && ( file1.WriteString8(_SHVS8("line 3")) ));

	ok = (ok && ( file1.SetPosition(0) ));
	tmpStr.SetBufferSize(5);
	ok = (ok && ( file1.ReadString8(tmpStr) && tmpStr == "test" ));
	ok = (ok && ( file1.SetPosition(0) ));
	ok = (ok && ( file1.ReadLine8(tmpStr) && tmpStr == "testing testing 123" ));
	ok = (ok && ( file1.ReadLine8(tmpStr) && tmpStr == "line 2" ));
	ok = (ok && ( file1.ReadLine8(tmpStr) && tmpStr == "line 3" ));

#ifndef __SHVSTRING_EXCLUDE_UNICODE
SHVString16 tmpStr16;
	ok = (ok && ( file1.Open(fileName,SHVFileBase::FlagOpen|SHVFileBase::FlagOverride|SHVFileBase::FlagWrite|SHVFileBase::FlagRead) ));
	ok = (ok && ( file1.WriteString16(_SHVS16("testing ")) ));
	ok = (ok && ( file1.WriteString16(_SHVS16("testing ")) ));
	ok = (ok && ( file1.WriteLine16(_SHVS16("123")) ));
	ok = (ok && ( file1.WriteLine16(_SHVS16("line 2")) ));
	ok = (ok && ( file1.WriteString16(_SHVS16("line 3")) ));

	ok = (ok && ( file1.SetPosition(0) ));
	tmpStr16.SetBufferSize(5);
	ok = (ok && ( file1.ReadString16(tmpStr16) && tmpStr16 == _SHVS16("test") ));
	ok = (ok && ( file1.SetPosition(0) ));
	ok = (ok && ( file1.ReadLine16(tmpStr16) && tmpStr16 == _SHVS16("testing testing 123") ));
	ok = (ok && ( file1.ReadLine16(tmpStr16) && tmpStr16 == _SHVS16("line 2") ));
	ok = (ok && ( file1.ReadLine16(tmpStr16) && tmpStr16 == _SHVS16("line 3") ));
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
