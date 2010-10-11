#include "../../../include/platformspc.h"
#include "shvjsonreaderimpl.h"

//=========================================================================================================
// SHVJsonReaderImpl - Json reader implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVJsonReaderImpl::SHVJsonReaderImpl(const SHVStringUTF8C rootName): RootName(rootName)
{
	Config.allow_comments = 1;
	Config.callback = &SHVJsonReaderImpl::Callback;
	Config.callback_ctx = (void*) this;
	Config.depth =  -1;
	Config.handle_floats_manually = 0;
	Parser = new_JSON_parser(&Config);
}

/*************************************
 * Destructor
 *************************************/
SHVJsonReaderImpl::~SHVJsonReaderImpl()
{
	delete_JSON_parser(Parser);
}

/*************************************
 * Parse
 *************************************/
SHVBool SHVJsonReaderImpl::Parse(SHVStreamIn& stream)
{
SHVBool retVal = true;
	while (!stream.Eof() && retVal)
	{
		retVal = JSON_parser_char(Parser, stream.ReadChar()) != 0;
	}
	return retVal;
}

/*************************************
 * ParseDirect
 *************************************/
SHVBool SHVJsonReaderImpl::ParseDirect(const char* buffer, int actualLen)
{
int pos = 0;
SHVBool retVal = true;
	while (pos < actualLen && retVal)
	{
		JSON_parser_char(Parser, buffer[pos++]);
	}
	return retVal;
}

/*************************************
 * operator[]
 *************************************/
const SHVJsonKeyValuePair& SHVJsonReaderImpl::operator[](size_t idx)
{
	return *Stack.GetLast().KeyValuePairs[idx];
}

/*************************************
 * GetCount
 *************************************/
int SHVJsonReaderImpl::GetCount()
{
	return Stack.GetLast().KeyValuePairs.CalculateCount();
}

/*************************************
 * SetStartObjectCallback
 *************************************/
void SHVJsonReaderImpl::SetStartObjectCallback(SHVJsonReaderCallbackBase* callback)
{
	StartObjectCallback = callback;
}

/*************************************
 * SetEndObjectCallback
 *************************************/
void SHVJsonReaderImpl::SetEndObjectCallback(SHVJsonReaderCallbackBase* callback)
{
	EndObjectCallback = callback;
}

/*************************************
 * SetStartArrayCallback
 *************************************/
void SHVJsonReaderImpl::SetStartArrayCallback(SHVJsonReaderCallbackBase* callback)
{
	StartArrayCallback = callback;
}

/*************************************
 * SetEndArrayCallback
 *************************************/
void SHVJsonReaderImpl::SetEndArrayCallback(SHVJsonReaderCallbackBase* callback)
{
	EndArrayCallback = callback;
}

int SHVJsonReaderImpl::Callback(void *ctx, int type, const JSON_value_struct *value)
{
SHVJsonReaderImpl* self = (SHVJsonReaderImpl*) ctx;
bool cont = true;

	switch (type)
	{
	case JSON_T_KEY:
		self->CurrentKey = SHVStringUTF8C(SHVStringUTF8C(value->vu.str.value));
		break;
	case JSON_T_OBJECT_BEGIN:
		self->Stack.AddTail(SHVJsonStack());
		if (self->StartObjectCallback)
			cont = self->StartObjectCallback->PerformCallback(*self);
		break;
	case JSON_T_OBJECT_END:
		if (self->EndObjectCallback)
			cont = self->EndObjectCallback->PerformCallback(*self);
		self->Stack.PopTail();
		break;
	case JSON_T_ARRAY_BEGIN:
		if (self->StartArrayCallback)
			cont = self->StartArrayCallback->PerformCallback(*self);
		break;
	case JSON_T_ARRAY_END:
		if (self->EndArrayCallback)
			cont = self->EndArrayCallback->PerformCallback(*self);
		break;
	default:
		if (self->Stack.GetCount())
		{
			self->Stack.GetLast().KeyValuePairs.Add(new SHVJsonKeyValuePairImpl(self->CurrentKey,(JSON_type) type,value));
			self->CurrentKey = NULL;
		}
		break;
	}
	return (cont ? 1 : 0);
}

