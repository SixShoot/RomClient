#include "../../netwaker_member/NetWakerClientInc.h"
#include "BulletinBoard.h"

CBulletinBoard*				g_pBulletinBoard = NULL;

int LuaFunc_NotifyBulletinBoard(lua_State* L);
int	LuaFunc_GetBulletinBoardNumItems(lua_State* L);
int	LuaFunc_GetBulletinBoardHeaderInfo(lua_State* L);
int	LuaFunc_GetBulletinBoardContent(lua_State* L);
int	LuaFunc_GetBulletinBoardRewardMoney(lua_State* L);
int	LuaFunc_GetBulletinBoardNumRewards(lua_State* L);
int	LuaFunc_GetBulletinBoardRewardInfo(lua_State* L);
int LuaFunc_HasNewBulletinBoard(lua_State* L);

// ----------------------------------------------------------------------------
CBulletinBoard::CBulletinBoard(CInterface* object)
	: CInterfaceBase(object)
{
	g_pBulletinBoard = this;
}

// ----------------------------------------------------------------------------
CBulletinBoard::~CBulletinBoard()
{
}

// ----------------------------------------------------------------------------
void CBulletinBoard::RegisterFunc()
{
	lua_State* L = GetLuaState();
	if ( L )
	{
		lua_register(L, "NotifyBulletinBoard",				LuaFunc_NotifyBulletinBoard);
		lua_register(L, "GetBulletinBoardNumItems",			LuaFunc_GetBulletinBoardNumItems);
		lua_register(L, "GetBulletinBoardHeaderInfo",		LuaFunc_GetBulletinBoardHeaderInfo);
		lua_register(L, "GetBulletinBoardContent",			LuaFunc_GetBulletinBoardContent);
		lua_register(L, "GetBulletinBoardRewardMoney",		LuaFunc_GetBulletinBoardRewardMoney);
		lua_register(L, "GetBulletinBoardNumRewards",		LuaFunc_GetBulletinBoardNumRewards);
		lua_register(L, "GetBulletinBoardRewardInfo",		LuaFunc_GetBulletinBoardRewardInfo);
		lua_register(L, "HasNewBulletinBoard",				LuaFunc_HasNewBulletinBoard);
	}
}

// ----------------------------------------------------------------------------
void CBulletinBoard::DestroyFrames()
{
}

// ----------------------------------------------------------------------------
void CBulletinBoard::Save()
{
	if ( m_interface == NULL || m_interface->IsLoadVariables() == false )
		return;

	char globalName[MAX_PATH];
	char localName[MAX_PATH];
	FILE* fptr = NULL;	

	GetGlobalPath("BulletinBoard.bsd", globalName, MAX_PATH);
	GetLocalPath("BulletinBoard.bsd", localName, MAX_PATH);

	// ?x?s???a????
	if ( (fptr = fopen(globalName, "wb")) != NULL )
	{
		// ?x?s???????s????
		fwrite(&NetCli_ImportBoardChild::s_importBoardLastUpdateTime, sizeof(float), 1, fptr);

		// ????????
		fclose(fptr);
	}
}

// ----------------------------------------------------------------------------
void CBulletinBoard::Load()
{
	char globalName[MAX_PATH];
	char localName[MAX_PATH];
	FILE* fptr = NULL;	
	
	GetGlobalPath("BulletinBoard.bsd", globalName, MAX_PATH);
	GetLocalPath("BulletinBoard.bsd", localName, MAX_PATH);	

	// ???????a????
	if ( (fptr = fopen(globalName, "rb")) != NULL )
	{
		// ????????
		fread(&NetCli_ImportBoardChild::s_importBoardLastUpdateTime, sizeof(float), 1, fptr);

		// ????????
		fclose(fptr);
	}
}

// ----------------------------------------------------------------------------
int LuaFunc_NotifyBulletinBoard(lua_State* L)
{
	// ???s???o????
	if ( NetCli_ImportBoardChild::s_importBoardUpdateTime == 0.0f ||
		 NetCli_ImportBoardChild::s_importBoardUpdateTime != NetCli_ImportBoardChild::s_importBoardNewVersion ) {
		// ???????o???T
		NetCli_ImportBoard::SL_ImportBoardRequest(NetCli_ImportBoardChild::s_importBoardUpdateTime);
	} else {
		g_pInterface->SendWorldEvent("BULLETINBOARD_UPDATE");
	}
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetBulletinBoardNumItems(lua_State* L)
{
	lua_pushnumber(L, (lua_Number)NetCli_ImportBoardChild::s_importBoardList.size());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetBulletinBoardHeaderInfo(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;

	vector<ImportBoardBaseStruct>& data = NetCli_ImportBoardChild::s_importBoardList;
	if ( index >= 0 && index < (int)data.size() ) {
		lua_pushstring(L, data[index].Title);			// ?????W??
		lua_pushboolean(L, data[index].IsHot);			// ????????
		lua_pushboolean(L, data[index].IsRed);			// ???r???D
		return 3;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetBulletinBoardContent(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;

	vector<ImportBoardBaseStruct>& data = NetCli_ImportBoardChild::s_importBoardList;
	if ( index >= 0 && index < (int)data.size() ) {
		lua_pushstring(L, data[index].Content);			// ???T???e	
		return 1;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetBulletinBoardRewardMoney(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;

	int money = 0, moneyAccount = 0;
	vector<ImportBoardBaseStruct>& data = NetCli_ImportBoardChild::s_importBoardList;
	if ( index >= 0 && index < (int)data.size() ) {
		money = data[index].Money;
		moneyAccount = data[index].Money_Account;
	}
	lua_pushnumber(L, data[index].Money);			// ????
	lua_pushnumber(L, data[index].Money_Account);	// ??????
	return 2;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetBulletinBoardNumRewards(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;
	int count  = 0;

	vector<ImportBoardBaseStruct>& data = NetCli_ImportBoardChild::s_importBoardList;
	if ( index >= 0 && index < (int)data.size() )
	{
		while ( data[index].Item[count] > 0 )
			count++;
	}
	lua_pushnumber(L, count);			// ???~???q
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetBulletinBoardRewardInfo(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;

	vector<ImportBoardBaseStruct>& data = NetCli_ImportBoardChild::s_importBoardList;
	if ( index >= 0 && index < (int)data.size() )
	{
		int pos = luaL_checkint(L, 2) - 1;
		if ( pos >= 0 && pos < 5 )
		{
			GameObjDbStructEx* itemDB = g_ObjectData->GetObj(data[index].Item[pos]);
			if ( itemDB )
			{
				GameObjDbStructEx* imageDB = g_ObjectData->GetObj(itemDB->ImageID);
				if ( imageDB )
				{
					lua_pushstring(L, imageDB->Image.ACTField);			// ????????
					lua_pushstring(L, itemDB->GetName());					// ???~?W??
					lua_pushnumber(L, data[index].ItemCount[pos]);		// ???|???q
					return 3;
				}
			}
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_HasNewBulletinBoard(lua_State* L)
{
	lua_pushboolean(L, NetCli_ImportBoardChild::s_importBoardLastUpdateTime != NetCli_ImportBoardChild::s_importBoardNewVersion);
	return 1;
}
	