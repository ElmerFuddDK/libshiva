#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shvstringtester.h"
#include "shiva/include/utils/shvstringutf8.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVStringTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVStringTester::GetID() const
{
	return "string";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVStringTester::GetTitle() const
{
	return _S("SHVString test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVStringTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionComparison, "comparison", _S("Comparison"), _S("This will test comparison"), &SHVStringTester::TestComparison },
	{ ActionConversion, "conversion", _S("Conversion"), _S("This will test conversion"), &SHVStringTester::TestConversion },
	{ ActionRightLeftMid, "rightleftmid", _S("RightLeftMid"), _S("This will test Right, Left and Mid"), &SHVStringTester::TestRightLeftMid },
	{ ActionTokenizeTrim, "tokenizetrim", _S("TokenizeTrim"), _S("This will test Tokenize and Trim"), &SHVStringTester::TestTokenizeTrim },
	{ ActionFindLocate, "findlocate", _S("FindLocate"), _S("This will test Find and Locate"), &SHVStringTester::TestFindLocate },
	{ ActionReplaceFormat, "replaceformat", _S("ReplaceFormat"), _S("This will test replace and format"), &SHVStringTester::TestReplaceFormat },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVStringTester::TestComparison(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 lowerStr(_SHVS8("MJALLOW"));
	SHVString8 higherStr(_SHVS8("mjallow"));
	SHVString8 equalStr(_SHVS8("MJALLOW"));

		ok = (ok &&  (nullStr <  lowerStr));
		ok = (ok &&  (nullStr <= lowerStr));
		ok = (ok && !(nullStr >  lowerStr));
		ok = (ok && !(nullStr >= lowerStr));
		ok = (ok && !(nullStr == lowerStr));
		ok = (ok &&  (nullStr != lowerStr));
		ok = (ok &&  (nullStr.Compare(lowerStr) < 0));
		ok = (ok &&  (nullStr.CompareNoCase(lowerStr) < 0));

		ok = (ok && !(higherStr <  lowerStr));
		ok = (ok && !(higherStr <= lowerStr));
		ok = (ok &&  (higherStr >  lowerStr));
		ok = (ok &&  (higherStr >= lowerStr));
		ok = (ok && !(higherStr == lowerStr));
		ok = (ok &&  (higherStr != lowerStr));
		ok = (ok &&  (higherStr.Compare(lowerStr) > 0));
		ok = (ok &&  (higherStr.CompareNoCase(lowerStr) == 0));

		ok = (ok && !(equalStr <  lowerStr));
		ok = (ok &&  (equalStr <= lowerStr));
		ok = (ok && !(equalStr >  lowerStr));
		ok = (ok &&  (equalStr >= lowerStr));
		ok = (ok &&  (equalStr == lowerStr));
		ok = (ok && !(equalStr != lowerStr));
		ok = (ok &&  (equalStr.Compare(lowerStr) == 0));
		ok = (ok &&  (equalStr.CompareNoCase(lowerStr) == 0));

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
	// silly test of refcounting strings
	if (ok)
	{
	const SHVString8 strC(_SHVS8("MJALLOW"));
		ok = true;
		
		{
		SHVString8CRef strCRef(strC.GetBufferConst());
		SHVString8CRef strNullRef(NULL);
			{
			SHVString8CRef strCRef2(strCRef);
			SHVString8CRef strCRef3;
				strCRef2 = strCRef3;
				strCRef3 = strCRef;
				
				ok = (ok && (strCRef2.IsNull()));
			}

			ok = (ok && (strC.CompareNoCase(strCRef) == 0));
			ok = (ok && (strNullRef.IsNull()));

		}

		// log.AddLine(_S("Result for SHVString8CRef:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 nullStr;
	SHVString16 lowerStr(_SHVS16("MJALLOW"));
	SHVString16 higherStr(_SHVS16("mjallow"));
	SHVString16 equalStr(_SHVS16("MJALLOW"));

		ok = (ok &&  (nullStr <  lowerStr));
		ok = (ok &&  (nullStr <= lowerStr));
		ok = (ok && !(nullStr >  lowerStr));
		ok = (ok && !(nullStr >= lowerStr));
		ok = (ok && !(nullStr == lowerStr));
		ok = (ok &&  (nullStr != lowerStr));
		ok = (ok &&  (nullStr.Compare(lowerStr) < 0));
		ok = (ok &&  (nullStr.CompareNoCase(lowerStr) < 0));

		ok = (ok && !(higherStr <  lowerStr));
		ok = (ok && !(higherStr <= lowerStr));
		ok = (ok &&  (higherStr >  lowerStr));
		ok = (ok &&  (higherStr >= lowerStr));
		ok = (ok && !(higherStr == lowerStr));
		ok = (ok &&  (higherStr != lowerStr));
		ok = (ok &&  (higherStr.Compare(lowerStr) > 0));
		ok = (ok &&  (higherStr.CompareNoCase(lowerStr) == 0));

		ok = (ok && !(equalStr <  lowerStr));
		ok = (ok &&  (equalStr <= lowerStr));
		ok = (ok && !(equalStr >  lowerStr));
		ok = (ok &&  (equalStr >= lowerStr));
		ok = (ok &&  (equalStr == lowerStr));
		ok = (ok && !(equalStr != lowerStr));
		ok = (ok &&  (equalStr.Compare(lowerStr) == 0));
		ok = (ok &&  (equalStr.CompareNoCase(lowerStr) == 0));

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
	// silly test of refcounting strings
	if (ok)
	{
		const SHVString16 strC(_SHVS16("MJALLOW"));
		
		{
			SHVString16CRef strCRef(strC.GetBufferConst());
			SHVString16CRef strNullRef(NULL);
			{
				SHVString16CRef strCRef2(strCRef);
				SHVString16CRef strCRef3;
				strCRef2 = strCRef3;
				strCRef3 = strCRef;
				
				ok = (ok && (strCRef2.IsNull()));
			}

			ok = (ok && (strC.CompareNoCase(strCRef) == 0));
			ok = (ok && (strNullRef.IsNull()));

		}

		// log.AddLine(_S("Result for SHVString16CRef:  %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVStringTester::TestConversion(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 noNumber(_SHVS8("text and such"));
	SHVString8 cleanInt(_SHVS8("1234"));
	SHVString8 text(_SHVS8(" bottles of beer on the wall"));
	SHVString8 intWithText(_SHVS8("42 bottles of beer on the wall"));
	SHVChar* ch = NULL;

		ok = (ok && (nullStr.ToLong() == 0));
		ok = (ok && (noNumber.ToLong() == 0));
		ok = (ok && (cleanInt.ToLong() == 1234));
		ok = (ok && (intWithText.ToLong(&ch) == 42));
		ok = (ok && (SHVString8C(ch) == text));

		ok = (ok && (SHVString8(SHVString8C::LongToString(1234)) == cleanInt));

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 nullStr;
	SHVString16 noNumber(_SHVS16("text and such"));
	SHVString16 cleanInt(_SHVS16("1234"));
	SHVString16 text(_SHVS16(" bottles of beer on the wall"));
	SHVString16 intWithText(_SHVS16("42 bottles of beer on the wall"));
	SHVWChar* ch = NULL;

		ok = (ok && (nullStr.ToLong() == 0));
		ok = (ok && (noNumber.ToLong() == 0));
		ok = (ok && (cleanInt.ToLong() == 1234));
		ok = (ok && (intWithText.ToLong(&ch) == 42));
		ok = (ok && (SHVString16C(ch) == text));

		ok = (ok && (SHVString16(SHVString16C::LongToString(1234)) == cleanInt));

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
	{
	SHVString8 str(SHVStringUTF8C("hejÃ¥ noget").ToStr8());
	SHVString8 testStr(_SHVS8("hejå noget"));

		ok = (ok && (testStr == str));

		// log.AddLine(_S("Result for SHVString8(UTF8): %s"), log.Success(ok));
	}
	{
	SHVString16 str(SHVStringUTF8C("hejÃ¥ noget").ToStr16());
	SHVString16 testStr(_SHVS16("hejå noget"));

		ok = (ok && (testStr == str));

		// log.AddLine(_S("Result for SHVString16(UTF8): %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVStringTester::TestRightLeftMid(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 text(_SHVS8("42 bottles of beer on the wall"));
	SHVString8 smallTxt(_SHVS8("42"));

		ok = (ok && (nullStr.Left(10).IsNull()));
		ok = (ok && (nullStr.Right(10).IsNull()));
		ok = (ok && (nullStr.Mid(10).IsNull()));
		ok = (ok && (nullStr.Mid(10,10).IsNull()));

		ok = (ok && (text.Left(10) == _SHVS8("42 bottles")));
		ok = (ok && (text.Right(10) == _SHVS8("n the wall")));
		ok = (ok && (text.Mid(10) == _SHVS8(" of beer on the wall")));
		ok = (ok && (text.Mid(10,10) == _SHVS8(" of beer o")));

		ok = (ok && (smallTxt.Left(10) == _SHVS8("42")));
		ok = (ok && (smallTxt.Right(10) == _SHVS8("42")));
		ok = (ok && (smallTxt.Mid(10).IsNull()));
		ok = (ok && (smallTxt.Mid(10,10).IsNull()));

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 nullStr;
	SHVString16 text(_SHVS16("42 bottles of beer on the wall"));
	SHVString16 smallTxt(_SHVS16("42"));

		ok = (ok && (nullStr.Left(10).IsNull()));
		ok = (ok && (nullStr.Right(10).IsNull()));
		ok = (ok && (nullStr.Mid(10).IsNull()));
		ok = (ok && (nullStr.Mid(10,10).IsNull()));

		ok = (ok && (text.Left(10) == _SHVS16("42 bottles")));
		ok = (ok && (text.Right(10) == _SHVS16("n the wall")));
		ok = (ok && (text.Mid(10) == _SHVS16(" of beer on the wall")));
		ok = (ok && (text.Mid(10,10) == _SHVS16(" of beer o")));

		ok = (ok && (smallTxt.Left(10) == _SHVS16("42")));
		ok = (ok && (smallTxt.Right(10) == _SHVS16("42")));
		ok = (ok && (smallTxt.Mid(10).IsNull()));
		ok = (ok && (smallTxt.Mid(10,10).IsNull()));

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVStringTester::TestTokenizeTrim(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 text(_SHVS8("  42 bottles of beer on the wall  "));
	SHVString8 trimResult(_SHVS8("42 bottles of beer on the wall"));
	SHVString8 tokenStr(_SHVS8("or"));
	SHVString8 str;
	size_t pos, sLen;

		text.Trim();
		ok = (ok && (text == trimResult));

		pos = 0;
		sLen = text.GetLength();
		while (ok && pos < text.GetLength())
		{
			str = text.Tokenize(tokenStr,pos);
			switch (pos)
			{
			case  5: ok = ( str == _SHVS8("42 b"));			break;
			case 12: ok = ( str == _SHVS8("ttles "));		break;
			case 18: ok = ( str == _SHVS8("f bee"));		break;
			case 20: ok = ( str == _SHVS8(" "));			break;
			case 30: ok = ( str == _SHVS8("n the wall"));	break;
			default:
				ok = false;
			}
		}

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 text(_SHVS16("  42 bottles of beer on the wall  "));
	SHVString16 trimResult(_SHVS16("42 bottles of beer on the wall"));
	SHVString16 tokenStr(_SHVS16("or"));
	SHVString16 str;
	size_t pos, sLen;

		text.Trim();
		ok = (ok && (text == trimResult));

		pos = 0;
		sLen = text.GetLength();
		while (ok && pos < text.GetLength())
		{
			str = text.Tokenize(tokenStr,pos);
			switch (pos)
			{
			case  5: ok = ( str == _SHVS16("42 b"));		break;
			case 12: ok = ( str == _SHVS16("ttles "));		break;
			case 18: ok = ( str == _SHVS16("f bee"));		break;
			case 20: ok = ( str == _SHVS16(" "));			break;
			case 30: ok = ( str == _SHVS16("n the wall"));	break;
			default:
				ok = false;
			}
		}

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVStringTester::TestFindLocate(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 text(_SHVS8("42 bottles of beer on le wall")); // typo intended
	SHVString8 searchStr(_SHVS8("le"));
	SHVString8 searchStr2(_SHVS8("leGremlin"));

		ok = (ok && (nullStr.Find(searchStr) == -1));
		ok = (ok && (nullStr.ReverseFind(searchStr) == -1));
		ok = (ok && (text.Find(searchStr) == 7));
		ok = (ok && (text.ReverseFind(searchStr) == 22));
		ok = (ok && (text.Find(searchStr2) == -1));
		ok = (ok && (text.ReverseFind(searchStr2) == -1));

		ok = (ok && (nullStr.LocateChar('r') == -1));
		ok = (ok && (text.LocateChar('r') == 17));
		ok = (ok && (text.LocateChar('R') == -1));

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 nullStr;
	SHVString16 text(_SHVS16("42 bottles of beer on le wall")); // typo intended
	SHVString16 searchStr(_SHVS16("le"));
	SHVString16 searchStr2(_SHVS16("leGremlin"));

		ok = (ok && (nullStr.Find(searchStr) == -1));
		ok = (ok && (nullStr.ReverseFind(searchStr) == -1));
		ok = (ok && (text.Find(searchStr) == 7));
		ok = (ok && (text.ReverseFind(searchStr) == 22));
		ok = (ok && (text.Find(searchStr2) == -1));
		ok = (ok && (text.ReverseFind(searchStr2) == -1));

		ok = (ok && (nullStr.LocateChar('r') == -1));
		ok = (ok && (text.LocateChar('r') == 17));
		ok = (ok && (text.LocateChar('R') == -1));

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVStringTester::TestReplaceFormat(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;

	{
	SHVString8 nullStr;
	SHVString8 preReplace(_SHVS8("1 2 3 4 5"));
	SHVString8 postReplace(_SHVS8("1  2  3  4  5"));
	SHVString8 str(_SHVS8("test"));

		nullStr.Replace(_SHVS8(" "), _SHVS8("  "));
		preReplace.Replace(_SHVS8(" "), _SHVS8("  "));
		str.Replace(_SHVS8(" "), _SHVS8("  "));
		ok = (ok && (nullStr.IsNull()));
		ok = (ok && (preReplace == postReplace));
		ok = (ok && (str == _SHVS8("test")));

		if (ok) // test integer formatting
		{
		SHVString8 s(_SHVS8("%d %20d %u"));
		SHVString8 res(_SHVS8("1234                 1234 1234"));
		int i = 1234;
			str.Format(s.GetBufferConst(), i, i, i);
			ok = (ok && (str == res));
		}
		if (ok) // test string formatting
		{
		SHVString8 s(_SHVS8("%c%s %5.5s %.5s %5s %.5s"));
		SHVString8 res(_SHVS8(" a very long string a ver a ver    ni ni"));
		SHVString8 s1(_SHVS8("a very long string"));
		SHVString8 s2(_SHVS8("ni"));
			str.Format(s.GetBufferConst(), 32, s1.GetBufferConst(), s1.GetBufferConst(), s1.GetBufferConst(), s2.GetBufferConst(), s2.GetBufferConst());
			ok = (ok && (str == res));
		}

		// log.AddLine(_S("Result for SHVString8:  %s"), log.Success(ok));
	}
#ifndef __SHVSTRING_EXCLUDE_UNICODE
	{
	SHVString16 nullStr;
	SHVString16 preReplace(_SHVS16("1 2 3 4 5"));
	SHVString16 postReplace(_SHVS16("1  2  3  4  5"));
	SHVString16 str(_SHVS16("test"));

		nullStr.Replace(_SHVS16(" "), _SHVS16("  "));
		preReplace.Replace(_SHVS16(" "), _SHVS16("  "));
		str.Replace(_SHVS16(" "), _SHVS16("  "));
		ok = (ok && (nullStr.IsNull()));
		ok = (ok && (preReplace == postReplace));
		ok = (ok && (str == _SHVS16("test")));

		if (ok) // test integer formatting
		{
		SHVString16 s(_SHVS16("%d %20d %u"));
		SHVString16 res(_SHVS16("1234                 1234 1234"));
		int i = 1234;
			str.Format(s.GetBufferConst(), i, i, i);
			ok = (ok && (str == res));
		}
		if (ok) // test string formatting
		{
		SHVString16 s(_SHVS16("%c%s %5.5s %.5s %5s %.5s"));
		SHVString16 res(_SHVS16(" a very long string a ver a ver    ni ni"));
		SHVString16 s1(_SHVS16("a very long string"));
		SHVString16 s2(_SHVS16("ni"));
			str.Format(s.GetBufferConst(), 32, s1.GetBufferConst(), s1.GetBufferConst(), s1.GetBufferConst(), s2.GetBufferConst(), s2.GetBufferConst());
			ok = (ok && (str == res));
		}

		// log.AddLine(_S("Result for SHVString16: %s"), log.Success(ok));
	}
#endif

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
