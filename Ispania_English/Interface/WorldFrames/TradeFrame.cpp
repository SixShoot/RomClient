#include "../../mainproc/GameMain.h"
#include "../../netwaker_member/NetWakerClientInc.h"
#include "ChatFrame.h"
#include "ItemClipboard.h"
#include "BagFrame.h"
#include "TradeFrame.h"
#include "ItemUtil.h"

#include "../../ErrorList/errorlist.h"

int LuaFunc_RequestTrade				(lua_State* L);
int LuaFunc_AgreeTrade					(lua_State* L);
int LuaFunc_DisagreeTrade				(lua_State* L);
int LuaFunc_GetTradePlayerItemInfo		(lua_State* L);
int LuaFunc_GetTradeTargetItemInfo		(lua_State* L);
int LuaFunc_SetTradeMoney				(lua_State* L);
int LuaFunc_GetTradePlayerMoney			(lua_State* L);
int LuaFunc_GetTradeTargetMoney			(lua_State* L);
int LuaFunc_ClickTradeItem				(lua_State* L);
int LuaFunc_AcceptTrade					(lua_State* L);
int LuaFunc_CancelTradeAccept			(lua_State* L);
int LuaFunc_CloseTrade					(lua_State* L);
int LuaFunc_GetTradeRecipientName		(lua_State* L);

// Debug print
int LuaFunc_PrintTradeState				(lua_State* L);

CTradeFrame*				g_pTradeFrame = NULL;

// ----------------------------------------------------------------------------
CTradeFrame::CTradeFrame(CInterface* object)
	: CInterfaceBase(object)
{
	m_target					= 0;
	m_moneyMode					= 0;
	m_money						= 0;	
	m_targetMoneyMode			= 0;
	m_targetMoney				= 0;
	m_time						= 0.0f;
	m_tradeState				= eTradeState_None;
	m_playerAcceptState			= eTradeAcceptState_None;
	m_targetAcceptState			= eTradeAcceptState_None;
	memset(m_items, 0, sizeof(m_items));

	g_pTradeFrame	= this;
}

// ----------------------------------------------------------------------------
CTradeFrame::~CTradeFrame()
{
	g_pTradeFrame = NULL;
}

// ----------------------------------------------------------------------------
void CTradeFrame::RegisterFunc()
{
	lua_State* L = GetLuaStateWorld();
	if (L)
	{
		lua_register(L, "RequestTrade",					LuaFunc_RequestTrade);
		lua_register(L, "AgreeTrade",					LuaFunc_AgreeTrade);
		lua_register(L, "DisagreeTrade",				LuaFunc_DisagreeTrade);
		lua_register(L, "GetTradePlayerItemInfo",		LuaFunc_GetTradePlayerItemInfo);
		lua_register(L, "GetTradeTargetItemInfo",		LuaFunc_GetTradeTargetItemInfo);
		lua_register(L, "SetTradeMoney",				LuaFunc_SetTradeMoney);	
		lua_register(L, "GetTradePlayerMoney",			LuaFunc_GetTradePlayerMoney);
		lua_register(L, "GetTradeTargetMoney",			LuaFunc_GetTradeTargetMoney);
		lua_register(L, "ClickTradeItem",				LuaFunc_ClickTradeItem);
		lua_register(L, "AcceptTrade",					LuaFunc_AcceptTrade);
		lua_register(L, "CancelTradeAccept",			LuaFunc_CancelTradeAccept);
		lua_register(L, "CloseTrade",					LuaFunc_CloseTrade);
		lua_register(L, "GetTradeRecipientName",		LuaFunc_GetTradeRecipientName);

		// 
		lua_register(L, "PrintTradeState",				LuaFunc_PrintTradeState);
	}
}

