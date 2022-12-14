#include "../mainproc/GameMain.h"
#include "../../netwaker_member/NetWakerClientInc.h"
#include "../Debug/DebugMessage.h"
#include "../itemlink/ItemLink.h"
#include "ItemClipboard.h"
#include "BagFrame.h"
#include "StoreFrame.h"
#include "CharacterFrame.h"
//#include "../Houses/HousesFrame.h"
#include "ItemUtil.h"
#include "SuitSkillFrame.h"
#include "ChatFrame.h"

#include "../../netwaker_member/net_rolevalue/NetCli_RoleValueChild.h"
#include "../SkillBook/UI_SkillBook.h"

#define UiEvent_SuitSkillUpdate  "SUIT_SKILL_PLATE_UPDATE"  

CSuitSkillFrame*			g_pSuitSkillFrame = NULL;

int LuaFunc_SoubleSuitSkill				(lua_State* L);
int LuaFunc_PickupSuitSkillEquipment	(lua_State* L);
int LuaFunc_CheckSuitSkillEquipment		(lua_State* L);
int LuaFunc_GetSuitSkillEquipInfo		(lua_State* L);
int LuaFunc_CloseSuitSkill				(lua_State* L);
int LuaFunc_GetSuitSkillInfo			(lua_State* L);
int LuaFunc_GetLearnSuitSkillMoney		(lua_State* L);
int LuaFunc_GetSuitSkill_List			(lua_State* L);
int LuaFunc_SetSuitSkill_List			(lua_State* L);
int	LuaFunc_DragSuitSkill_job			(lua_State* L);
int LuaFunc_SkillPlateReceiveDrag		(lua_State* L);
int LuaFunc_SkillPlateUpdate			(lua_State* L);
int LuaFunc_SkillPlateDragStart			(lua_State* L);
int LuaFunc_SkillPlateDel				(lua_State* L);
int LuaFunc_GetPlatePosInfo				(lua_State* L);
CSuitSkillFrame::CSuitSkillFrame(CInterface* object)
	:CInterfaceBase(object)
{
	m_npcId = -1;
	m_soubleItem.clear();

	/*m_job0_Skillcount = 0 ;				// 驹м计秖
	m_job1_Skillcount = 0 ;				// 笴獿м计秖
	m_job2_Skillcount = 0 ;				// 紇︽м计秖
	m_job3_Skillcount = 0 ;				// 猭畍м计秖
	m_job4_Skillcount = 0 ;				// 步畍м计秖
	m_job5_Skillcount = 0 ;				// 肕м计秖
	m_job6_Skillcount = 0 ;				// 此狶м计秖
	m_job7_Skillcount = 0 ;				// 礛﹛м计秖
	m_job8_Skillcount = 0 ;				// 硄ノм计秖		*/

	for( int i = 0 ; i < SKILL_PLATE_MAX_JOB ; i ++ ){
		m_jobSkillcount[i] = 0;			  
	}
	g_pSuitSkillFrame = this;
}

