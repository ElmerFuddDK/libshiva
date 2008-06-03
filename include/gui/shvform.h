#ifndef __SHIVA_GUI_FORM_H
#define __SHIVA_GUI_FORM_H


#include "shvformbase.h"
#include "shvformimplementer.h"
#include "shvcontrol.h"
#include "shvcontrolcontainer.h"
#include "shvguimanager.h"


//-=========================================================================================================
/// SHVForm - base form for coordinating 1..n ControlContainers/Controls
/**
 * This class is the basis of all forms in SHIVA.
 */

template <class T = SHVFormBase>
class SHVForm : public T
{
public:

	SHVForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);

	virtual void InitializeForm() = 0;

	virtual void Show();
	virtual void Hide();
	virtual void Enable();
	virtual void Disable();

	virtual SHVBool GetData(SHVDataBinder* data);
	virtual SHVBool SetData(SHVDataBinder* data);

	virtual SHVString8C GetEntityName();

	virtual SHVBool PreClose();

protected:

	inline SHVGUIManager* GetManager();
	inline SHVControlContainer* GetContainer();

	virtual void SetStandardLayoutEngine(); // sets a standard layout engine that calls onresizeform
	virtual void OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout);

	// From event target
	virtual void OnEvent(SHVEvent* event);

private:
	///\cond INTERNAL
	SHVFormImplementerPtr Base;
	///\endcond
};



// ============================================ implementation ============================================ //

/*************************************
 * Constructor
 *************************************/
template <class T>
SHVForm<T>::SHVForm(SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName)
{
	Base = manager->ConstructFormImplementer(this,manager,controlContainer,entityName);
}

/*************************************
 * SetStandardLayoutEngine
 *************************************/
template <class T>
void SHVForm<T>::SetStandardLayoutEngine()
{
	Base->GetContainer()->SetLayoutEngine(new SHVControlLayoutCallback< SHVForm<T> >(this, &SHVForm<T>::OnResizeForm));
}

/*************************************
 * Show
 *************************************/
template <class T>
void SHVForm<T>::Show()
{
	Base->Show();
}

/*************************************
 * Hide
 *************************************/
template <class T>
void SHVForm<T>::Hide()
{
	Base->Hide();
}

/*************************************
 * Enable
 *************************************/
template <class T>
void SHVForm<T>::Enable()
{
	Base->Enable();
}

/*************************************
 * Disable
 *************************************/
template <class T>
void SHVForm<T>::Disable()
{
	Base->Disable();
}

/*************************************
 * GetData
 *************************************/
template <class T>
SHVBool SHVForm<T>::GetData(SHVDataBinder* data)
{
	return Base->GetData(data);
}

/*************************************
 * SetData
 *************************************/
template <class T>
SHVBool SHVForm<T>::SetData(SHVDataBinder* data)
{
	return Base->SetData(data);
}

/*************************************
 * GetEntityName
 *************************************/
template <class T>
SHVString8C SHVForm<T>::GetEntityName()
{
	return Base->GetEntityName();
}

/*************************************
 * PreClose
 *************************************/
/// Called before the control container is closed
/** 
 * Return false to prevent the window/dialog from closing
 */
template <class T>
SHVBool SHVForm<T>::PreClose()
{
	return SHVBool::True;
}


/*************************************
 * OnEvent
 *************************************/
template <class T>
void SHVForm<T>::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

/*************************************
 * OnResizeForm
 *************************************/
template <class T>
void SHVForm<T>::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
	SHVUNUSED_PARAM(container);
	SHVUNUSED_PARAM(layout);
}

/*************************************
 * GetManager
 *************************************/
template <class T>
SHVGUIManager* SHVForm<T>::GetManager()
{
	return Base->GetManager();
}

/*************************************
 * GetContainer
 *************************************/
template <class T>
SHVControlContainer* SHVForm<T>::GetContainer()
{
	return Base->GetContainer();
}

#endif
