#include <windows.h>
#include "DebugMessage.h"

CDebugMessage*				g_pDebugMessage = NULL;

int LuaFunc_Debug			( lua_State *L );
int LuaFunc_Debug_GetButton	( lua_State *L );
int LuaFunc_Debug_GetFont	( lua_State *L );
int LuaFunc_Debug_GetString	( lua_State *L );
int LuaFunc_Debug_GetNumber	( lua_State *L );
// ----------------------------------------------------------------------------
CDebugMessage::CDebugMessage(CInterface* object)
	: CInterfaceBase(object)
{
	g_pDebugMessage = this;
}

// ----------------------------------------------------------------------------
CDebugMessage::~CDebugMessage()
{
	g_pDebugMessage = NULL;
}

// ----------------------------------------------------------------------------
void CDebugMessage::RegisterFunc()
{
	lua_State* l = GetLuaStateWorld();
	if (l)
	{
		lua_register( l, "Debug",					LuaFunc_Debug				);
		lua_register( l, "DebugGetButton",			LuaFunc_Debug_GetButton		);
		lua_register( l, "DebugGetFont",			LuaFunc_Debug_GetFont		);
		lua_register( l, "DebugGetString",			LuaFunc_Debug_GetString		);
		lua_register( l, "DebugGetNumber",			LuaFunc_Debug_GetNumber		);
	}
}

// ----------------------------------------------------------------------------
void CDebugMessage::DebugOutput(int type, const char* format, ...)
{
	static char buf[4096];
	int color;

	va_list args;
	va_start( args, format );
	vsprintf( buf, format, args );
	va_end( args);

	switch ( type )
	{
	case 0:
		color = 0xFFFF1010;
		break;

	case 1:
		color = 0xFFFFFF20;
		break;

	case 2:
		color = 0xFF10FF08;
		break;

	case 3:
		color = 0xFF1010F0;
		break;

	case 4:
		color = 0xFF10EFF0;
		break;

	case 5:
		color = 0xFFDF20FE;
		break;

	default:
		color = 0xFFEFF0F1;
		break;
	}

	CUiMessageFrame* messageFrame = (CUiMessageFrame*)m_interface->GetUi()->FindObject("DebugMessageFrame");
	if (messageFrame)
	{
		WCHAR* tmp = TransToWChar(buf);
		if (tmp)
		{
			messageFrame->AddMessage(tmp, color);
			delete [] tmp;
		}
	}
}
// --------------------------------------------------------------------------------------
int LuaFunc_Debug_GetString	( lua_State *L )
{
	const char* password = luaL_checkstring(L, 1);
	return 0;
}
// --------------------------------------------------------------------------------------
int LuaFunc_Debug_GetNumber	( lua_State *L )
{
	int iNumber = luaL_checkint(L, 1);
	return 0;
}
// --------------------------------------------------------------------------------------
int LuaFunc_Debug_GetFont	( lua_State *L )
{
	CUiFontString* pFont = dynamic_cast<CUiFontString*>( CUiLuaLayout::CheckLayoutTable(L, 1) );
	return 0;
}
// --------------------------------------------------------------------------------------
int LuaFunc_Debug_GetButton	( lua_State *L )
{
	CUiButton* pButton = dynamic_cast<CUiButton*>( CUiLuaLayout::CheckLayoutTable(L, 1) );
	return 0;
}
// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int LuaFunc_Debug			( lua_State *L )
{
	CUiLayout* layout = CUiLuaLayout::CheckLayoutTable(L, 1);

	switch(lua_type(L, 1))
	{
	case LUA_TTABLE:
		{
			CUiLayout* layout = CUiLuaLayout::CheckLayoutTable(L, 1);
		} break;

	case LUA_TSTRING:
		{
			UiOuputError( luaL_checkstring(L, 1) );
		} break;
	}	
	return 0;

}