// ----------------------------------------------------------------------------
void CTradeFrame::Update(float elapsedTime)
{
	m_time += elapsedTime;
	if ( m_tradeState != eTradeState_None )
	{
		char temp[MAX_PATH];
		bool isCancel = false;
		CRoleSprite* player = g_pGameMain->GetPlayer();
		CRoleSprite* target = g_pGameMain->FindSpriteByID(m_target);
		if ( player == NULL || target == NULL || (player->GetPos() - target->GetPos()).Magnitude() > 128.0f )
			isCancel = true;

		switch (m_tradeState)
		{
		case eTradeState_Request_Made:
			if (isCancel || m_time > 120.0f)
			{
				// 取消交易邀請
				g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_REQUEST_CANCEL"), m_targetName.c_str());

				NetCli_Trade::S_CancelTardeRequest(m_target);
				SetProcess(eTradeState_None, 0);
			}
			break;

		case eTradeState_Request_Receive:
			if (isCancel || m_time > 120.0f )
			{
				// 拒�F交易
				g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_REQUEST_DISAGREE"), m_targetName.c_str());

				g_pChatFrame->SendMsgEvent(CHAT_MSG_SYSTEM, temp, "");
				SetProcess(eTradeState_Disagree, m_target);
			}
			break;

		case eTradeState_Agree:
		case eTradeState_Disagree:
			break;

		case eTradeState_Traded:
			if (isCancel)
			{
				// 通知SERVER
				NetCli_Trade::S_StopTrade(m_target);

				// 關閉界面
				TradeClose();
			}
			break;
		}
	}
}

// ----------------------------------------------------------------------------
int CTradeFrame::SetProcess(TradeState state, int target)
{
	lua_State* L = GetLuaStateWorld();
	CRoleSprite* player = g_pGameMain->GetPlayer();
	CRoleSprite* sprite = g_pGameMain->FindSpriteByID(target);
	const char* targetName = "";

	if ( sprite )
		targetName = sprite->GetName();

	switch(state)
	{
	case eTradeState_None:
		break;

	// 發出交易通知
	case eTradeState_Request_Made:
		if (m_tradeState != eTradeState_None && m_tradeState != eTradeState_Request_Made)
		{
			g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_REQUEST_ERROR1"));
			return -2;										// 未處於無狀態下,不能發出交易申請
		}
		else if (sprite == NULL)
		{
			g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_TARGET_NOTEXIST"));
			return -2;
		}
		else if ( g_pGameMain->IsDisableTrade() )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_TRADE_ERR_INBATTLEGROUND"));
			return -2;
		}
		else if ( g_pGameMain->IsBattleGroundZone() && sprite->GetEnemyState() )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_TRADE_ERR_ENEMY"));
			return -2;
		}
		else
		{
			if ( (player->GetPos() - sprite->GetPos()).Magnitude() > 64.0f )
			{
				g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_DISTANCE_ERROR"));
				return -2;
			}			
			else
			{
				g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_REQUESR"), targetName);
				NetCli_Trade::S_RequestTrade(target);
			}
		}			
		break;

	// 接收到對方要求交易
	case eTradeState_Request_Receive:
		if (m_tradeState != eTradeState_None)
		{
			NetCli_Trade::S_AgreeTrade(target, false, 1);
			return -2;										// 回應對方自己忙碌中
		}
		g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_INVITE"), targetName);
		
		// 送出訊問事件,是否願意與對交易
		lua_pushstring(L, targetName);
		lua_setglobal(L, "arg1");
		SendWorldEvent("TRADE_REQUEST");
		break;

	// 同意與對方交易
	case eTradeState_Agree:
		if (m_tradeState != eTradeState_Request_Receive)	
			return -2;										// 未被要求交易

		g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_INVITE_AGREE"), targetName);
		NetCli_Trade::S_AgreeTrade(target, true, 0);
		break;

	// 拒絕與對方交易
	case eTradeState_Disagree:
		if (m_tradeState != eTradeState_Request_Receive)	
			return -2;										// 未被要求交易	

		g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_TRADE_INVITE_DISAGREE"), targetName);
		NetCli_Trade::S_AgreeTrade(m_target, false, 0);
		state	= eTradeState_None;
		target	= 0;
		break;

	// 開啟雙方交易界面
	case eTradeState_Traded:
		if (m_tradeState != eTradeState_Request_Made && m_tradeState != eTradeState_Agree)
			return -1;										// 流程錯誤

		SendWorldEvent("TRADE_SHOW");
		break;

	default:
		return -1;
	}

	m_tradeState		= state;
	m_time				= 0.0f;
	m_target			= target;
	m_targetName		= targetName;
	m_moneyMode			= 0;
	m_money				= 0;
	m_targetMoneyMode	= 0;
	m_targetMoney		= 0;
	m_playerAcceptState	= eTradeAcceptState_None;
	m_targetAcceptState	= eTradeAcceptState_None;

	return 0;
}

