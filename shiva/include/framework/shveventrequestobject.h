#ifndef __SHIVA_FRAMEWORK_EVENTREQUESTOBJECT_H
#define __SHIVA_FRAMEWORK_EVENTREQUESTOBJECT_H

#include "shvevent.h"
#include "shveventsubscriberbase.h"


//-=========================================================================================================
/// SHVEventRequestObject class - Request subscribers to perform something
/**
 * Emit an event with this object if you want unknown subscribers to perform
 * something for you. When the event is finished and SHVEventRequestObject is
 * destroyed the caller will receive an event of the same type with the result.\n
 * Example:
\code
typedef SHVRefObjectTemplate<int> SHVIntObj;

void MyModuleEmitter::EmitDoStuff()
{
SHVIntObj* result = new SHVIntObj();
SHVEventRef event = SHVEventRequestObject<SHVIntObj>::CreateEvent(Modules,this,EventDoStuff,new SHVEventSubscriber(this),Modules.CreateTag(),result);
	EmitEvent(event);
}

void MyModuleEmitter::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == this)
	{
		if (SHVEvent::Equals(event,MyModuleEmitter::EventDoStuff)
		{
		SHVEventRequestObject<SHVIntObj>* reqObject = SHVEventRequestObject<SHVIntObj>::FromEvent(event);
		
			// Process the result
			SHVConsole::Printf8("Result from DoStuff: %d\n", (int)reqObject->GetObject()->Object());
		}
	}
}

void MyModuleReceiver::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == ModuleEmitter)
	{
		if (SHVEvent::Equals(event,MyModuleEmitter::EventDoStuff)
		{
		SHVEventRequestObject<SHVIntObj>* reqObject = SHVEventRequestObject<SHVIntObj>::FromEvent(event);
		
			// Set the result
			reqObject->GetObject()->Object() = 1234;
		}
	}
}
\endcode
 */

template<class T = SHVRefObject>
class SHVEventRequestObject : public SHVRefObject
{
public:


	inline SHVInt GetTag();
	inline T* GetObject();

	
	static inline SHVEvent* CreateEvent(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVEventSubscriberBase* callback, SHVInt tag, T* obj = NULL);
	static inline SHVEvent* CreateEvent(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVInt callbackId, SHVEventSubscriberBase* callback, SHVInt tag, T* obj = NULL);
	static inline SHVEventRequestObject<T>* FromEvent(SHVEvent* event);


protected:
	SHVEventRequestObject(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVEventSubscriberBase* callback, SHVInt tag, T* obj = NULL);
	virtual ~SHVEventRequestObject();
	
	///\cond INTERNAL
	SHVModuleList& Modules;
	SHVEventRef CallbackEvent;
	SHVEventSubscriberBaseRef Callback;
	///\endcond
};


// ============================================= implementation  ============================================ //
/// \class SHVEventRequestObject shveventrequestobject.h "shiva/include/framework/shveventrequestobject.h"


/*************************************
  * Constructor
 *************************************/
template<class T>
SHVEventRequestObject<T>::SHVEventRequestObject(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVEventSubscriberBase* callback, SHVInt tag, T* obj)
	: Modules(mdl), Callback(callback)
{
	CallbackEvent = new SHVEvent(caller,id,tag,obj);
}

/*************************************
  * Destructor
 *************************************/
template<class T>
SHVEventRequestObject<T>::~SHVEventRequestObject()
{
	Callback->EmitNow(Modules,CallbackEvent);
}

/*************************************
  * GetTag
 *************************************/
template<class T>
SHVInt SHVEventRequestObject<T>::GetTag()
{
	return CallbackEvent->GetSubID();
}

/*************************************
  * GetObject
 *************************************/
template<class T>
T* SHVEventRequestObject<T>::GetObject()
{
	return (T*)CallbackEvent->GetObject();
}

/*************************************
  * CreateEvent
 *************************************/
template<class T>
SHVEvent* SHVEventRequestObject<T>::CreateEvent(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVEventSubscriberBase* callback, SHVInt tag, T* obj)
{
SHVEventRequestObject<T>* reqObj = new SHVEventRequestObject<T>(mdl,caller,id,callback,tag,obj);
	return new SHVEvent(caller, id, tag, reqObj);
}
template<class T>
SHVEvent* SHVEventRequestObject<T>::CreateEvent(SHVModuleList& mdl, SHVEventTarget* caller, SHVInt id, SHVInt callbackId, SHVEventSubscriberBase* callback, SHVInt tag, T* obj)
{
SHVEventRequestObject<T>* reqObj = new SHVEventRequestObject<T>(mdl,caller,callbackId,callback,tag,obj);
	return new SHVEvent(caller, id, tag, reqObj);
}

/*************************************
  * FromEvent
 *************************************/
template<class T>
SHVEventRequestObject<T>* SHVEventRequestObject<T>::FromEvent(SHVEvent* event)
{
	return (SHVEventRequestObject<T>*)event->GetObject();
}

#endif
