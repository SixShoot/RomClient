#if !defined(AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_)
#define AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIButton.h"


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CToolbarUI : public CHorizontalLayoutUI
{
public:
   CToolbarUI(SquirrelObject&so);

   LPCTSTR GetClass() const;
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CToolbarTitlePanelUI : public CControlUI
{
public:
   CToolbarTitlePanelUI(SquirrelObject&so);

   void SetPadding(int iPadding);

   LPCTSTR GetClass() const;   
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   int m_iPadding;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CToolSeparatorUI : public CControlUI
{
public:
	CToolSeparatorUI(SquirrelObject&so);
   LPCTSTR GetClass() const;
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CToolGripperUI : public CControlUI
{
public:
	CToolGripperUI(SquirrelObject&so);
   LPCTSTR GetClass() const;
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CToolButtonUI : public CButtonUI
{
public:
   CToolButtonUI(SquirrelObject&so);

   LPCTSTR GetClass() const;

   void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStatusbarUI : public CContainerUI
{
public:

	CStatusbarUI(SquirrelObject&so);
   LPCTSTR GetClass() const;

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};




#endif // !defined(AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_)