// ----------------------------------------------------------------------------
// 關閉交易
void CTradeFrame::TradeClose()
{
	// g_pError->AddMessage(0 , 0, "Trade State[%d]", m_tradeState);

	// 狀態存在
	if ( m_tradeState != eTradeState_None ) {
		m_target			= 0;
		m_moneyMode			= 0;
		m_money				= 0;
		m_targetMoney		= 0;
		m_targetMoneyMode	= 0;
		m_time				= 0.0f;
		m_tradeState		= eTradeState_None;
		m_playerAcceptState	= eTradeAcceptState_None;
		m_targetAcceptState	= eTradeAcceptState_None;
		memset(m_items, 0, sizeof(m_items));

		// 清除物品鎖定狀態
		for ( int j = 0; j < CNetGlobal::RoleData()->PlayerBaseData->Body.Count; j++ )
		{
			ItemFieldStruct* item = CNetGlobal::RoleData()->GetBodyItem(j);
			if (item && item->Pos >= EM_TRADE_ITEM1 && item->Pos <= EM_TRADE_ITEM8 )
				item->Pos = EM_ItemState_NONE;
		}
		NetCli_TradeChild::ClearItem();

		// 通知錢更新
		SendWorldEvent("PLAYER_TRADE_MONEY");

		// 通知界面關閉
		SendWorldEvent("TRADE_CLOSED");
	}
}

// ----------------------------------------------------------------------------
void CTradeFrame::AcceptTrade(const char* password)
{
	switch (m_playerAcceptState)
	{
	case eTradeAcceptState_None:
		NetCli_Trade::S_ItemOK();
		break;

	case eTradeAcceptState_Ready:
		if ( m_targetAcceptState > 0 )
			NetCli_Trade::S_FinalOK(password);
		break;
	}		
}

// ----------------------------------------------------------------------------
void CTradeFrame::CancelTradeAccept()
{
	//TradeAcceptState(false, (m_tradeAcceptState & eTradeAcceptState_Target) != 0);
}

// ----------------------------------------------------------------------------
void CTradeFrame::TradeAcceptState(int player, int target)
{
	if ( m_playerAcceptState != player || m_targetAcceptState != target )
	{
		lua_State* L = GetLuaStateWorld();
		m_playerAcceptState = player;
		m_targetAcceptState = target;

		lua_pushnumber(L, m_playerAcceptState);
		lua_setglobal(L, "arg1");
		lua_pushnumber(L, m_targetAcceptState);
		lua_setglobal(L, "arg2");
		SendWorldEvent("TRADE_ACCEPT_UPDATE");
	}
}

// ----------------------------------------------------------------------------
TradeItem* CTradeFrame::GetItem(int index)
{
	if (index >= 0 && index < TRADE_ITEM_MAX_SIZE)
		return &m_items[index];
	return NULL;
}

// ----------------------------------------------------------------------------
void CTradeFrame::Cut(int index)
{
	RoleDataEx* role = CNetGlobal::RoleData();
	TradeItem* tradeItem = GetItem(index);

	if ( tradeItem )
	{
		ItemFieldStruct* item = NULL;
		switch( tradeItem->type )
		{
		case eTradeItemType_None:
			break;

		case eTradeItemType_Bag:
			if ( (item = CNetGlobal::RoleData()->GetBodyItem(tradeItem->pos)) != NULL )
			{
				NetCli_Trade::S_PutItem(EM_ItemState_NONE, tradeItem->pos, *item);
				g_pBagFrame->Cut(tradeItem->pos);
				
				/*
				ItemFieldStruct* item = role->GetBodyItem( tradeItem->pos );
				if ( item )
				{
					NetCli_Trade::S_PutItem(EM_ItemState_NONE, tradeItem->pos, *item);
					GameObjDbStruct* itemDB = CNetGlobal::GetObj( item->OrgObjID );
					if ( itemDB )
					{
						g_pItemClipboard->SetFile( itemDB->ImageObj->ACTField );
						g_pItemClipboard->PickupTrade( tradeItem->type, tradeItem->pos, tradeItem->id );
					}
				}
				*/
			}
			break;

		case eTradeItemType_Bank:
		case eTradeItemType_Equipment:			
			break;
		}
	}

	// 清除項目會由SERVER通知
}

