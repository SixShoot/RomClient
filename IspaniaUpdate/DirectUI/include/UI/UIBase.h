#if !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
#define AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_

#pragma once


/////////////////////////////////////////////////////////////////////////////////////
//

class CPaintManagerUI;


#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_LAUNCHER_FRAME (WS_POPUP| WS_VISIBLE  )
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


/////////////////////////////////////////////////////////////////////////////////////
//

#define ASSERT(expr)  _ASSERTE(expr)

#ifdef _DEBUG
   #define TRACE __Trace
   #define TRACEMSG __TraceMsg
#else
   #define TRACE
   #define TRACEMSG _T("")
#endif

void UILIB_API __Trace(LPCTSTR pstrFormat, ...);
LPCTSTR __TraceMsg(UINT uMsg);


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRect : public tagRECT
{
public:
   CRect();
   CRect(const RECT& src);
   CRect(int iLeft, int iTop, int iRight, int iBottom);

   int GetWidth() const;
   int GetHeight() const;
   void Empty();
   void Join(const RECT& rc);
   void ResetOffset();
   void Normalize();
   void Offset(int cx, int cy);
   void Inflate(int cx, int cy);
   void Deflate(int cx, int cy);
   void Union(CRect& rc);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CSize : public tagSIZE
{
public:
   CSize();
   CSize(const SIZE& src);
   CSize(const RECT rc);
   CSize(int cx, int cy);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CPoint : public tagPOINT
{
public:
   CPoint();
   CPoint(const POINT& src);
   CPoint(int x, int y);
   CPoint(LPARAM lParam);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStdPtrArray
{
public:
   CStdPtrArray(int iPreallocSize = 0);
   virtual ~CStdPtrArray();

   void Empty();
   void Resize(int iSize);
   bool IsEmpty() const;
   int Find(LPVOID iIndex) const;
   bool add(LPVOID pData);
   bool SetAt(int iIndex, LPVOID pData);
   bool InsertAt(int iIndex, LPVOID pData);
   bool remove(int iIndex);
   int size() const;
   LPVOID* GetData();

   LPVOID GetAt(int iIndex) const;
   LPVOID operator[] (int nIndex) const;

protected:
   LPVOID* m_ppVoid;
   int m_nCount;
   int m_nAllocated;
};



/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStdValArray
{
public:
   CStdValArray(int iElementSize, int iPreallocSize = 0);
   virtual ~CStdValArray();

   void Empty();
   bool IsEmpty() const;
   bool Add(LPCVOID pData);
   bool Remove(int iIndex);
   int size() const;
   LPVOID GetData();

   LPVOID GetAt(int iIndex) const;
   LPVOID operator[] (int nIndex) const;

protected:
   LPBYTE m_pVoid;
   int m_iElementSize;
   int m_nCount;
   int m_nAllocated;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWaitCursor
{
public:
   CWaitCursor();
   ~CWaitCursor();

protected:
   HCURSOR m_hOrigCursor;
};
void ProcessResourceTokens(base::String&v);

/////////////////////////////////////////////////////////////////////////////////////
//
/*
class UILIB_API base::String
{
public:
   enum { MAX_LOCAL_STRING_LEN = 63 };

   base::String();
   base::String(const TCHAR ch);
   base::String(const base::String& src);
   base::String(LPCTSTR lpsz, int nLen = -1);
   virtual ~base::String();

   static base::String RES(UINT nRes);

   void Empty();
   int GetLength() const;
   bool IsEmpty() const;
   TCHAR GetAt(int nIndex) const;
   void Append(LPCTSTR pstr);
   void Assign(LPCTSTR pstr, int nLength = -1);
   LPCTSTR GetData();
   
   void SetAt(int nIndex, TCHAR ch);
   operator LPCTSTR() const;

   TCHAR operator[] (int nIndex) const;
   const base::String& operator=(const base::String& src);
   const base::String& operator=(const TCHAR ch);
   const base::String& operator=(LPCTSTR pstr);
#ifndef _UNICODE
   const base::String& base::String::operator=(LPCWSTR lpwStr);
#endif
   base::String operator+(const base::String& src);
   base::String operator+(LPCTSTR pstr);
   const base::String& operator+=(const base::String& src);
   const base::String& operator+=(LPCTSTR pstr);
   const base::String& operator+=(const TCHAR ch);

   bool operator == (LPCTSTR str) const;
   bool operator != (LPCTSTR str) const;
   bool operator <= (LPCTSTR str) const;
   bool operator <  (LPCTSTR str) const;
   bool operator >= (LPCTSTR str) const;
   bool operator >  (LPCTSTR str) const;

   int Compare(LPCTSTR pstr) const;
   int CompareNoCase(LPCTSTR pstr) const;
   
   void MakeUpper();
   void MakeLower();

   base::String Left(int nLength) const;
   base::String Mid(int iPos, int nLength = -1) const;
   base::String Right(int nLength) const;

   int Find(TCHAR ch, int iPos = 0) const;
   int Find(LPCTSTR pstr, int iPos = 0) const;
   int ReverseFind(TCHAR ch) const;
   int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
   
  
   int __cdecl Format(LPCTSTR pstrFormat, ...);

protected:
   LPTSTR m_pstr;
   TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};
*/



#endif // !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
