#ifndef __SHIVA_WIN32GUI_CONTROLIMPLEMENTER_H
#define __SHIVA_WIN32GUI_CONTROLIMPLEMENTER_H


#include "../../../include/gui/shvcontrolimplementer.h"
#include "shvcontrolimplementerwin32base.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif

#if defined(__SHIVA_WINCE) && !defined(MulDiv)
# define MulDiv(num,numerator,denom) (num*numerator)/denom
#endif


//-=========================================================================================================
/// SHVControlImplementerWin32 - template class for implementing implementers in win32
/**
 */

template <class T>
class SHVControlImplementerWin32 : public T, public SHVControlImplementerWin32Base
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

	SHVControlImplementerWin32();

};


// ============================================ implementation ============================================ //


/*************************************
 * Constructor
 *************************************/
template <class T>
SHVControlImplementerWin32<T>::SHVControlImplementerWin32() : SHVControlImplementerWin32Base()
{

}

/*************************************
 * GetSubType
 *************************************/
template <class T>
int SHVControlImplementerWin32<T>::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * IsCreated
 *************************************/
template <class T>
SHVBool SHVControlImplementerWin32<T>::IsCreated()
{
	return SHVControlImplementerWin32Base::IsCreated();
}

/*************************************
 * Reparent
 *************************************/
template <class T>
SHVBool SHVControlImplementerWin32<T>::Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVControlImplementerWin32Base::Reparent(owner, parent, flags);
}

/*************************************
 * Destroy
 *************************************/
template <class T>
SHVBool SHVControlImplementerWin32<T>::Destroy(SHVControl* owner)
{
	return SHVControlImplementerWin32Base::Destroy(owner);
}

/*************************************
 * GetRect
 *************************************/
template <class T>
SHVRect SHVControlImplementerWin32<T>::GetRect(SHVControl* owner)
{
	return SHVControlImplementerWin32Base::GetRect(owner);
}

/*************************************
 * SetRect
 *************************************/
template <class T>
void SHVControlImplementerWin32<T>::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVControlImplementerWin32Base::SetRect(owner, rect);
}

/*************************************
 * SetFlag
 *************************************/
template <class T>
SHVBool SHVControlImplementerWin32<T>::SetFlag(SHVControl* owner, int flag, bool enable)
{
	return SHVControlImplementerWin32Base::SetFlag(owner, flag, enable);
}

/*************************************
 * GetFlag
 *************************************/
template <class T>
bool SHVControlImplementerWin32<T>::GetFlag(SHVControl* owner, int flag)
{
	return SHVControlImplementerWin32Base::GetFlag(owner, flag);
}

/*************************************
 * GetFont
 *************************************/
template <class T>
SHVFont* SHVControlImplementerWin32<T>::GetFont(SHVControl* owner)
{
	return SHVControlImplementerWin32Base::GetFont(owner);
}

/*************************************
 * SetFont
 *************************************/
template <class T>
SHVBool SHVControlImplementerWin32<T>::SetFont(SHVControl* owner, SHVFont* font, bool resetHeight)
{
SHVInt newHeight;
	if (resetHeight)
		newHeight = CalculateNewHeight(owner,font);
	return SHVControlImplementerWin32Base::SetFont(owner, font, newHeight);
}

/*************************************
 * GetNative
 *************************************/
template <class T>
void* SHVControlImplementerWin32<T>::GetNative()
{
	return (SHVControlImplementerWin32Base*)this;
}

/*************************************
 * CalculateNewHeight
 *************************************/
template <class T>
int SHVControlImplementerWin32<T>::CalculateNewHeight(SHVControl* owner, SHVFont* font)
{
	return MulDiv(font->GetCellHeight(),145,100);
}

#endif