// ----------------------------------------------------------------------------
void CTradeFrame::Paste(int index)
{	
	TradeItem* tradeItem = GetItem(index);

	if ( tradeItem )
	{
		ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
		ClipboardItemType itemType = g_pItemClipboard->GetType();

		if ( m_playerAcceptState != eTradeAcceptState_None )
			return;

		switch(itemType)
		{
		case eClipboardItem_Type_None:			// 拿取物品
			Cut(index);
			break;

		case eClipboardItem_Type_Bag:			// 背包 -> 交易
		case eClipboardItem_Type_Trade:			// 交易 -> 交易
			if ( tradeItem->type != eTradeItemType_None )
				Cut(index);
			else
				g_pItemClipboard->Clear();

			switch(itemType)
			{
			case eClipboardItem_Type_Bag:			// 背包 -> 交易
				PutItem(eTradeItemType_Bag, index, itemInfo.bag.pos);
				break;

			case eClipboardItem_Type_Trade:			// 交易 -> 交易
				PutItem(itemInfo.trade.type, index, itemInfo.trade.pos);
				break;
			}
			break;

			/*
		case CItemClipboard::eItem_Type_Bankpack:		// 銀行 -> 交易
			{
				ClipboardBankpackItem& bankpack = g_pItemClipboard->GetBankpackItem();
				if ( tradeItem->type != eTradeItemType_None )
					Cut(index);
				else
					g_pItemClipboard->Clear();

				tradeItem->type	= eTradeItemType_Bank;
				tradeItem->id	= bankpack.itemID;
				tradeItem->pos	= bankpack.pos;
			}
			break;

		case CItemClipboard::eItem_Type_Equipment:		// 裝備 -> 交易
			{
				ClipboardEquipmentItem equipment = g_pItemClipboard->GetEquipmentItem();
				if ( tradeItem->type != eTradeItemType_None )
					Cut(index);
				else
					g_pItemClipboard->Clear();

				tradeItem->type	= eTradeItemType_Equipment;
				tradeItem->id	= equipment.itemID;
				tradeItem->pos	= equipment.pos;
			}
			break;		

		case CItemClipboard::eItem_Type_Action:			// 快捷 -> 交易
			break;

		case CItemClipboard::eItem_Type_Shop:
			break;

		case CItemClipboard::eItem_Type_Trade:
			break;

		case CItemClipboard::eItem_Type_Skill:
			break;
			*/
		}
	}
}

// ----------------------------------------------------------------------------
void CTradeFrame::SetTradeMoney(int money, int money_Account )
{
	int moneyMode = 0;
	if ( money_Account != 0 )
	{
		moneyMode = 1;
		money = money_Account;
	}

	if ( m_money != money || m_moneyMode != moneyMode )
	{
		lua_State* L = GetLuaStateWorld();

		m_moneyMode = moneyMode;
		m_money = money;
		lua_pushnumber(L, m_money);
		lua_setglobal(L, "arg1");

		switch (m_moneyMode)
		{
		case 0:
			lua_pushstring(L, MONEY_MODE_COPPER);
			break;

		default:
			lua_pushstring(L, MONEY_MODE_ACCOUNT);			
			break;
		}
		lua_setglobal(L, "arg2");

		SendWorldEvent("PLAYER_TRADE_MONEY");
	}
}

// ----------------------------------------------------------------------------
void CTradeFrame::SetTradeTargetMoney(int money , int money_Account )
{
	lua_State* L = GetLuaStateWorld();

	int moneyMode = 0;
	if ( money_Account != 0 )
	{
		moneyMode = 1;
		money = money_Account;
	}

	m_targetMoney = money;
	m_targetMoneyMode = moneyMode;
	lua_pushnumber(L, m_targetMoney);
	lua_setglobal(L, "arg1");

	switch (m_targetMoneyMode)
	{
	case 0:
		lua_pushstring(L, MONEY_MODE_COPPER);
		break;

	default:
		lua_pushstring(L, MONEY_MODE_ACCOUNT);			
		break;
	}
	lua_setglobal(L, "arg2");

	SendWorldEvent("TRADE_MONEY_CHANGED");
}