CSuitSkillFrame::~CSuitSkillFrame()
{
	Close();
}
// ----------------------------------------------------------------------------
void CSuitSkillFrame::RegisterFunc()
{
	lua_State* L = GetLuaStateWorld();
	if (L)
	{		
		lua_register( L, "SoubleSuitSkill",				LuaFunc_SoubleSuitSkill );
		lua_register( L, "PickupSuitSkillEquipment",	LuaFunc_PickupSuitSkillEquipment );
		lua_register( L, "CheckSuitSkillEquipment",		LuaFunc_CheckSuitSkillEquipment );
		lua_register( L, "GetSuitSkillEquipInfo",		LuaFunc_GetSuitSkillEquipInfo );
		lua_register( L, "CloseSuitSkill",				LuaFunc_CloseSuitSkill );
		lua_register( L, "GetSuitSkillInfo",			LuaFunc_GetSuitSkillInfo );
		lua_register( L, "GetLearnSuitSkillMoney",		LuaFunc_GetLearnSuitSkillMoney );
		
		lua_register( L, "GetSuitSkill_List",			LuaFunc_GetSuitSkill_List );
		lua_register( L, "SetSuitSkill_List",			LuaFunc_SetSuitSkill_List );
		lua_register( L, "DragSuitSkill_job",			LuaFunc_DragSuitSkill_job );
		lua_register( L, "SkillPlateReceiveDrag",		LuaFunc_SkillPlateReceiveDrag );
		lua_register( L, "SkillPlateUpdate",			LuaFunc_SkillPlateUpdate );
		lua_register( L, "SkillPlateDragStart",			LuaFunc_SkillPlateDragStart );
		lua_register( L, "SkillPlateDel",				LuaFunc_SkillPlateDel );
		lua_register( L, "GetPlatePosInfo",				LuaFunc_GetPlatePosInfo );
		
	}
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::BuildFrames()
{
	Close();
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::DestroyFrames()
{
	Close();
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::Update(float elapsedTime)
{
	if ( !IsOpen() )
		return;

	CRoleSprite* sprite = g_pGameMain->FindSpriteByID(m_npcId);

	// 浪琩琌惠璶闽超
	if ( sprite == NULL || g_pGameMain->GetDistanceWithPlayer(sprite) > _MAX_ROLE_TOUCH_MAXRANGE_ + sprite->GetAttackRange() )
	{
		Close();
	}
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::Close()
{
	if ( m_npcId != -1 )
	{
		m_npcId = -1;

		NetCli_EQ::S_LearnSuitSkill_Close();

		SendWorldEvent("SUITSKILL_CLOSE");
	}

	// 睲埃┮Τ珇
	ClearItems();
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::ClearItems()
{
	vector<int> updateList;
	for ( map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin(); iter != m_soubleItem.end(); iter++ )
	{		
		iter->second.Unlock();

		updateList.push_back(iter->first);
	}
	m_soubleItem.clear();

	// 穝
	for ( vector<int>::iterator iter = updateList.begin(); iter != updateList.end(); iter++ )
	{
		UpdateItem(*iter);
	}
}
// ----------------------------------------------------------------------------

bool CSuitSkillFrame::Cut(int index)
{
	map<int, SoubleItemLocked>::iterator iter = m_soubleItem.find(index);
	if ( iter == m_soubleItem.end() )
		return false;
	
	SoubleItemLocked item = iter->second;

	// 簿埃赣逆戈
	DeleteItem(index);

	switch ( item.type )
	{
	case 0:
		SendWorldEvent("SUITSKILL_MODELUPDATE");
		return g_pBagFrame->Cut(item.pos);
		
	
	case 1:
		SendWorldEvent("SUITSKILL_MODELUPDATE");
		return g_pCharacterFrame->Cut(item.pos);

	default:
		return false;
	}	

	return true;
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::Paste(int index)
{	
	ClipboardItemType itemType = g_pItemClipboard->GetType();
	ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
		
	switch(g_pItemClipboard->GetType())
	{
	case eClipboardItem_Type_None:					// 珇
		Cut(index);
		return;

	case eClipboardItem_Type_Bag:					// 璉		
		if ( InsertItem(index, 0, itemInfo.bag.pos) )
			g_pBagFrame->ClearPickupItemIndex();
		break;

	case eClipboardItem_Type_Equipment:				// 杆称		
		if ( InsertItem(index, 1, itemInfo.equipment.pos) )
			g_pCharacterFrame->ClearPickupItemIndex();
		break;
		
	case eClipboardItem_Type_SuitSkillequip:		// 甅杆洪
		break;
	}	 

	g_pItemClipboard->Clear();
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SkillEquip(int slot, int type, int index ,int ContainerID)
{
	ItemFieldStruct* item = NULL;
}
// ----------------------------------------------------------------------------

SoubleItemLocked* CSuitSkillFrame::GetSoubleItem(int equipIndex)
{
	map<int, SoubleItemLocked>::iterator iter = m_soubleItem.find(equipIndex);
	if ( iter != m_soubleItem.end() )
		return &(iter->second);
	return NULL;
}
// ----------------------------------------------------------------------------

int CSuitSkillFrame::GetSuitID()
{
	map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin();
	if ( iter != m_soubleItem.end() )
	{
		ItemFieldStruct* item = iter->second.GetItemFieldStruct();
		if ( item )
		{
			GameObjDbStruct* itemDB = CNetGlobal::GetObj(item->OrgObjID);
			if ( itemDB )
			{
				return itemDB->Item.SuitID;
			}
		}
	}
	return 0;
}
// ----------------------------------------------------------------------------

bool CSuitSkillFrame::InsertItem(int equipIndex, int type, int pos)
{
	ItemFieldStruct* itemField = NULL;

	// 浪琩琌猭
	switch(type)
	{
	case 0:
		if ( (itemField = CNetGlobal::RoleData()->GetBodyItem(pos)) == NULL )
			return false;
		/*
		else if ( itemField->Pos != EM_ItemState_NONE )
			return false;
		*/
		break;

	case 1:
		if ( (itemField = CNetGlobal::RoleData()->GetEqItem(pos)) == NULL )
			return false;
		/*
		else if ( itemField->Pos != EM_ItemState_NONE )
			return false;
		*/
		break;

	default:
		return false;
	}

	// 浪琩竚琌杆称
	if ( RoleDataEx::CheckEquipmentPos(equipIndex, itemField) == false )
	{
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_POS_ERROR"));
		return false;
	}
	
	// 琌獶甅杆
	if ( itemField )
	{
		// 竒瞗筁
		if ( itemField->Mode.SuitSkill )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_ALREADY_SOUBLE"));
			return false;
		}

		// 眔珇戈
		GameObjDbStruct* itemDB = CNetGlobal::GetObj(itemField->OrgObjID);
		if ( itemDB == NULL )
			return false;

		// 眔甅杆戈
		GameObjDbStruct* suitDB = CNetGlobal::GetObj(itemDB->Item.SuitID);
		if ( suitDB == NULL )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_NOT_SUIT"));
			return false;
		}
	}

	// 浪琩琌甅甅杆
	if ( m_soubleItem.size() > 0 )
	{
		map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin();
		ItemFieldStruct* firstItem = iter->second.GetItemFieldStruct();
		if ( firstItem == NULL )
			return false;

		// 眔珇戈
		GameObjDbStruct* itemDB = CNetGlobal::GetObj(firstItem->OrgObjID);
		if ( itemDB == NULL )
			return false;

		// 眔甅杆戈
		GameObjDbStruct* suitDB = CNetGlobal::GetObj(itemDB->Item.SuitID);
		if ( suitDB == NULL )
			return false;

		bool isDifferent = true;
		for ( int i = 0; i < suitDB->Suit.TotalCount; i++ )
		{
			if ( suitDB->Suit.SuitList[i] == itemField->OrgObjID )
			{
				isDifferent = false;
				break;
			}
		}

		if ( isDifferent )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_DIFFERENT_SUIT"));
			return false;
		}
	}

	// 杆称ぃ材ン	
	for ( map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin(); iter != m_soubleItem.end(); iter++ )
	{
		ItemFieldStruct* item = iter->second.GetItemFieldStruct();
		if ( item->OrgObjID == itemField->OrgObjID )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SAME_SUIT"));
			return false;
		}
	}	

	// 浪琩逆琌竒杆称
	map<int, SoubleItemLocked>::iterator iter = m_soubleItem.find(equipIndex);
	if ( iter != m_soubleItem.end() )
	{
		// 赣场竒Τ珇
		return false;
	}

	SoubleItemLocked item(type, pos);
	m_soubleItem[equipIndex] = item;
	itemField->Pos = EM_ItemState_CliDisabled;

	UpdateItem(equipIndex);

	return true;
}
// ----------------------------------------------------------------------------

bool CSuitSkillFrame::DeleteItem(int equipIndex)
{
	map<int, SoubleItemLocked>::iterator iter = m_soubleItem.find(equipIndex);
	if ( iter != m_soubleItem.end() )
	{
		// 秆埃玛﹚
		iter->second.Unlock();

		// 睲埃赣掸戈
		m_soubleItem.erase(iter);

		UpdateItem(equipIndex);

		return true;
	}
	return false;
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::UpdateItem(int equipIndex)
{
	lua_State* L = GetLuaState();
	if ( L )
	{
		lua_pushnumber(L, equipIndex);
		lua_setglobal(L, "arg1");
		SendWorldEvent("SUITSKILLEQUIP_UPDATE");
	}
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SoubleItem(int selectItem)
{
	if ( m_soubleItem.size() > 0 && selectItem >= 0 && selectItem < 4 )
	{
		ItemFieldStruct* itemField = NULL;
		vector<int> list;

		for ( map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin(); iter != m_soubleItem.end(); iter++ )
		{
			if ( itemField == NULL )
				itemField = iter->second.GetItemFieldStruct();
			list.push_back(iter->second._value);
		}		

		// 眔珇戈
		GameObjDbStruct* itemDB = CNetGlobal::GetObj(itemField->OrgObjID);
		if ( itemDB == NULL )
			return;

		// 眔甅杆戈
		GameObjDbStruct* suitDB = CNetGlobal::GetObj(itemDB->Item.SuitID);
		if ( suitDB == NULL )
			return;

		// 甅杆计秖琌ì镑
		
		// 浪琩產窥
		if ( CNetGlobal::RoleData()->BodyMoney() < suitDB->Suit.SuitSkillMoney )
		{
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_AC_NOT_ENOUGH_MONEY"));
			return;
		}

		NetCli_EQ::S_LearnSuitSkillRequest( list, suitDB->Suit.SuitSkillID[selectItem] );
	}
}
// ----------------------------------------------------------------------------

bool CSuitSkillFrame::CheckSuitItem()
{
	int suitId = GetSuitID();

	// 眔甅杆戈
	GameObjDbStruct* suitDB = CNetGlobal::GetObj(suitId);
	if ( suitDB == NULL )
		return false;	

	set<int> suitList;
	for ( int i = 0; i < 10; i++ )
	{
		if ( suitDB->Suit.SuitList[i] != 0 )
			suitList.insert(suitDB->Suit.SuitList[i]);
	}
	
	for ( map<int, SoubleItemLocked>::iterator iter = m_soubleItem.begin(); iter != m_soubleItem.end(); iter++ )
	{
		ItemFieldStruct* item = iter->second.GetItemFieldStruct();
		if ( item == NULL )
			return false;

		set<int>::iterator suit_iter = suitList.find(item->OrgObjID);
		if ( suit_iter == suitList.end() )
			return false;

		suitList.erase(suit_iter);
	}

	return suitList.empty();
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::R_Open(int npcId)
{
	if ( m_npcId != -1 )
		Close();

	m_npcId = npcId;
	SendWorldEvent("SUITSKILL_OPEN");
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::R_LearnSuitSkillResult( int LearnSkillID , LearnSuitSkillResultENUM Result )
{
	switch (Result)
	{
	case EM_LearnSuitSkillResult_OK:
		{
			int count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillCount;
			CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillCount++;

			if ( count >= 0 && count < _MAX_SUIT_SKILL_COUNT_ )
				CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[count].SkillID = LearnSkillID;

			string link;
			SkillGUID_To_HyperLink( LearnSkillID, 0, link );

			g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM_GET, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_SUCCESS"), link.c_str());

			SendWorldEvent("LEARNSUITSKILL_SUCCESS");

			ClearItems();
		}
		break;

	case EM_LearnSuitSkillResult_NotOpen:		//ざ⊿秨币
		g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_FAILED1"));
		break;

	case EM_LearnSuitSkillResult_DataErr:		//戈Τ拜肈
		g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_FAILED2"));
		break;

	case EM_LearnSuitSkillResult_CountErr:		//甅杆计秖Τ拜肈
		g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_FAILED3"));
		break;

	case EM_LearnSuitSkillResult_AlwaysLearn:	//厩穦
		g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_FAILED4"));
		break;

	case EM_LearnSuitSkillResult_SkillFull:		//м骸
		g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_SUITSKILL_LEARN_FAILED5"));
		break;
	}
}

// ----------------------------------------------------------------------------

void CSuitSkillFrame::R_SetSuitSkillResult( SetSuitSkillResultENUM Result )
{	
	SkillPlateData Info;
	switch ( Result )
	{
	case EM_SetSuitSkillResult_OK:
		if( CliNetWaker::PopClientData( "Info", &Info, sizeof(Info) ) )
		{
			if(&Info) 
			{
				CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.EQ[Info.Pos] = Info.SkillID;
				//**g_pSuitSkillFrame->m_skillplateitems[GetJobForPlateItem()][Info.Pos] = Info.SkillID;
				lua_State* L = GetLuaState();

				//lua_pushnumber(L, Info.Pos);
				//lua_setglobal(L, "arg1");
				//SendWorldEvent("SUITSKILLEPLATE_UPDATE");	
				//m_skillplateitems[GetJobForPlateItem()][Info.Pos] = Info.SkillID;
				//SaveEQ_plateitems(Info.Pos , Info.SkillID);
				SetSaveUpdateTime();
			}	
		}
			
		break;

	case EM_SetSuitSkillResult_OnAttackMode:		//驹矮篈ぃㄏノ
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED1"));	
		break;	

	case EM_SetSuitSkillResult_SkillNotFind:		//琩礚м
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED2"));
		break;

	case EM_SetSuitSkillResult_PosErr:				//竚岿粇
		//g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED3"));
		//g_pItemClipboard->Clear();	
		break;
	}
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::R_UseSuitSkillResult( UseSuitSkillResultENUM Result )
{
}



// ----------------------------------------------------------------------------
// 瞗м
// arg1 мま絪腹 1 ~ 4
int LuaFunc_SoubleSuitSkill				(lua_State* L)
{
	int num = luaL_checkint(L, 1) - 1;
	if( num == -1 )
	{
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_CHECK_ERROR"));
	}
	else
	{
		g_pSuitSkillFrame->SoubleItem(num);	
	}
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_PickupSuitSkillEquipment (lua_State* L)
{
	g_pSuitSkillFrame->Paste(luaL_checkint(L, 1));	
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_CheckSuitSkillEquipment (lua_State* L)
{
	lua_pushboolean(L, g_pSuitSkillFrame->CheckSuitItem());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_CloseSuitSkill(lua_State* L)
{
	g_pSuitSkillFrame->Close();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetSuitSkillInfo(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;
	int suitId = g_pSuitSkillFrame->GetSuitID();

	if ( suitId )
	{
		GameObjDbStructEx* suitDB = CNetGlobal::GetObj(suitId);
		if ( suitDB || index < 0 || index >= 4 )
		{
			GameObjDbStructEx* magicDB = CNetGlobal::GetObj(suitDB->Suit.SuitSkillID[index]);
			if ( magicDB )
			{
				lua_pushstring(L, magicDB->GetName());
				lua_pushstring(L, ItemUTIL::GetItemIcon(magicDB, false));
				return 2;
			}
		}
	}

	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetLearnSuitSkillMoney(lua_State* L)
{
	int suitId = g_pSuitSkillFrame->GetSuitID();
	int money = 0;

	if ( suitId )
	{
		GameObjDbStructEx* suitDB = CNetGlobal::GetObj(suitId);
		if ( suitDB )
		{
			money = suitDB->Suit.SuitSkillMoney;
		}
	}
	lua_pushnumber(L, money);
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetSuitSkillEquipInfo(lua_State* L)
{
	SoubleItemLocked* soubleItem = g_pSuitSkillFrame->GetSoubleItem(luaL_checkint(L, 1));
	if ( soubleItem )
	{
		ItemFieldStruct* item = soubleItem->GetItemFieldStruct();
		if ( item )
		{
			GameObjDbStructEx* itemDB = CNetGlobal::GetObj(item->OrgObjID);
			if ( itemDB )
			{
				lua_pushstring(L, itemDB->GetName());
				lua_pushstring(L, ItemUTIL::GetItemIcon(item));

				string link;
				if( !ItemField_To_ItemLink( item, link ) ) 
				{
					return 2;
				}
				lua_pushstring(L, link.c_str() );
				return 3;
			}
		}
	}
	return 0;
}










// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SaveClientData()
{
	// 玻ネ既戈

	CTempFile tempFile(65536);

	int  itemSize;	

	// 糶挡篶
	itemSize = sizeof(m_skillplateitems);
	tempFile.Write(&itemSize, sizeof(int));	

	// 纗俱掸戈
	tempFile.Write(m_skillplateitems, sizeof(m_skillplateitems));
	
	g_pGameMain->SaveClientData(eSaveClientData_SkillPlate, tempFile.GetPosition(), tempFile.GetData());

	// 盢程郎丁埃
	SetSaveUpdateTime(0.0f);
}

// ----------------------------------------------------------------------------

void CSuitSkillFrame::LoadClientData(int type, int size, char* data)
{
	if( type == eSaveClientData_SkillPlate )
	{
		CTempFile tempFile(data,size);

		int itemSize;

		// ゑ癸挡篶
		tempFile.Read(&itemSize, sizeof(int));

		if ( itemSize != sizeof(m_skillplateitems) )
			return;

		tempFile.Read(m_skillplateitems, sizeof(m_skillplateitems));
	} 
}

// ----------------------------------------------------------------------------

void CSuitSkillFrame::Loadplateitems_EQ()
{
	int i ;
	int JobID = GetJobForPlateItem();
	int count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;

	if( m_skillplateitems ){
 		for( i = 0 ; i < count ; i++ ) {	
			if(  JobID >= 0 && JobID < SKILL_PLATE_MAX_JOB ){
				if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
					if( ( m_skillplateitems[JobID][i] == NULL || m_skillplateitems[JobID][i] < 10000 ) && m_skillplateitems[JobID][i] != 0){
						m_skillplateitems[JobID][i] = 0;
					}
					if( m_skillplateitems[JobID][i] != CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.EQ[i]){
						g_pSuitSkillFrame->SendSkillPlatePosToServer( m_skillplateitems[JobID][i] , i ); 
					}
				}
			}
		}
	}
}
// ----------------------------------------------------------------------------
void CSuitSkillFrame::SaveEQ_plateitems(int pos , int iskillID)
{		
	int PlateItemNum = GetJobForPlateItem();
	if(  PlateItemNum >= 0 && PlateItemNum < SKILL_PLATE_MAX_JOB ){
		if( pos >= 0 && pos < SKILL_PLATE_MAX_ITEM){
			m_skillplateitems[PlateItemNum][pos] = iskillID;  
		}
	}
}
// ----------------------------------------------------------------------------
int	CSuitSkillFrame::GetJobForPlateItem()
{
	int Jobid = 0;
	RoleDataEx*    Role    = CNetGlobal::RoleData();
	Jobid = Role->BaseData.Voc - 1;
	return Jobid;	
	/* switch(Role->BaseData.Voc)
	 {
	 case EM_Vocation_Warrior:						// 驹
		Jobid = 0;
		return Jobid;
	 case EM_Vocation_Ranger:						// 笴獿
		Jobid = 1;
		return Jobid;
	 case EM_Vocation_Rogue:						// 紇︽
		Jobid = 2;
		return Jobid;
	 case EM_Vocation_Wizard:						// 猭畍
		Jobid = 3;
		return Jobid;
	 case EM_Vocation_Priest:						// 步
		Jobid = 4;
		return Jobid;
	 case EM_Vocation_Knight:						// 肕
		Jobid = 5;
		return Jobid;
	 case EM_Vocation_RuneDancer:					// 此狶
		Jobid = 6;
		return Jobid;
	 case EM_Vocation_Druid:						// 礛﹛
		Jobid = 7;
		return Jobid;
	 case EM_Vocation_Harpsyn:						// 艶
		 Jobid = 8;
		 return Jobid;
	 case EM_Vocation_Psyron:						// 才ゅ
		 Jobid = 9;
		 return Jobid;
	 case EM_Vocation_Duelist:						// 矮囱
		 Jobid = 10;
		 return Jobid;   
	 }	 
	return Jobid;  */
}
// ----------------------------------------------------------------------------
void CSuitSkillFrame::InitJobSkill_Count()
{
	/*m_job0_Skillcount = 0 ;				// 驹м计秖
	m_job1_Skillcount = 0 ;				// 笴獿м计秖
	m_job2_Skillcount = 0 ;				// 紇︽м计秖
	m_job3_Skillcount = 0 ;				// 猭畍м计秖
	m_job4_Skillcount = 0 ;				// 步畍м计秖
	m_job5_Skillcount = 0 ;				// 肕м计秖
	m_job6_Skillcount = 0 ;				// 此狶м计秖
	m_job7_Skillcount = 0 ;				// 礛﹛м计秖
	m_job8_Skillcount = 0 ;				// 硄ノм计秖	*/

	for( int i = 0 ; i < SKILL_PLATE_MAX_JOB ; i ++ ){
		m_jobSkillcount[i] = 0;			  
	}

	int count;
	int i ;
	count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillCount;
	for(i = 0 ; i < count ; i++ ){
		if( i >= _MAX_SUIT_SKILL_COUNT_)
			break;

		int SkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[i].SkillID;
		if( SkillID != 0 ){
			GameObjDbStruct* SkillObj = CNetGlobal::GetObj(SkillID);
			int id;
			//耞戮穨
			
			if( SkillObj != NULL && SkillObj->MagicCol.Need[0].Type == EM_MagicSpellNeedType_Self_MainJob )
				id = SkillObj->MagicCol.Need[0].Value;				
			else if(SkillObj  != NULL &&  SkillObj->MagicCol.Need[1].Type == EM_MagicSpellNeedType_Self_MainJob )
				id = SkillObj->MagicCol.Need[1].Value;
			else id = SKILL_PLATE_MAX_JOB ; 
	
			if( id > 0 && id <= SKILL_PLATE_MAX_JOB){
				
				m_jobSkillcount[id - 1] += 1;	


			/*	if( id == 1)		m_job0_Skillcount = m_job0_Skillcount + 1;				// 驹м计秖
				else if( id == 2)	m_job1_Skillcount = m_job1_Skillcount + 1;				// 笴獿м计秖
				else if( id == 3)	m_job2_Skillcount = m_job2_Skillcount + 1;				// 紇︽м计秖
				else if( id == 4)	m_job3_Skillcount = m_job3_Skillcount + 1;				// 猭畍м计秖
				else if( id == 5)	m_job4_Skillcount = m_job4_Skillcount + 1;				// 步畍м计秖
				else if( id == 6)	m_job5_Skillcount = m_job5_Skillcount + 1;				// 肕м计秖
				else if( id == 7)	m_job6_Skillcount = m_job6_Skillcount + 1;				// 此狶м计秖
				else if( id == 8)	m_job7_Skillcount = m_job7_Skillcount + 1;				// 礛﹛м计秖
				else m_job8_Skillcount = m_job8_Skillcount + 1;								// 硄ノм计秖		*/
			}
		}
	}

}
bool CSuitSkillFrame::CheckSkillItem_voc( int SkillID)
{
	RoleDataEx* pRoleDataEx = CNetGlobal::RoleData();
	GameObjDbStructEx* itemDB = CNetGlobal::GetObj(SkillID);

	if( NULL != itemDB )
	{
		if( pRoleDataEx->TempData.Attr.Voc == itemDB->MagicCol.JobInfo.Job || pRoleDataEx->TempData.Attr.Voc == 0 || itemDB->MagicCol.JobInfo.Job == -1)
		{
			return true;
		}
	}
	else if(SkillID == 0)
	{
		return true;
	}
	return false;
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SendSkillPlatePosToServer(int SkillID, int Pos)
{
	int size;
	SkillPlateData Info;

	Info.SkillID = SkillID ;
	Info.Pos = Pos ;
	size = sizeof(Info);
	CliNetWaker::PushClientData( "Info" , &Info , size);


	if( CheckSkillItem_voc(SkillID) )
	{
		NetCli_EQ::S_SetSuitSkillRequest( SkillID , Pos );
	}
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SetSkillPlateOldID(int SkillID, int Pos)
{
	g_pSuitSkillFrame->SendSkillPlatePosToServer( SkillID, Pos);  ///(iSkillID, pos);
}
// ----------------------------------------------------------------------------

void CSuitSkillFrame::SkillPlateDelete()
{
	lua_State* L = GetLuaState();
	int iSkillID , i , count;

	ClipboardItemType itemType = g_pItemClipboard->GetType();
	ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
	
	iSkillID = itemInfo.SuitSkill.skillID;

	count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;

	g_pItemClipboard->Clear();
	for( i = 0 ; i < count ; i++ )
	{
		int PlateItemNum = GetJobForPlateItem();
		if(  PlateItemNum >= 0 && PlateItemNum < SKILL_PLATE_MAX_JOB ){
			if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
				if( iSkillID == g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][i]){
					lua_pushnumber(L, i);
					lua_setglobal(L, "arg1");
					SendWorldEvent("SKILLPLATE_DELETE");
				}
			}
		}
	}

}
// ----------------------------------------------------------------------------
bool CSuitSkillFrame::CheckSkillItem( int SkillID )
{
	if( SkillID >= 0 )
	{
		RoleDataEx* pRoleDataEx = CNetGlobal::RoleData();
		GameObjDbStructEx* itemDB = CNetGlobal::GetObj(SkillID);
		if( NULL != itemDB )
		{
			if( pRoleDataEx->TempData.Attr.Level >= itemDB->MagicCol.LimitLv )
			{
				return true;
			}
			else
			{
				g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED5"));
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------

int LuaFunc_SetSuitSkill_List(lua_State* L)
{
	g_pSuitSkillFrame->InitJobSkill_Count();


	for( int i = 0 ; i < SKILL_PLATE_MAX_JOB ; i ++ ){
		lua_pushnumber(L, g_pSuitSkillFrame->m_jobSkillcount[i]);	
	}
	return SKILL_PLATE_MAX_JOB ;


/*	lua_pushnumber(L, g_pSuitSkillFrame->m_job0_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job1_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job2_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job3_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job4_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job5_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job6_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job7_Skillcount);	
	lua_pushnumber(L, g_pSuitSkillFrame->m_job8_Skillcount);	
 	return 9 ;   */
}
// ----------------------------------------------------------------------------

int LuaFunc_GetSuitSkill_List(lua_State* L)
{
	int Jobindex = luaL_checkint(L, 1);
	int Skillindex = luaL_checkint(L, 2);

	int count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillCount;
	int i , k = 0 , num = 0;

	int SkillID = 0;
	const char* name = "";
	const char* texture = "";

	for( i = 0 ; i < count ; i++ ){
		if( i >= _MAX_SUIT_SKILL_COUNT_)
			break;

		SkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[i].SkillID;
	
		if( SkillID != 0 ){
			GameObjDbStruct* SkillObj = CNetGlobal::GetObj(SkillID);
			int needjob;
			//耞戮穨
			if(SkillObj->MagicCol.Need[0].Type == EM_MagicSpellNeedType_Self_MainJob )
				needjob = SkillObj->MagicCol.Need[0].Value;				
			else if( SkillObj->MagicCol.Need[1].Type == EM_MagicSpellNeedType_Self_MainJob )
				needjob = SkillObj->MagicCol.Need[1].Value;
			else needjob = SKILL_PLATE_MAX_JOB ; 

			if( SkillObj ){
				if( needjob == Jobindex ){
					num += 1 ;
					if(Skillindex == num - 1 ){						
						GameObjDbStruct* SkillIconObj = CNetGlobal::GetObj(SkillObj->ImageID);

						name = SkillObj->Name;
						texture = SkillIconObj->Image.ACTField;
					
						if(texture){

						}
						else{
							texture = "";
						}
						lua_pushstring(L, name);		//м嘿
						lua_pushstring(L, texture);     //м瓜
						lua_pushnumber(L, i);     
						return 3;	
					}

				}
			}
		} 
	}
	return 0;
}
// ----------------------------------------------------------------------------

//□□□□□□□□□□□□□□□□□□м╈┰□□□□□□□□□□□□□□□□□□
int LuaFunc_DragSuitSkill_job(lua_State* L)
{	

	int index = luaL_checkint(L, 1);

	if( index < 0 || index >= _MAX_SUIT_SKILL_COUNT_)	
		return 0;

	int SkillID = 0;
	SkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[index].SkillID;
	
	if ( SkillID != 0){
		GameObjDbStruct* itemDB = CNetGlobal::GetObj( SkillID );
		if ( itemDB ){
			g_pItemClipboard->SetFileFromImageID(itemDB->ImageID);
			g_pItemClipboard->PickupSuitSkill(SkillID);
		}
	}
	return 0;
}
//□□□□□□□□□□□□□□□□□□м絃钡Μ场だ□□□□□□□□□□□□□□□□□□
// num = 0  钡╈┰ 眖g_pItemClipboard眔skillID  ;  num ~= 0 翴匡,眖竚眔SkillID
int LuaFunc_SkillPlateReceiveDrag(lua_State* L)
{
	// iSkillID , ipos ヘ夹竚侣iSkillID , pos
	// kSkillID , kpos 稱璶蠢传穝戈 
	int iSkillID = 0 , ipos = 0 , kSkillID = 0 , kpos = 0 ;  
	int i , k = 0 ;
	int num = luaL_checkint(L, 1);
	int index = luaL_checkint(L, 2);  //pos
	int count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount ;

	if ( g_pGameMain->GetCombatState() == true ){
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED1"));	
		return 0;
	}
	RoleDataEx* pRoleDataEx = CNetGlobal::RoleData();
	int Jobid = g_pSuitSkillFrame->GetJobForPlateItem();

	ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
	if(g_pItemClipboard->GetType() == eClipboardItem_Type_SuitSkill){
		GameObjDbStructEx* itemDB = CNetGlobal::GetObj(itemInfo.SuitSkill.skillID);
		if( NULL == itemDB ){
			return 0;
		}

		if ( itemDB->MagicCol.JobInfo.Job != EM_Vocation_None ){
			// 戮穨蛤盡妮м
			if ( itemDB->MagicCol.JobInfo.IsJobLimit ){
				if ( pRoleDataEx->TempData.Attr.Voc != itemDB->MagicCol.JobInfo.Job ){
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED6"));
					return 0;
				}		
			}
		}
	}

	//砞﹚ iSkillID , ipos , kSkillID , kpos ;  
	if( num == 0){
		ipos = index;
		if(  Jobid >= 0 && Jobid < SKILL_PLATE_MAX_JOB ){
			if( ipos >= 0 && ipos < SKILL_PLATE_MAX_ITEM){
				iSkillID =  g_pSuitSkillFrame->m_skillplateitems[Jobid][ipos];
			}
		}		
		ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
			
		switch(g_pItemClipboard->GetType()){
		case eClipboardItem_Type_SuitSkill:  
			kSkillID = itemInfo.SuitSkill.skillID;
			kpos = -1; 
			count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;
			for( i = 0 ; i < count ; i++ ){	 

				if(  Jobid >= 0 && Jobid < SKILL_PLATE_MAX_JOB ){
					if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
						if( kSkillID == g_pSuitSkillFrame->m_skillplateitems[Jobid][i] ){
							g_pItemClipboard->Clear();
							g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED4"));
							return 0;      //狦ㄓ方琌碞Return
						}

					}
				}
			}  
			break;	
		case eClipboardItem_Type_SkillPlate:
			kSkillID = itemInfo.SkillPlate.skillID;
			kpos = itemInfo.SkillPlate.pos;  
			break;	
		}
	}
	else{
		kSkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[index].SkillID;
		{
			GameObjDbStructEx* itemDB = CNetGlobal::GetObj(kSkillID);
			if( NULL == itemDB ){
				return 0;
			}
			if( itemDB->MagicCol.JobInfo.IsJobLimit ){
				if( pRoleDataEx->TempData.Attr.Voc != itemDB->MagicCol.JobInfo.Job ){
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED6"));
					return 0;
				}
			}
		}

		kpos = -1;
		for( i = 0 ; i < count ; i++ ){				  
			if(  Jobid >= 0 && Jobid < SKILL_PLATE_MAX_JOB ){
				if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
					if( kSkillID == g_pSuitSkillFrame->m_skillplateitems[Jobid][i] ){
						g_pItemClipboard->Clear();
						g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED4"));
						return 0;
					}
				}
			}
		}  

		for( i = 0 ; i < count ; i++ ){	  		  
			if(  Jobid >= 0 && Jobid < SKILL_PLATE_MAX_JOB ){
				if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
					if(g_pSuitSkillFrame->m_skillplateitems[Jobid][i] <= 0 || g_pSuitSkillFrame->m_skillplateitems[Jobid][i] == NULL ){
						ipos = i;
						iSkillID = g_pSuitSkillFrame->m_skillplateitems[Jobid][ipos];
						k = 1;
						break;
					}
				}
			}
		}
		if( k != 1 ){
			ipos = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount - 1 ;
			if(  Jobid >= 0 && Jobid < SKILL_PLATE_MAX_JOB ){
				if( ipos >= 0 && ipos < SKILL_PLATE_MAX_ITEM){
					iSkillID = g_pSuitSkillFrame->m_skillplateitems[Jobid][ipos];
				}
			}
		}
	}


	// 蠢传
	if( g_pSuitSkillFrame->CheckSkillItem(kSkillID) ){
		//盢セ竚睲
		if( kpos != -1 ){
			//蠢传癸禜琌玥挡狦ぃ跑
			if( kpos == ipos){
				g_pItemClipboard->Clear();
				return 0;
			}
			g_pSuitSkillFrame->SendSkillPlatePosToServer( 0, kpos); 

			int PlateItemNum = g_pSuitSkillFrame->GetJobForPlateItem();
			if(  PlateItemNum >= 0 && PlateItemNum < SKILL_PLATE_MAX_JOB ){
				if( kpos >= 0 && kpos < SKILL_PLATE_MAX_ITEM){
					g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][kpos] = 0;
				}
			}
		}

		//璝璶蠢传竚琌箂 碞钡蠢传 睲芭禟茂
		int PlateItemNum = g_pSuitSkillFrame->GetJobForPlateItem();
		if(  PlateItemNum >= 0 && PlateItemNum < SKILL_PLATE_MAX_JOB ){
			if( ipos >= 0 && ipos < SKILL_PLATE_MAX_ITEM){

				if(g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][ipos] <= 0 || g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][ipos] == NULL ){
					g_pSuitSkillFrame->SendSkillPlatePosToServer( kSkillID, ipos );
					g_pItemClipboard->Clear();
					g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][ipos] = kSkillID;
				}
				//璝璶蠢传竚Τм  蠢传  狡籹芭禟茂
				else{	
					g_pSuitSkillFrame->SendSkillPlatePosToServer( 0 , ipos ); 
					g_pSuitSkillFrame->SendSkillPlatePosToServer( kSkillID , ipos ); 
					g_pSuitSkillFrame->m_skillplateitems[PlateItemNum][ipos] = kSkillID;
					g_pItemClipboard->Clear();

					//狡籹芭禟茂 (╈Σ璶)
					if( num == 0){
						for( i = 0 ; i < CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillCount ; i++ ){
							if( iSkillID == CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.LeanSkillList[i].SkillID ){
								GameObjDbStruct* itemDB = CNetGlobal::GetObj( iSkillID );
								if ( itemDB ){
									g_pItemClipboard->SetFileFromImageID(itemDB->ImageID);
									g_pItemClipboard->PickupSuitSkill(iSkillID);
									break;
								}
							}
						}
					}
				}	
			}
		}
	}
	g_pSuitSkillFrame->SendWorldEvent(UiEvent_SuitSkillUpdate);	
	return 0;
}
// ----------------------------------------------------------------------------
//穝м絃 肚м絃竚 眔skillID 肚瓜 
int LuaFunc_SkillPlateUpdate(lua_State* L)
{
	int index = luaL_checkint(L, 1);
	int iSkillID ;
	const char* name = "";
	const char* texture = "";
	
	int count ;

	if( index == -1 )  //璶计秖
	{
		count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;
		lua_pushnumber(L, count);   
		return 1;
	}

	else
	{
		int num = g_pSuitSkillFrame->GetJobForPlateItem();
		if(  num >= 0 && num < SKILL_PLATE_MAX_JOB ){
			if( index >= 0 && index < SKILL_PLATE_MAX_ITEM){
				iSkillID = g_pSuitSkillFrame->m_skillplateitems[num][index] ;
			}
		}
		//if( iSkillID != 0 || iSkillID != NULL){
			GameObjDbStruct* SkillObj = CNetGlobal::GetObj(iSkillID);
			if(SkillObj){
				GameObjDbStruct* SkillIconObj = CNetGlobal::GetObj(SkillObj->ImageID);
				name = SkillObj->Name;
				texture = SkillIconObj->Image.ACTField;

				lua_pushstring(L, name);		//м嘿
				lua_pushstring(L, texture);     //м瓜    
				return 2;	
			}	   			
		//}
		else
		{
			name = "123456789";
			if( index == 0 )
			{
				texture = "interface\\SuitSkill\\Suit_icons_01";
			}
			else if( index == 1 )
			{
				texture = "interface\\SuitSkill\\Suit_icons_02";
			} 
			else if( index == 2 )
			{
				texture = "interface\\SuitSkill\\Suit_icons_03";
			} 
			else if( index == 3 )
			{
				texture = "interface\\SuitSkill\\Suit_icons_04";
			} 
			else if( index == 4 )
			{
				texture = "interface\\SuitSkill\\Suit_icons_05";
			} 
			lua_pushstring(L, name);		//м嘿
			lua_pushstring(L, texture);     //м瓜    
			return 2;	
		}
	}
	return 0 ;
}
// ----------------------------------------------------------------------------
//м絃更    -1 睲埃场竚  
int LuaFunc_SkillPlateDel(lua_State* L)
{
	int index = luaL_checkint(L, 1);
	int i , count ;
	if ( g_pGameMain->GetCombatState() == true )
	{
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_SKILL_FAILED1"));	
		return 0;
	}
	count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;

	if( index == -1 ){	//睲埃场
		for( i = 0 ; i < count ; i++ ){
			g_pSuitSkillFrame->SendSkillPlatePosToServer( 0, i);  ///(iSkillID, pos);
			int num = g_pSuitSkillFrame->GetJobForPlateItem();
			if(  num >= 0 && num < SKILL_PLATE_MAX_JOB ){
				if( i >= 0 && i < SKILL_PLATE_MAX_ITEM){
					g_pSuitSkillFrame->m_skillplateitems[num][i] = 0 ;
				}
			}
		}
		return 0;
	}

	else{			//睲埃ㄤい竚
		g_pSuitSkillFrame->SendSkillPlatePosToServer( 0, index);  ///(iSkillID, pos);
		int num = g_pSuitSkillFrame->GetJobForPlateItem();
		if(  num >= 0 && num < SKILL_PLATE_MAX_JOB ){
			if( index >= 0 && index < SKILL_PLATE_MAX_ITEM){
				g_pSuitSkillFrame->m_skillplateitems[num][index] = 0 ;
			}
		}
		return 0;	
	}

	return 0;
}


// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

int LuaFunc_SkillPlateDragStart(lua_State* L)
{
	int index = luaL_checkint(L, 1);
	//**int SkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.EQ[index];
	int SkillID = 0;
	int num = g_pSuitSkillFrame->GetJobForPlateItem();
	if(  num >= 0 && num < SKILL_PLATE_MAX_JOB ){
		if( index >= 0 && index < SKILL_PLATE_MAX_ITEM){
			SkillID = g_pSuitSkillFrame->m_skillplateitems[g_pSuitSkillFrame->GetJobForPlateItem()][index] ;
		}
	}

	if ( SkillID !=0 ){
		GameObjDbStruct* itemDB = CNetGlobal::GetObj( SkillID );
		if ( itemDB ){
			g_pItemClipboard->SetFileFromImageID(itemDB->ImageID);
			g_pItemClipboard->PickupSkillPlate(SkillID,index);
		}
	}
	return 0;
}
// ----------------------------------------------------------------------------

int LuaFunc_GetPlatePosInfo(lua_State* L)
{
	int index = luaL_checkint(L, 1);    //-1 check g_pItemClipboard  ; 1 check pos
	int id = luaL_checkint(L, 2);
	int count = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.MaxEqSkillCount;
	int SkillID;
	if( index == -1 )	//浪琩芭禟茂琌Τ狥﹁ 
	{			
		switch(g_pItemClipboard->GetType()){
		case eClipboardItem_Type_None:	
			return false;	
		case eClipboardItem_Type_SuitSkill:
			return true;	
		case eClipboardItem_Type_SkillPlate:
			return true;	
		}
	}else if( index == -2 )//產翴匡м絃﹟ゼ秨币竚 'м﹟ゼ秨'
	{
		if( id > count){
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_SUITSKILL_PLATE_FAILED1"));
		}	
	}

	else if( index != -1 && index != -2 )	//肚竚 璝Τм 玥true 
	{
		//**SkillID = CNetGlobal::RoleData()->PlayerBaseData->SuitSkill.EQ[id];

		int num = g_pSuitSkillFrame->GetJobForPlateItem();
		if(  num >= 0 && num < SKILL_PLATE_MAX_JOB ){
			if( id >= 0 && id < SKILL_PLATE_MAX_ITEM){
				SkillID = g_pSuitSkillFrame->m_skillplateitems[g_pSuitSkillFrame->GetJobForPlateItem()][id] ;
			}
		}
		if( SkillID != 0 ){
			return true;
		}
		return false;
	}
	return false;
}
// ----------------------------------------------------------------------------