#include "../GameMain.h"
#include ".\UI_Quest.h"


// ����ŧi
//-------------------------------------------------------------------
CUI_Quest			*g_pUi_Quest = NULL;

//-------------------------------------------------------------------
CUI_Quest::CUI_Quest(void)
{

}
// --------------------------------------------------------------------------------------
CUI_Quest::~CUI_Quest(void)
{

}
// --------------------------------------------------------------------------------------
void CUI_Quest::Initial()
{
	if ( g_pUi_Quest == NULL )
	{
		g_pUi_Quest = new CUI_Quest;
	}
}
// --------------------------------------------------------------------------------------
void CUI_Quest::Release()
{
	SAFE_DELETE( g_pUi_Quest );	
}
// --------------------------------------------------------------------------------------