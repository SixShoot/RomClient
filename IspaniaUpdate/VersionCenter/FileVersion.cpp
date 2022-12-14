#include "stdafx.h"
#include "FileVersion.h"

#pragma comment(lib, "version")

//////////////////////////////////////////////////////////////////////
/*
 $History: FileVersion.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Navis        Date: 6/18/99    Time: 3:33p
 * Updated in $/PJ
 * Standardized history/include format.
 * Implemented Insert Template Wizard.
 * Working on Template Editor
 * 
 * *****************  Version 2  *****************
 * User: Navis        Date: 4/19/99    Time: 3:29p
 * Updated in $/PJ
 * class to obtain module version info.  Intend to use in About Dialog
 * box.
*/
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// by Manuel Laflamme from CodeGuru site.

CFileVersion::CFileVersion() 
{ 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
}

CFileVersion::~CFileVersion() 
{ 
    Close();
} 

void CFileVersion::Close()
{
    delete[] m_lpVersionData; 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
}

BOOL CFileVersion::Open(LPCTSTR lpszModuleName)
{
    ASSERT(_tcslen(lpszModuleName) > 0);
    ASSERT(m_lpVersionData == NULL);

    // Get the version information size for allocate the buffer
    DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize((LPTSTR)lpszModuleName, &dwHandle); 
    if ( dwDataSize == 0 ) 
        return FALSE;

    // Allocate buffer and retrieve version information
    m_lpVersionData = new BYTE[dwDataSize]; 
    if (!::GetFileVersionInfo((LPTSTR)lpszModuleName, dwHandle, dwDataSize, 
	                          (void**)m_lpVersionData) )
    {
        Close();
        return FALSE;
    }

    // Retrieve the first language and character-set identifier
    UINT nQuerySize;
    DWORD* pTransTable;
    if (!::VerQueryValue(m_lpVersionData, _T("\\VarFileInfo\\Translation"),
                         (void **)&pTransTable, &nQuerySize) )
    {
        Close();
        return FALSE;
    }

    // Swap the words to have lang-charset in the correct format
    m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

    return TRUE;
}

base::String CFileVersion::QueryValue(LPCTSTR lpszValueName, 
                                 DWORD dwLangCharset /* = 0*/)
{
    // Must call Open() first
    ASSERT(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return (base::String)_T("");

    // If no lang-charset specified use default
    if ( dwLangCharset == 0 )
        dwLangCharset = m_dwLangCharset;

    // Query version information value
    UINT nQuerySize;
    LPVOID lpData;
    base::String strValue;
	TCHAR strBlockName[512];
	wsprintf(strBlockName,_T("\\StringFileInfo\\%08lx\\%s"), 
		dwLangCharset, lpszValueName);
   
    if ( ::VerQueryValue((void **)m_lpVersionData, strBlockName, 
	                     &lpData, &nQuerySize) )
        strValue = (LPCTSTR)lpData;

  

    return strValue;
}

BOOL CFileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
{
    // Must call Open() first
    ASSERT(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return FALSE;

    UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
    if ( ::VerQueryValue((void **)m_lpVersionData, _T("\\"),
                         (void**)&pVsffi, &nQuerySize) )
    {
        vsffi = *pVsffi;
        return TRUE;
    }

    return FALSE;
}

base::String CFileVersion::GetFixedFileVersion()
{
    base::String sVersion;
	VS_FIXEDFILEINFO vsffi;


    if ( GetFixedInfo(vsffi) )
    {
		TCHAR strBlockName[512];

       wsprintf(strBlockName,_T("%u_%u_%u_%u"),
		   HIWORD(vsffi.dwFileVersionMS),
            LOWORD(vsffi.dwFileVersionMS),
            HIWORD(vsffi.dwFileVersionLS),
            LOWORD(vsffi.dwFileVersionLS));
	 sVersion=  strBlockName;
    }
    return sVersion;
}

base::String CFileVersion::GetFixedProductVersion()
{
    base::String sVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {TCHAR strBlockName[512];
        wsprintf(strBlockName,_T("%u,%u,%u,%u"),
			HIWORD(vsffi.dwProductVersionMS),
            LOWORD(vsffi.dwProductVersionMS),
            HIWORD(vsffi.dwProductVersionLS),
            LOWORD(vsffi.dwProductVersionLS));
		 sVersion=  strBlockName;
    }
    return sVersion;
}

