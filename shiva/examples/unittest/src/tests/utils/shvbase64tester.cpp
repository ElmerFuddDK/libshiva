#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvbase64tester.h"
#include "shiva/include/utils/shvbase64.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVBase64Tester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVBase64Tester::GetID() const
{
	return "base64";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVBase64Tester::GetTitle() const
{
	return _S("SHVBase64Encoder/Decoder test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVBase64Tester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionSimpleEncoding, "simpleenc", _S("Simple encoding test"), _S("This will test some simple base64 encodings"), &SHVBase64Tester::TestSimpleEncoding },
	{ ActionSimpleDecoding, "simpledec", _S("Simple decoding test"), _S("This will test some simple base64 decodings"), &SHVBase64Tester::TestSimpleDecoding },
	{ ActionEncoding, "enc", _S("Advanced encoding test"), _S("This will perform a more advanced encoding test"), &SHVBase64Tester::TestEncoding },
	{ ActionDecoding, "dec", _S("Advanced decoding test"), _S("This will perform a more advanced decoding test"), &SHVBase64Tester::TestDecoding },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination

	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVBase64Tester::TestSimpleEncoding(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVBase64Encoder encoder;
const SHVString8C helloWorld("Hello World");
const SHVString8C helloWorldB64("SGVsbG8gV29ybGQ=");

	encoder.CalculateString8(helloWorld);

	ok = true;
	ok = (ok && encoder.GetDataAsString() == helloWorldB64);
	self->AddLine(_S("Encoded string : %s"), encoder.GetDataAsString().ToStrT().GetSafeBuffer());

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVBase64Tester::TestSimpleDecoding(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVBase64Decoder decoder;
const SHVString8C helloWorld("Hello World");
const SHVString8C helloWorldB64("SGVsbG8gV29ybGQ=");
const SHVString8C helloWorldB64Nl("SGVsb\r\nG8gV2\r\n9ybGQ=");
const SHVString8C helloWorldB64Fail("SGVsb\r\nG8g|||V2\r\n9ybGQ=");
SHVBufferRef buf;
size_t pos;
SHVString8 result;

	ok = true;

	decoder.CalculateString8(helloWorldB64);
	ok = (ok && decoder.GetError());
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result == helloWorld);

	decoder.CalculateString16(helloWorldB64.ToStr16());
	ok = (ok && decoder.GetError());
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result == helloWorld);

	decoder.CalculateString8(helloWorldB64Nl);
	ok = (ok && decoder.GetError());
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result == helloWorld);
	self->AddLine(_S("Decoded string : %s"), result.ToStrT().GetSafeBuffer());

	decoder.CalculateString8(helloWorldB64Fail);
	ok = (ok && decoder.GetError() == SHVBase64Decoder::ErrInvalidChar);
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result.IsEmpty());

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVBase64Tester::TestEncoding(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVBase64Encoder encoder;
const SHVString8C story("This is a tale about a cow. A cow with special s"
						"uper powers. A super cow, if you will. This cow "
						"was capable of performing incredible things. But"
						" she was not aware of her powers, hence they onl"
						"y came to play at random incidences, but more on"
						" that later. Her name was Molly. Molly the cow. "
						"A happy cow, for the most. Her favorite activiti"
						"es involved chewing and staring into the air, wh"
						"ich was perfect, since she had all the time in t"
						"he world for just that. Especially during the su"
						"mmer where she was free to roam around on the fi"
						"elds, full of fresh grass. Yum yum. It was a fin"
						"e summer morning. Molly had selected a good spot"
						" for chewing and staring, just next to a tree at"
						" the top of a hill. The weather was good, and ju"
						"st as she was busy chewing, the farmer appeared,"
						" checking up on his flock of cows. He was especi"
						"ally fond of Molly, since she always produced lo"
						"ts of milk. 'Hello Molly', he chirped, whilst sc"
						"ratching her head. It was at this moment her sup"
						"er powers came into effect. She started humming "
						"the first 2 verses of 'Oops I did it again' by B"
						"ritney Spears! Naturally the farmer had to have "
						"her put down, and have steaks made of here becau"
						"se of this. Mm, steaks. Yummy yummy, I got Molly"
						" in my tummy. The end.");
const SHVString8C enc("VGhpcyBpcyBhIHRhbGUgYWJvdXQgYSBjb3cuIEEgY293IHdp"
					  "dGggc3BlY2lhbCBzdXBlciBwb3dlcnMuIEEgc3VwZXIgY293"
					  "LCBpZiB5b3Ugd2lsbC4gVGhpcyBjb3cgd2FzIGNhcGFibGUg"
					  "b2YgcGVyZm9ybWluZyBpbmNyZWRpYmxlIHRoaW5ncy4gQnV0"
					  "IHNoZSB3YXMgbm90IGF3YXJlIG9mIGhlciBwb3dlcnMsIGhl"
					  "bmNlIHRoZXkgb25seSBjYW1lIHRvIHBsYXkgYXQgcmFuZG9t"
					  "IGluY2lkZW5jZXMsIGJ1dCBtb3JlIG9uIHRoYXQgbGF0ZXIu"
					  "IEhlciBuYW1lIHdhcyBNb2xseS4gTW9sbHkgdGhlIGNvdy4g"
					  "QSBoYXBweSBjb3csIGZvciB0aGUgbW9zdC4gSGVyIGZhdm9y"
					  "aXRlIGFjdGl2aXRpZXMgaW52b2x2ZWQgY2hld2luZyBhbmQg"
					  "c3RhcmluZyBpbnRvIHRoZSBhaXIsIHdoaWNoIHdhcyBwZXJm"
					  "ZWN0LCBzaW5jZSBzaGUgaGFkIGFsbCB0aGUgdGltZSBpbiB0"
					  "aGUgd29ybGQgZm9yIGp1c3QgdGhhdC4gRXNwZWNpYWxseSBk"
					  "dXJpbmcgdGhlIHN1bW1lciB3aGVyZSBzaGUgd2FzIGZyZWUg"
					  "dG8gcm9hbSBhcm91bmQgb24gdGhlIGZpZWxkcywgZnVsbCBv"
					  "ZiBmcmVzaCBncmFzcy4gWXVtIHl1bS4gSXQgd2FzIGEgZmlu"
					  "ZSBzdW1tZXIgbW9ybmluZy4gTW9sbHkgaGFkIHNlbGVjdGVk"
					  "IGEgZ29vZCBzcG90IGZvciBjaGV3aW5nIGFuZCBzdGFyaW5n"
					  "LCBqdXN0IG5leHQgdG8gYSB0cmVlIGF0IHRoZSB0b3Agb2Yg"
					  "YSBoaWxsLiBUaGUgd2VhdGhlciB3YXMgZ29vZCwgYW5kIGp1"
					  "c3QgYXMgc2hlIHdhcyBidXN5IGNoZXdpbmcsIHRoZSBmYXJt"
					  "ZXIgYXBwZWFyZWQsIGNoZWNraW5nIHVwIG9uIGhpcyBmbG9j"
					  "ayBvZiBjb3dzLiBIZSB3YXMgZXNwZWNpYWxseSBmb25kIG9m"
					  "IE1vbGx5LCBzaW5jZSBzaGUgYWx3YXlzIHByb2R1Y2VkIGxv"
					  "dHMgb2YgbWlsay4gJ0hlbGxvIE1vbGx5JywgaGUgY2hpcnBl"
					  "ZCwgd2hpbHN0IHNjcmF0Y2hpbmcgaGVyIGhlYWQuIEl0IHdh"
					  "cyBhdCB0aGlzIG1vbWVudCBoZXIgc3VwZXIgcG93ZXJzIGNh"
					  "bWUgaW50byBlZmZlY3QuIFNoZSBzdGFydGVkIGh1bW1pbmcg"
					  "dGhlIGZpcnN0IDIgdmVyc2VzIG9mICdPb3BzIEkgZGlkIGl0"
					  "IGFnYWluJyBieSBCcml0bmV5IFNwZWFycyEgTmF0dXJhbGx5"
					  "IHRoZSBmYXJtZXIgaGFkIHRvIGhhdmUgaGVyIHB1dCBkb3du"
					  "LCBhbmQgaGF2ZSBzdGVha3MgbWFkZSBvZiBoZXJlIGJlY2F1"
					  "c2Ugb2YgdGhpcy4gTW0sIHN0ZWFrcy4gWXVtbXkgeXVtbXks"
					  "IEkgZ290IE1vbGx5IGluIG15IHR1bW15LiBUaGUgZW5kLg==");
const SHVString8C encfolded("VGhpcyBpcyBhIHRhbGUgYWJvdXQgYSBjb3cuIEEgY293IHdp\r\n"
							"dGggc3BlY2lhbCBzdXBlciBwb3dlcnMuIEEgc3VwZXIgY293\r\n"
							"LCBpZiB5b3Ugd2lsbC4gVGhpcyBjb3cgd2FzIGNhcGFibGUg\r\n"
							"b2YgcGVyZm9ybWluZyBpbmNyZWRpYmxlIHRoaW5ncy4gQnV0\r\n"
							"IHNoZSB3YXMgbm90IGF3YXJlIG9mIGhlciBwb3dlcnMsIGhl\r\n"
							"bmNlIHRoZXkgb25seSBjYW1lIHRvIHBsYXkgYXQgcmFuZG9t\r\n"
							"IGluY2lkZW5jZXMsIGJ1dCBtb3JlIG9uIHRoYXQgbGF0ZXIu\r\n"
							"IEhlciBuYW1lIHdhcyBNb2xseS4gTW9sbHkgdGhlIGNvdy4g\r\n"
							"QSBoYXBweSBjb3csIGZvciB0aGUgbW9zdC4gSGVyIGZhdm9y\r\n"
							"aXRlIGFjdGl2aXRpZXMgaW52b2x2ZWQgY2hld2luZyBhbmQg\r\n"
							"c3RhcmluZyBpbnRvIHRoZSBhaXIsIHdoaWNoIHdhcyBwZXJm\r\n"
							"ZWN0LCBzaW5jZSBzaGUgaGFkIGFsbCB0aGUgdGltZSBpbiB0\r\n"
							"aGUgd29ybGQgZm9yIGp1c3QgdGhhdC4gRXNwZWNpYWxseSBk\r\n"
							"dXJpbmcgdGhlIHN1bW1lciB3aGVyZSBzaGUgd2FzIGZyZWUg\r\n"
							"dG8gcm9hbSBhcm91bmQgb24gdGhlIGZpZWxkcywgZnVsbCBv\r\n"
							"ZiBmcmVzaCBncmFzcy4gWXVtIHl1bS4gSXQgd2FzIGEgZmlu\r\n"
							"ZSBzdW1tZXIgbW9ybmluZy4gTW9sbHkgaGFkIHNlbGVjdGVk\r\n"
							"IGEgZ29vZCBzcG90IGZvciBjaGV3aW5nIGFuZCBzdGFyaW5n\r\n"
							"LCBqdXN0IG5leHQgdG8gYSB0cmVlIGF0IHRoZSB0b3Agb2Yg\r\n"
							"YSBoaWxsLiBUaGUgd2VhdGhlciB3YXMgZ29vZCwgYW5kIGp1\r\n"
							"c3QgYXMgc2hlIHdhcyBidXN5IGNoZXdpbmcsIHRoZSBmYXJt\r\n"
							"ZXIgYXBwZWFyZWQsIGNoZWNraW5nIHVwIG9uIGhpcyBmbG9j\r\n"
							"ayBvZiBjb3dzLiBIZSB3YXMgZXNwZWNpYWxseSBmb25kIG9m\r\n"
							"IE1vbGx5LCBzaW5jZSBzaGUgYWx3YXlzIHByb2R1Y2VkIGxv\r\n"
							"dHMgb2YgbWlsay4gJ0hlbGxvIE1vbGx5JywgaGUgY2hpcnBl\r\n"
							"ZCwgd2hpbHN0IHNjcmF0Y2hpbmcgaGVyIGhlYWQuIEl0IHdh\r\n"
							"cyBhdCB0aGlzIG1vbWVudCBoZXIgc3VwZXIgcG93ZXJzIGNh\r\n"
							"bWUgaW50byBlZmZlY3QuIFNoZSBzdGFydGVkIGh1bW1pbmcg\r\n"
							"dGhlIGZpcnN0IDIgdmVyc2VzIG9mICdPb3BzIEkgZGlkIGl0\r\n"
							"IGFnYWluJyBieSBCcml0bmV5IFNwZWFycyEgTmF0dXJhbGx5\r\n"
							"IHRoZSBmYXJtZXIgaGFkIHRvIGhhdmUgaGVyIHB1dCBkb3du\r\n"
							"LCBhbmQgaGF2ZSBzdGVha3MgbWFkZSBvZiBoZXJlIGJlY2F1\r\n"
							"c2Ugb2YgdGhpcy4gTW0sIHN0ZWFrcy4gWXVtbXkgeXVtbXks\r\n"
							"IEkgZ290IE1vbGx5IGluIG15IHR1bW15LiBUaGUgZW5kLg==\r\n");

	ok = true;

	encoder.CalculateString8(story);

	ok = true;
	ok = (ok && encoder.GetDataAsString() == enc);
	ok = (ok && encoder.GetDataAsString(48,"\r\n") == encfolded);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
bool SHVBase64Tester::TestDecoding(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVBase64Decoder decoder;
SHVBufferRef buf;
size_t pos;
SHVString8 result;
const SHVChar* ch;
SHVChar miniStr[2];
const SHVString8C story("This is a tale about a cow. A cow with special s"
						"uper powers. A super cow, if you will. This cow "
						"was capable of performing incredible things. But"
						" she was not aware of her powers, hence they onl"
						"y came to play at random incidences, but more on"
						" that later. Her name was Molly. Molly the cow. "
						"A happy cow, for the most. Her favorite activiti"
						"es involved chewing and staring into the air, wh"
						"ich was perfect, since she had all the time in t"
						"he world for just that. Especially during the su"
						"mmer where she was free to roam around on the fi"
						"elds, full of fresh grass. Yum yum. It was a fin"
						"e summer morning. Molly had selected a good spot"
						" for chewing and staring, just next to a tree at"
						" the top of a hill. The weather was good, and ju"
						"st as she was busy chewing, the farmer appeared,"
						" checking up on his flock of cows. He was especi"
						"ally fond of Molly, since she always produced lo"
						"ts of milk. 'Hello Molly', he chirped, whilst sc"
						"ratching her head. It was at this moment her sup"
						"er powers came into effect. She started humming "
						"the first 2 verses of 'Oops I did it again' by B"
						"ritney Spears! Naturally the farmer had to have "
						"her put down, and have steaks made of here becau"
						"se of this. Mm, steaks. Yummy yummy, I got Molly"
						" in my tummy. The end.");
const SHVString8C enc("VGhpcyBpcyBhIHRhbGUgYWJvdXQgYSBjb3cuIEEgY293IHdp"
					  "dGggc3BlY2lhbCBzdXBlciBwb3dlcnMuIEEgc3VwZXIgY293"
					  "LCBpZiB5b3Ugd2lsbC4gVGhpcyBjb3cgd2FzIGNhcGFibGUg"
					  "b2YgcGVyZm9ybWluZyBpbmNyZWRpYmxlIHRoaW5ncy4gQnV0"
					  "IHNoZSB3YXMgbm90IGF3YXJlIG9mIGhlciBwb3dlcnMsIGhl"
					  "bmNlIHRoZXkgb25seSBjYW1lIHRvIHBsYXkgYXQgcmFuZG9t"
					  "IGluY2lkZW5jZXMsIGJ1dCBtb3JlIG9uIHRoYXQgbGF0ZXIu"
					  "IEhlciBuYW1lIHdhcyBNb2xseS4gTW9sbHkgdGhlIGNvdy4g"
					  "QSBoYXBweSBjb3csIGZvciB0aGUgbW9zdC4gSGVyIGZhdm9y"
					  "aXRlIGFjdGl2aXRpZXMgaW52b2x2ZWQgY2hld2luZyBhbmQg"
					  "c3RhcmluZyBpbnRvIHRoZSBhaXIsIHdoaWNoIHdhcyBwZXJm"
					  "ZWN0LCBzaW5jZSBzaGUgaGFkIGFsbCB0aGUgdGltZSBpbiB0"
					  "aGUgd29ybGQgZm9yIGp1c3QgdGhhdC4gRXNwZWNpYWxseSBk"
					  "dXJpbmcgdGhlIHN1bW1lciB3aGVyZSBzaGUgd2FzIGZyZWUg"
					  "dG8gcm9hbSBhcm91bmQgb24gdGhlIGZpZWxkcywgZnVsbCBv"
					  "ZiBmcmVzaCBncmFzcy4gWXVtIHl1bS4gSXQgd2FzIGEgZmlu"
					  "ZSBzdW1tZXIgbW9ybmluZy4gTW9sbHkgaGFkIHNlbGVjdGVk"
					  "IGEgZ29vZCBzcG90IGZvciBjaGV3aW5nIGFuZCBzdGFyaW5n"
					  "LCBqdXN0IG5leHQgdG8gYSB0cmVlIGF0IHRoZSB0b3Agb2Yg"
					  "YSBoaWxsLiBUaGUgd2VhdGhlciB3YXMgZ29vZCwgYW5kIGp1"
					  "c3QgYXMgc2hlIHdhcyBidXN5IGNoZXdpbmcsIHRoZSBmYXJt"
					  "ZXIgYXBwZWFyZWQsIGNoZWNraW5nIHVwIG9uIGhpcyBmbG9j"
					  "ayBvZiBjb3dzLiBIZSB3YXMgZXNwZWNpYWxseSBmb25kIG9m"
					  "IE1vbGx5LCBzaW5jZSBzaGUgYWx3YXlzIHByb2R1Y2VkIGxv"
					  "dHMgb2YgbWlsay4gJ0hlbGxvIE1vbGx5JywgaGUgY2hpcnBl"
					  "ZCwgd2hpbHN0IHNjcmF0Y2hpbmcgaGVyIGhlYWQuIEl0IHdh"
					  "cyBhdCB0aGlzIG1vbWVudCBoZXIgc3VwZXIgcG93ZXJzIGNh"
					  "bWUgaW50byBlZmZlY3QuIFNoZSBzdGFydGVkIGh1bW1pbmcg"
					  "dGhlIGZpcnN0IDIgdmVyc2VzIG9mICdPb3BzIEkgZGlkIGl0"
					  "IGFnYWluJyBieSBCcml0bmV5IFNwZWFycyEgTmF0dXJhbGx5"
					  "IHRoZSBmYXJtZXIgaGFkIHRvIGhhdmUgaGVyIHB1dCBkb3du"
					  "LCBhbmQgaGF2ZSBzdGVha3MgbWFkZSBvZiBoZXJlIGJlY2F1"
					  "c2Ugb2YgdGhpcy4gTW0sIHN0ZWFrcy4gWXVtbXkgeXVtbXks"
					  "IEkgZ290IE1vbGx5IGluIG15IHR1bW15LiBUaGUgZW5kLg==");
const SHVString8C encfolded("VGhpcyBpcyBhIHRhbGUgYWJvdXQgYSBjb3cuIEEgY293IHdp\r\n"
							"dGggc3BlY2lhbCBzdXBlciBwb3dlcnMuIEEgc3VwZXIgY293\r\n"
							"LCBpZiB5b3Ugd2lsbC4gVGhpcyBjb3cgd2FzIGNhcGFibGUg\r\n"
							"b2YgcGVyZm9ybWluZyBpbmNyZWRpYmxlIHRoaW5ncy4gQnV0\r\n"
							"IHNoZSB3YXMgbm90IGF3YXJlIG9mIGhlciBwb3dlcnMsIGhl\r\n"
							"bmNlIHRoZXkgb25seSBjYW1lIHRvIHBsYXkgYXQgcmFuZG9t\r\n"
							"IGluY2lkZW5jZXMsIGJ1dCBtb3JlIG9uIHRoYXQgbGF0ZXIu\r\n"
							"IEhlciBuYW1lIHdhcyBNb2xseS4gTW9sbHkgdGhlIGNvdy4g\r\n"
							"QSBoYXBweSBjb3csIGZvciB0aGUgbW9zdC4gSGVyIGZhdm9y\r\n"
							"aXRlIGFjdGl2aXRpZXMgaW52b2x2ZWQgY2hld2luZyBhbmQg\r\n"
							"c3RhcmluZyBpbnRvIHRoZSBhaXIsIHdoaWNoIHdhcyBwZXJm\r\n"
							"ZWN0LCBzaW5jZSBzaGUgaGFkIGFsbCB0aGUgdGltZSBpbiB0\r\n"
							"aGUgd29ybGQgZm9yIGp1c3QgdGhhdC4gRXNwZWNpYWxseSBk\r\n"
							"dXJpbmcgdGhlIHN1bW1lciB3aGVyZSBzaGUgd2FzIGZyZWUg\r\n"
							"dG8gcm9hbSBhcm91bmQgb24gdGhlIGZpZWxkcywgZnVsbCBv\r\n"
							"ZiBmcmVzaCBncmFzcy4gWXVtIHl1bS4gSXQgd2FzIGEgZmlu\r\n"
							"ZSBzdW1tZXIgbW9ybmluZy4gTW9sbHkgaGFkIHNlbGVjdGVk\r\n"
							"IGEgZ29vZCBzcG90IGZvciBjaGV3aW5nIGFuZCBzdGFyaW5n\r\n"
							"LCBqdXN0IG5leHQgdG8gYSB0cmVlIGF0IHRoZSB0b3Agb2Yg\r\n"
							"YSBoaWxsLiBUaGUgd2VhdGhlciB3YXMgZ29vZCwgYW5kIGp1\r\n"
							"c3QgYXMgc2hlIHdhcyBidXN5IGNoZXdpbmcsIHRoZSBmYXJt\r\n"
							"ZXIgYXBwZWFyZWQsIGNoZWNraW5nIHVwIG9uIGhpcyBmbG9j\r\n"
							"ayBvZiBjb3dzLiBIZSB3YXMgZXNwZWNpYWxseSBmb25kIG9m\r\n"
							"IE1vbGx5LCBzaW5jZSBzaGUgYWx3YXlzIHByb2R1Y2VkIGxv\r\n"
							"dHMgb2YgbWlsay4gJ0hlbGxvIE1vbGx5JywgaGUgY2hpcnBl\r\n"
							"ZCwgd2hpbHN0IHNjcmF0Y2hpbmcgaGVyIGhlYWQuIEl0IHdh\r\n"
							"cyBhdCB0aGlzIG1vbWVudCBoZXIgc3VwZXIgcG93ZXJzIGNh\r\n"
							"bWUgaW50byBlZmZlY3QuIFNoZSBzdGFydGVkIGh1bW1pbmcg\r\n"
							"dGhlIGZpcnN0IDIgdmVyc2VzIG9mICdPb3BzIEkgZGlkIGl0\r\n"
							"IGFnYWluJyBieSBCcml0bmV5IFNwZWFycyEgTmF0dXJhbGx5\r\n"
							"IHRoZSBmYXJtZXIgaGFkIHRvIGhhdmUgaGVyIHB1dCBkb3du\r\n"
							"LCBhbmQgaGF2ZSBzdGVha3MgbWFkZSBvZiBoZXJlIGJlY2F1\r\n"
							"c2Ugb2YgdGhpcy4gTW0sIHN0ZWFrcy4gWXVtbXkgeXVtbXks\r\n"
							"IEkgZ290IE1vbGx5IGluIG15IHR1bW15LiBUaGUgZW5kLg==\r\n");

	ok = true;
	miniStr[1] = '\0';

	decoder.CalculateString8(enc);

	ok = true;
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result == story);

	for (ch = encfolded.GetBufferConst(); *ch && ok && decoder.GetError(); ch++)
	{
		miniStr[0] = *ch;
		decoder.CalculateString8(miniStr);
	}
	buf = decoder.GetData();
	pos = 0;
	ok = (ok && buf->ReadString8(result,buf->GetSize(),pos));
	ok = (ok && result == story);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());

	return ok;
}