// ----------------------------------------------------------------------------
void CTradeFrame::PutItem(int type, int index, int pos)
{
	ItemFieldStruct* item = NULL;

	switch (type)
	{
	case eTradeItemType_Bag:
		item = CNetGlobal::RoleData()->GetBodyItem(pos);
		break;

	case eTradeItemType_Bank:
		break;

	case eTradeItemType_Equipment:
		break;
	}

	if ( item )
	{
		switch (index)
		{
		case 0:
			item->Pos = EM_TRADE_ITEM1;
			break;
		case 1:
			item->Pos = EM_TRADE_ITEM2;
			break;
		case 2:
			item->Pos = EM_TRADE_ITEM3;
			break;
		case 3:
			item->Pos = EM_TRADE_ITEM4;
			break;
		case 4:
			item->Pos = EM_TRADE_ITEM5;
			break;
		case 5:
			item->Pos = EM_TRADE_ITEM6;
			break;
		case 6:
			item->Pos = EM_TRADE_ITEM7;
			break;
		case 7:
			item->Pos = EM_TRADE_ITEM8;
			break;
		}

		switch (type)
		{
		case eTradeItemType_Bag:
			NetCli_Trade::S_PutItem(item->Pos, pos, *item);
			break;

		case eTradeItemType_Bank:
			break;

		case eTradeItemType_Equipment:
			break;
		}		
	}
}

/*
// ----------------------------------------------------------------------------
void CTradeFrame::Clear()
{
	m_target		= 0;
	m_money			= 0;
	m_time			= 0.0f;
	m_tradeState	= eTradeState_None;

	RoleDataEx* role = CNetGlobal::RoleData();
	for ( int j = 0; j < role->PlayerBaseData->Body.Count; j++ )
	{
		ItemFieldStruct* item = role->GetBodyItem(j);
		if (item && item->Pos >= EM_TRADE_ITEM1 && item->Pos <= EM_TRADE_ITEM8 )
			item->Pos = EM_ItemState_NONE;
	}

	ClearItem();	
}

// ----------------------------------------------------------------------------
void CTradeFrame::ClearItem()
{
	memset(m_items, 0, sizeof(m_items));
}
*/

// ----------------------------------------------------------------------------
void CTradeFrame::Refresh()
{
	memset(m_items, 0, sizeof(m_items));
	for ( int j = 0; j < CNetGlobal::RoleData()->PlayerBaseData->Body.Count; j++ )
	{
		ItemFieldStruct* item = CNetGlobal::RoleData()->GetBodyItem(j);
		if (item && item->Pos >= EM_TRADE_ITEM1 && item->Pos <= EM_TRADE_ITEM8 )
		{
			TradeItem* tradeItem = GetItem(item->Pos - EM_TRADE_ITEM1);
			if ( tradeItem && tradeItem->type == eTradeItemType_None )
			{
				tradeItem->type		= eTradeItemType_Bag;
				tradeItem->id		= item->OrgObjID;
				tradeItem->pos		= j;
			}
		}
	}
}

