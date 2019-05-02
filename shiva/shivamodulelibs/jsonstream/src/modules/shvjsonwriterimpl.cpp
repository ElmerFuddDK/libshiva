#include "shiva/include/platformspc.h"
#include "shvjsonwriterimpl.h"

//=========================================================================================================
// SHVJsonWriterImpl - Json writer implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVJsonWriterImpl::SHVJsonWriterImpl()
{
}

/*************************************
 * Destructor
 *************************************/
SHVJsonWriterImpl::~SHVJsonWriterImpl()
{
}

/*************************************
 * WriteEndObject
 *************************************/
void SHVJsonWriterImpl::WriteEndObject(SHVStreamOut& w)
{
	w.WriteChar8('}');
	FirstElementStack.PopTail();
}


/*************************************
 * WriteEndArray
 *************************************/
void SHVJsonWriterImpl::WriteEndArray(SHVStreamOut& w)
{
	FirstElementStack.PopTail();
}

/*************************************
 * WriteStartArrayUTF8
 *************************************/
void SHVJsonWriterImpl::WriteStartArrayUTF8(SHVStreamOut &w, const SHVStringUTF8C key)
{
	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteChar8('[');
	FirstElementStack.AddTail(true);
}

/*************************************
 * WriteStartObjectUTF8
 *************************************/
void SHVJsonWriterImpl::WriteStartObjectUTF8(SHVStreamOut& w, const SHVStringUTF8C key)
{
	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteChar8('{');
	FirstElementStack.AddTail(true);
}

/*************************************
 * WriteStringUTF8
 *************************************/
void SHVJsonWriterImpl::WriteStringUTF8(SHVStreamOut &w, const SHVStringUTF8C val, const SHVStringUTF8C key)
{
size_t len;

	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteChar8('"');
	len = val.GetSizeInBytes();
	for (size_t i = 0; i < len; i++)
	{
		switch (val.GetBufferConst()[i])
		{
		case '"':
		case '\\':
		case '/':
			w.WriteChar8('\\');
			w.WriteChar8(val.GetBufferConst()[i]);
			break;
		case '\b':
			w.WriteString8("\\b");
			break;
		case '\f':
			w.WriteString8("\\f");
			break;
		case '\n':
			w.WriteString8("\\n");
			break;
		case '\r':
			w.WriteString8("\\r");
			break;
		case '\t':
			w.WriteString8("\\t");
			break;
		default:
			if (val.GetBufferConst()[i] >= 0 && val.GetBufferConst()[i] < 32)
				w.WriteString8(SHVString8C::Format("\\u%04X", (unsigned int) val.GetBufferConst()[i]).GetBufferConst());
			else
				w.WriteChar8(val.GetBufferConst()[i]);
			break;
		}
	}
	w.WriteChar('"');
}

/*************************************
 * WriteIntUTF8
 *************************************/
void SHVJsonWriterImpl::WriteIntUTF8(SHVStreamOut &w, SHVInt val, const SHVStringUTF8C key)
{
	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteString8(SHVString8C::LongToString(val).GetSafeBuffer());
}

/*************************************
 * WriteDoubleUTF8
 *************************************/
void SHVJsonWriterImpl::WriteDoubleUTF8(SHVStreamOut &w, SHVDouble val, const SHVStringUTF8C key)
{
	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteString8(SHVString8C::DoubleToString(val).GetSafeBuffer());
}

/*************************************
 * WriteBoolUTF8
 *************************************/
void SHVJsonWriterImpl::WriteBoolUTF8(SHVStreamOut &w, SHVBool val, const SHVStringUTF8C key)
{
	if (FirstElementStack.GetLast())
	{
		FirstElementStack.PopTail();
		FirstElementStack.AddTail(false);
	}
	else
		w.WriteChar8(',');
	if (!key.IsNull())
	{
		w.WriteString8(key.GetBufferConst());
		w.WriteChar(':');
	}
	w.WriteString8(val ? "true" : "false");
}

/*************************************
 * WriteDateTimeUTF8
 *************************************/
void SHVJsonWriterImpl::WriteDateTimeUTF8(SHVStreamOut &w, const SHVTime &time, const SHVStringUTF8C key)
{
	WriteStringUTF8(w, time.ToDateString().ToStrUTF8(), key);
}
