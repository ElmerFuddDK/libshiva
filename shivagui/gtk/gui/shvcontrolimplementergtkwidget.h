#ifndef __SHIVA_GTKGUI_CONTROLIMPLEMENTERWIDGET_H
#define __SHIVA_GTKGUI_CONTROLIMPLEMENTERWIDGET_H


#include "../../../include/gui/shvcontrolimplementer.h"
#include "shvcontrolimplementergtkwidgetbase.h"



//-=========================================================================================================
/// SHVControlImplementerGtkWidget - template class for implementing implementers in win32
/**
 */

template <class T>
class SHVControlImplementerGtkWidget : public T, public SHVControlImplementerGtkWidgetBase
{
public:


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool IsCreated();
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags) = 0;
	virtual SHVBool Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags);
	virtual SHVBool Destroy(SHVControl* owner);

	virtual SHVRect GetRect(SHVControl* owner);
	virtual void SetRect(SHVControl* owner, const SHVRect& rect);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVFont* GetFont(SHVControl* owner);
	virtual SHVBool SetFont(SHVControl* owner, SHVFont* font, bool resetHeight);

	virtual void* GetNative();

	virtual int CalculateNewHeight(SHVControl* owner, SHVFont* font);

protected:

	SHVControlImplementerGtkWidget();

};


// ============================================ implementation ============================================ //


/*************************************
 * Constructor
 *************************************/
template <class T>
SHVControlImplementerGtkWidget<T>::SHVControlImplementerGtkWidget() : SHVControlImplementerGtkWidgetBase()
{

}

/*************************************
 * GetSubType
 *************************************/
template <class T>
int SHVControlImplementerGtkWidget<T>::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * IsCreated
 *************************************/
template <class T>
SHVBool SHVControlImplementerGtkWidget<T>::IsCreated()
{
	return SHVControlImplementerGtkWidgetBase::IsCreated();
}

/*************************************
 * Reparent
 *************************************/
template <class T>
SHVBool SHVControlImplementerGtkWidget<T>::Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVControlImplementerGtkWidgetBase::Reparent(owner, parent, flags);
}

/*************************************
 * Destroy
 *************************************/
template <class T>
SHVBool SHVControlImplementerGtkWidget<T>::Destroy(SHVControl* owner)
{
	return SHVControlImplementerGtkWidgetBase::Destroy(owner);
}

/*************************************
 * GetRect
 *************************************/
template <class T>
SHVRect SHVControlImplementerGtkWidget<T>::GetRect(SHVControl* owner)
{
	return SHVControlImplementerGtkWidgetBase::GetRect(owner);
}

/*************************************
 * SetRect
 *************************************/
template <class T>
void SHVControlImplementerGtkWidget<T>::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVControlImplementerGtkWidgetBase::SetRect(owner, rect);
}

/*************************************
 * SetFlag
 *************************************/
template <class T>
SHVBool SHVControlImplementerGtkWidget<T>::SetFlag(SHVControl* owner, int flag, bool enable)
{
	return SHVControlImplementerGtkWidgetBase::SetFlag(owner, flag, enable);
}

/*************************************
 * GetFlag
 *************************************/
template <class T>
bool SHVControlImplementerGtkWidget<T>::GetFlag(SHVControl* owner, int flag)
{
	return SHVControlImplementerGtkWidgetBase::GetFlag(owner, flag);
}

/*************************************
 * GetFont
 *************************************/
template <class T>
SHVFont* SHVControlImplementerGtkWidget<T>::GetFont(SHVControl* owner)
{
	return SHVControlImplementerGtkWidgetBase::GetFont(owner);
}

/*************************************
 * SetFont
 *************************************/
template <class T>
SHVBool SHVControlImplementerGtkWidget<T>::SetFont(SHVControl* owner, SHVFont* font, bool resetHeight)
{
SHVInt newHeight;
	if (resetHeight)
		newHeight = CalculateNewHeight(owner,font);
	return SHVControlImplementerGtkWidgetBase::SetFont(owner, font, newHeight);
}

/*************************************
 * GetNative
 *************************************/
template <class T>
void* SHVControlImplementerGtkWidget<T>::GetNative()
{
	return GetHandle();
}

/*************************************
 * CalculateNewHeight
 *************************************/
template <class T>
int SHVControlImplementerGtkWidget<T>::CalculateNewHeight(SHVControl* owner, SHVFont* font)
{
	return (font->GetCellHeight()*145)/100;
}

#endif
