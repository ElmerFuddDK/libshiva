#ifndef __SHIVA_FRAMEWORK_EVENTDATA_H
#define __SHIVA_FRAMEWORK_EVENTDATA_H

#include "shveventstring.h"


//-=========================================================================================================
/// SHVEventData class - template for encapsulating data into an event
/**
 * This class is a thin wrapper for encapsulating any kind of data object into an event.\n
 * It will return a pointer to the encapsulated data via the GetData method from SHVEvent.
 * It is designed so only the event emitter will have to know its implementation, since the
 * receiver will use GetData to obtain a pointer to the data itself:
\code
void SHVMyModule::EmitterTest()
{
SHVEventRef emitter;
	
	// emits an event with an integer attached, initialized to 1234
	EmitEvent(new SHVEventData<int>(1434,this,EventIntegerTest));
}

void SHVMyOtherModule::OnEvent(SHVEvent* event)
{
	// is this event an integer test event
	if (SHVEvent::Equals(event,SHVMyModule::EventIntegerTest))
	{
	int myInt;
	 
		// assign the integer value from the event to myInt 
		myInt = *(int*)event->GetData(); 
		
		// do some math on it
		myInt -= 768;
		
		// it is actually possible to manipulate the contents of the data in the event
		*(int*)event->GetData() = myInt;
	}
}
\endcode
 */

template<class T, class C = const T&>
class SHVEventData : public SHVEvent
{
public:
	SHVEventData(SHVEventTarget* caller, SHVInt id, SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);
	SHVEventData(C t, SHVEventTarget* caller, SHVInt id, SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);
	
	virtual void* GetData();

	static T& Get(SHVEvent* event);

private:
	T Data;
};


//-=========================================================================================================
/// SHVEventStringData class - template for encapsulating data into a string event
/**
 * This class is a thin wrapper for encapsulating any kind of data object into an event.\n
 * It will return a pointer to the encapsulated data via the GetData method from SHVEvent.
 * It is designed so only the event emitter will have to know its implementation, since the
 * receiver will use GetData to obtain a pointer to the data itself:
\code
void SHVMyModule::EmitterTest()
{
SHVEventRef emitter;
	
	// emits an event with an integer attached, initialized to 1234
	EmitEvent(new SHVEventStringData<int>(1434,this,"StringIntegerTest"));
}

void SHVMyOtherModule::OnEvent(SHVEvent* event)
{
	// is this event an integer test event
	if (SHVEventString::Equals(event,"StringIntegerTest"))
	{
	int myInt;
	 
		// assign the integer value from the event to myInt 
		myInt = *(int*)event->GetData(); 
		
		// do some math on it
		myInt -= 768;
		
		// it is actually possible to manipulate the contents of the data in the event
		*(int*)event->GetData() = myInt;
	}
}
\endcode
 */

template<class T, class C = const T&>
class SHVEventStringData : public SHVEventString
{
public:
	SHVEventStringData(SHVEventTarget* caller, const SHVChar* name, SHVInt id = SHVInt(), SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);
	SHVEventStringData(C t, SHVEventTarget* caller, const SHVChar* name, SHVInt id = SHVInt(), SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);
	
	virtual void* GetData();

private:
	T Data;
};


// ====================================== implementation - SHVEventData ===================================== //

/*************************************
  * Constructors
 *************************************/
/// default constructor
template<class T, class C>
SHVEventData<T,C>::SHVEventData(SHVEventTarget* caller, SHVInt id, SHVInt subid, SHVRefObject* obj)
 : SHVEvent(caller, id, subid, obj)
{}
/// constructor that takes some T - I prefer coffee tho
template<class T, class C>
SHVEventData<T,C>::SHVEventData(C t, SHVEventTarget* caller, SHVInt id, SHVInt subid, SHVRefObject* obj)
 : SHVEvent(caller, id, subid, obj), Data(t)
{}

/*************************************
  * GetData
 *************************************/
/// returns a pointer to T - why not point to the coffee machine instead
template<class T, class C>
void* SHVEventData<T,C>::GetData()
{
	return &Data;
}
/*************************************
  * Get
 *************************************/
/// returns a reference to the data embedded data 
template<class T, class C>
T& SHVEventData<T,C>::Get(SHVEvent* event)
{ 
	return ((SHVEventData<T,C>*) event)->Data; 
}

/*************************************
  * Constructors
 *************************************/
/// default constructor
template<class T, class C>
SHVEventStringData<T,C>::SHVEventStringData(SHVEventTarget* caller, const SHVChar* name, SHVInt id, SHVInt subid, SHVRefObject* obj)
 : SHVEventString(caller, name, id, subid, obj)
{}
/// constructor that takes some T - I prefer coffee tho
template<class T, class C>
SHVEventStringData<T,C>::SHVEventStringData(C t, SHVEventTarget* caller, const SHVChar* name, SHVInt id, SHVInt subid, SHVRefObject* obj)
 : SHVEventString(caller, name, id, subid, obj), Data(t)
{}

/*************************************
  * GetData
 *************************************/
/// returns a pointer to T - why not point to the coffee machine instead
template<class T, class C>
void* SHVEventStringData<T,C>::GetData()
{
	return &Data;
}

#endif