// ----------------------------------------------------------------------------
int LuaFunc_RequestTrade(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetUnitSprite(luaL_checkstring(L,1));
	if ( sprite )
		g_pTradeFrame->SetProcess(eTradeState_Request_Made, sprite->GetIdentify());
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_AgreeTrade(lua_State* L)
{
	g_pTradeFrame->SetProcess(eTradeState_Agree, g_pTradeFrame->GetTradeTarget());
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_DisagreeTrade(lua_State* L)
{
	g_pTradeFrame->SetProcess(eTradeState_Disagree, g_pTradeFrame->GetTradeTarget());
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetTradePlayerItemInfo(lua_State* L)
{
	TradeItem* tradeItem = g_pTradeFrame->GetItem(luaL_checkint(L, 1) - 1);
	if ( tradeItem )
	{
		switch ( tradeItem->type )
		{
		case eTradeItemType_None:
			break;

		case eTradeItemType_Bag:
			{
				ItemFieldStruct* item = CNetGlobal::RoleData()->GetBodyItem(tradeItem->pos);
				if ( item )
				{
					GameObjDbStruct* itemDB = CNetGlobal::GetObj(item->OrgObjID);
					if ( itemDB )
					{
						string itemName = g_ObjectData->GetItemColorName(item);
						lua_pushstring(L, ItemUTIL::GetItemIcon(item));
						lua_pushstring(L, itemName.c_str());
						lua_pushnumber(L, item->GetCount());
						return 3;
					}
				}
			}
			break;

		case eTradeItemType_Bank:
			break;

		case eTradeItemType_Equipment:
			break;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetTradeTargetItemInfo(lua_State* L)
{
	ItemFieldStruct* item = NetCli_TradeChild::GetTradeItem(luaL_checkint(L, 1) - 1);
	if ( item && !item->IsEmpty() )
	{
		GameObjDbStruct* itemDB = CNetGlobal::GetObj( item->OrgObjID );
		if ( itemDB )
		{
			string itemName = g_ObjectData->GetItemColorName(item);
			lua_pushstring(L, ItemUTIL::GetItemIcon(item));
			lua_pushstring(L, itemName.c_str());
			lua_pushnumber(L, item->GetCount());
			return 3;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_SetTradeMoney(lua_State* L)
{
	const char* moneyMode = luaL_checkstring(L, 1);
	int money = 0;
	int moneyAccount = 0;

	if ( strcmp(moneyMode, MONEY_MODE_COPPER) == 0 )
		money = luaL_checkint(L, 2);
	else if ( strcmp(moneyMode, MONEY_MODE_ACCOUNT) == 0 )
		moneyAccount = luaL_checkint(L, 2);

	NetCli_Trade::S_PutMoney(money, moneyAccount);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetTradePlayerMoney(lua_State* L)
{
	lua_pushnumber(L, g_pTradeFrame->GetTradeMoney());
	switch ( g_pTradeFrame->GetTradeMoneyMode() )
	{
	case 0:
		lua_pushstring(L, MONEY_MODE_COPPER);
		break;

	case 1:
		lua_pushstring(L, MONEY_MODE_ACCOUNT);
		break;

	default:
		lua_pushnil(L);
	}

	return 2;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetTradeTargetMoney(lua_State* L)
{
	lua_pushnumber(L, g_pTradeFrame->GetTradeTargetMoney());
	switch ( g_pTradeFrame->GetTradeTargetMoneyMode() )
	{
	case 0:
		lua_pushstring(L, MONEY_MODE_COPPER);
		break;

	case 1:
		lua_pushstring(L, MONEY_MODE_ACCOUNT);
		break;

	default:
		lua_pushnil(L);
	}
	return 2;
}

// ----------------------------------------------------------------------------
int LuaFunc_ClickTradeItem(lua_State* L)
{
	g_pTradeFrame->Paste(luaL_checkint(L, 1) - 1);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_AcceptTrade(lua_State* L)
{
	g_pTradeFrame->AcceptTrade(luaL_checkstring(L, 1));
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_CancelTradeAccept(lua_State* L)
{
	g_pTradeFrame->CancelTradeAccept();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_CloseTrade(lua_State* L)
{
	// 通知 SERVER 關閉界面
	NetCli_Trade::S_StopTrade(g_pTradeFrame->GetTradeTarget());

	// 清除狀態
	g_pTradeFrame->TradeClose();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_PrintTradeState(lua_State* L)
{
	char temp[MAX_PATH];
	sprintf(temp, "[Trade]State:%d Target%d",g_pTradeFrame->GetTradeState(),g_pTradeFrame->GetTradeTarget());
	g_pChatFrame->SendMsgEvent(CHAT_MSG_SYSTEM, temp, "");
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetTradeRecipientName(lua_State* L)
{
	const char* name = "";
	CRoleSprite* target = g_pGameMain->FindSpriteByID(g_pTradeFrame->GetTradeTarget());
	if ( target )
		name = target->GetName();
	lua_pushstring(L, name);
	return 1;
}