//=========================================================================================================
// SHVJsonKeyValuePairImpl - Implementation of key/value pair
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVJsonKeyValuePairImpl::SHVJsonKeyValuePairImpl(const SHVStringUTF8C name, JSON_type type, const JSON_value_struct* val):
		Name(name)
{
	switch (type)
	{
	case JSON_T_INTEGER:
		Type = SHVJsonKeyValuePair::Type_Int;
		Data.intVal64 = val->vu.integer_value;
		break;
	case JSON_T_FLOAT:
		Type = SHVJsonKeyValuePair::Type_Float;
		Data.doubleVal = val->vu.float_value;
		break;
	case JSON_T_FALSE:
		Type = SHVJsonKeyValuePair::Type_Bool;
		Data.boolVal = false;
		break;
	case JSON_T_TRUE:
		Type = SHVJsonKeyValuePair::Type_Bool;
		Data.boolVal = true;
		break;
	case JSON_T_STRING:
		Type = SHVJsonKeyValuePair::Type_String;
		Data.stringVal = new SHVStringUTF8(val->vu.str.value);
		break;
	case JSON_T_NULL:
		Type = SHVJsonKeyValuePair::Type_Null;
		break;
	default:
		break;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVJsonKeyValuePairImpl::~SHVJsonKeyValuePairImpl()
{
	if (Type == SHVJsonKeyValuePair::Type_String)
		delete Data.stringVal;
}

/*************************************
 * GetName8
 *************************************/
SHVStringBuffer8 SHVJsonKeyValuePairImpl::GetName8() const
{
SHVString8 retVal(Name.GetSafeBuffer());
	return retVal.ReleaseBuffer();
}

/*************************************
 * GetNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVJsonKeyValuePairImpl::GetNameUTF8() const
{
SHVStringUTF8 retVal(Name);
	return retVal.ReleaseBuffer();
}

/*************************************
 * GetName16
 *************************************/
SHVStringBuffer16 SHVJsonKeyValuePairImpl::GetName16() const
{
SHVString16 retVal(Name.ToStr16());
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVJsonKeyValuePairImpl::AsInt() const
{
SHVInt retVal;
	if (Type == SHVJsonKeyValuePair::Type_Int)
		retVal = Data.intVal;
	return retVal;
}

/*************************************
 * AsInt64
 *************************************/
SHVInt64 SHVJsonKeyValuePairImpl::AsInt64() const
{
SHVInt64 retVal;
	if (Type == SHVJsonKeyValuePair::Type_Int)
		retVal = Data.intVal64;
	return retVal;
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVJsonKeyValuePairImpl::AsDouble() const
{
SHVDouble retVal;
	if (Type == SHVJsonKeyValuePair::Type_Float)
		retVal = Data.doubleVal;
	return retVal;
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVJsonKeyValuePairImpl::AsBool() const
{
SHVBool retVal = false;

	switch (Type)
	{
	case SHVJsonKeyValuePair::Type_Int:
		retVal = AsInt64() != 0;
		break;
	case SHVJsonKeyValuePair::Type_Float:
		retVal = AsDouble() != 0.0;
		break;
	case SHVJsonKeyValuePair::Type_String:
		retVal = !Data.stringVal->IsEmpty();
		break;
	case SHVJsonKeyValuePair::Type_Bool:
		retVal = Data.boolVal;
		break;
	case SHVJsonKeyValuePair::Type_Null:
		retVal = SHVBool::False;
		break;
	}
	return retVal;
}

/*************************************
 * AsString8
 *************************************/
SHVStringBuffer8 SHVJsonKeyValuePairImpl::AsString8() const
{
SHVString8 retVal(AsStringUTF8().GetBufferConst());
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsStringUTF8
 *************************************/
SHVStringBufferUTF8 SHVJsonKeyValuePairImpl::AsStringUTF8() const
{
SHVStringUTF8 retVal;
	switch (Type)
	{
	case SHVJsonKeyValuePair::Type_Int:
		retVal = SHVStringUTF8C::Int64ToString(Data.intVal64);
		break;
	case SHVJsonKeyValuePair::Type_Float:
		retVal = SHVStringUTF8C::DoubleToString(Data.doubleVal);
		break;
	case SHVJsonKeyValuePair::Type_String:
		retVal = *Data.stringVal;
		break;
	case SHVJsonKeyValuePair::Type_Bool:
		retVal = (AsBool() ? "1" : "0");
		break;
	case SHVJsonKeyValuePair::Type_Null:
		retVal = NULL;
		break;
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsString16
 *************************************/
SHVStringBuffer16 SHVJsonKeyValuePairImpl::AsString16() const
{
SHVString16 retVal(AsStringUTF8().ToStr16());
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsDateTime
 *************************************/
SHVTime SHVJsonKeyValuePairImpl::AsDateTime() const
{
SHVTime retVal;
	retVal.SetFromDateString(AsString());
	return retVal;
}

/*************************************
 * GetType
 *************************************/
SHVJsonKeyValuePair::JsonType SHVJsonKeyValuePairImpl::GetType() const
{
	return Type;
}
