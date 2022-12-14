#include <Windows.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include "../../netwaker_member/NetWakerClientInc.h"
#include "../mainproc/GameMain.h"
#include "../SkillBook/UI_SkillBook.h"
#include "../interface/Debug/DebugMessage.h"
#include "../Interface/Login/AccountLogin.h"
#include "ChatFrame.h"
#include "UnitFrame.h"
#include "BagFrame.h"
#include "StoreFrame.h"
#include "ItemClipboard.h"
#include "ActionFrame.h"
#include "KeyBinding.h"
#include "GameControl.h"
#include "PartyFrame.h"
#include "../Map/Minimap.h"
#include "../Houses/HousesFrame.h"
#include "TutorialFrame.h"
#include "SuitSkillFrame.h"
#define MAX_MULTI_RIDE_RANGE			50.0f


CGameControl*			g_pGameControl = NULL;

int LuaFunc_GetLanguage					(lua_State* L);
int LuaFunc_GetLocation					(lua_State* L);
int LuaFunc_GetInstall					(lua_State* L);
int LuaFunc_GetImageLocation			(lua_State* L);
int LuaFunc_IsHideLoginUI				(lua_State* L);
int LuaFunc_Logout						(lua_State* L);
int LuaFunc_CancelLogout				(lua_State* L);
int LuaFunc_ChangeParallelID			(lua_State* L);
int LuaFunc_CancelChangeParallelID		(lua_State* L);

int LuaFunc_GetUIScale					(lua_State* L);
int	LuaFunc_GetFramerate				(lua_State* L);
int	LuaFunc_GetTime						(lua_State* L);
int	LuaFunc_GetPing						(lua_State* L);
int	LuaFunc_GetCursorPos				(lua_State* L);
int	LuaFunc_GetPlayerPosition			(lua_State* L);
int	LuaFunc_GetPlayerDirection			(lua_State* L);
int LuaFunc_GetPlayerMaxExp				(lua_State* L);
int LuaFunc_GetPlayerExp				(lua_State* L);
int LuaFunc_GetPlayerExtraPoint			(lua_State* L);			// 額外增加點數
int LuaFunc_GetPlayerExpDebt			(lua_State* L);			// 取得負債經驗值
int LuaFunc_GetPlayerMoney				(lua_State* L);
int	LuaFunc_GetZoneID					(lua_State* L);
int LuaFunc_GetScreenWidth				(lua_State* L);
int LuaFunc_GetScreenHeight				(lua_State* L);
int	LuaFunc_GetMouseMoveOffset			(lua_State* L);
int LuaFunc_GetPlayerCombatState		(lua_State* L);
int LuaFunc_IsShiftKeyDown				(lua_State* L);
int LuaFunc_IsAltKeyDown				(lua_State* L);
int LuaFunc_IsCtrlKeyDown				(lua_State* L);

int	LuaFunc_SpellIsTargeting			(lua_State* L);
int	LuaFunc_SpellTargetUnit				(lua_State* L);
int	LuaFunc_SpellStopCasting			(lua_State* L);
int	LuaFunc_TargetUnit					(lua_State* L);
int	LuaFunc_FocusUnit					(lua_State* L);
int	LuaFunc_DropItemOnUnit				(lua_State* L);

int	LuaFunc_HasSelfRevive				(lua_State* L);
int	LuaFunc_UseSelfRevive				(lua_State* L);
int LuaFunc_GetResurrectTimeRemaining	(lua_State* L);
int LuaFunc_BrithRevive					(lua_State* L);
int LuaFunc_AcceptResurrect				(lua_State* L);

int	LuaFunc_PlaySoundByPath				(lua_State* L);
int	LuaFunc_SetFindPartyState			(lua_State* L);
int	LuaFunc_GetFindPartyState			(lua_State* L);

int	LuaFunc_FollowUnit					(lua_State* L);
int LuaFunc_AssistUnit					(lua_State* L);

int LuaFunc_MoveForwardStart			(lua_State* L);
int LuaFunc_MoveForwardStop				(lua_State* L);
int LuaFunc_MoveBackwardStart			(lua_State* L);
int LuaFunc_MoveBackwardStop			(lua_State* L);
int LuaFunc_TurnRightStart				(lua_State* L);
int LuaFunc_TurnRightStop				(lua_State* L);
int LuaFunc_TurnLeftStart				(lua_State* L);
int LuaFunc_TurnLeftStop				(lua_State* L);
int LuaFunc_StrafeRightStart			(lua_State* L);
int LuaFunc_StrafeRightStop				(lua_State* L);
int LuaFunc_StrafeLeftStart				(lua_State* L);
int LuaFunc_StrafeLeftStop				(lua_State* L);
int LuaFunc_Jump						(lua_State* L);
int LuaFunc_ToggleAutoRun				(lua_State* L);
int LuaFunc_ToggleRun					(lua_State* L);
int LuaFunc_ToggleSheath				(lua_State* L);
int LuaFunc_SitOrStand					(lua_State* L);

int	LuaFunc_TargetNearestEnemy			(lua_State* L);
int	LuaFunc_TargetNearestFriend			(lua_State* L);

int LuaFunc_ResetTutorialTrigger		(lua_State* L);

// Duel(PK)
int LuaFunc_StartDuelUnit				(lua_State* L);
int LuaFunc_AcceptDuel					(lua_State* L);
int LuaFunc_CancelDuel					(lua_State* L);
int LuaFunc_PlayerDuelState				(lua_State* L);

int LuaFunc_TakeScreenshot				(lua_State* L);
int LuaFunc_GetSelectedRealmState		(lua_State* L);
int LuaFunc_SetSelectedRealmState		(lua_State* L);
int LuaFunc_GetLastAccountState			(lua_State* L);
int LuaFunc_SetLastAccountState			(lua_State* L);

// 取得遊戲遊戲最大時限
int LuaFunc_GetPlayTimeQuota			(lua_State* L);
int LuaFunc_IsEnterWorld				(lua_State* L);
int LuaFunc_ReloadUI					(lua_State* L);

// 測試用
int LuaFunc_SetCameraMoveTo				(lua_State* L);			// 移動到目標點
int LuaFunc_SetCameraPosition			(lua_State* L);			// 鏡頭所在位置
int LuaFunc_SetTargetCamera				(lua_State* L);			// 鏡頭目的位置
int LuaFunc_SetCameraUpVector			(lua_State* L);			// 鏡頭上方所朝方向
int LuaFunc_GetCameraMoveTime			(lua_State* L);			// 鏡頭移動剩於時間
int LuaFunc_GetCameraPosition			(lua_State* L);			// 鏡頭移動剩於時間
int LuaFunc_GetTargetCamera				(lua_State* L);			// 鏡頭移動剩於時間
int LuaFunc_GetCameraUpVector			(lua_State* L);			// 鏡頭移動剩於時間
int LuaFunc_RotateCamera				(lua_State* L);			// 鏡頭移動剩於時間

// Reset UI
int LuaFunc_ResetUI						(lua_State* L);			// 重新設定UI

int LuaFunc_PasswordConfirm				(lua_State* L);			// 確認二次密碼
int LuaFunc_CheckPasswordState			(lua_State* L);			// 確認二次密碼
int LuaFunc_SetHyperlinkCursor			(lua_State* L);			// 游標是否改變為 Hyperlink Type
int LuaFunc_GetTipText					(lua_State* L);			// 取得小提示文字
int LuaFunc_GetKeyboardFocus			(lua_State* L);			// 取得目前輸入焦點
int	LuaFunc_IsMouseEnter				(lua_State* L);

int LuaFunc_GetAccountBookItems			(lua_State* L);			// 最大筆數
int LuaFunc_GetAccountBookEnterMoney	(lua_State* L);			// 取得未入帳的商城幣值
int LuaFunc_GetAccountBookLockMoney		(lua_State* L);			// 取得被鎖定不能使用幣值
int LuaFunc_GetAccountBookTotalMoney	(lua_State* L);			// 總未入帳金額

int LuaFunc_IsBattleGroundZone			(lua_State* L);			// 戰場
int LuaFunc_GetBattleGroundNumPlayers	(lua_State* L);			// 戰場最大人數限制
int LuaFunc_IsPartyEnable				(lua_State* L);			

int LuaFunc_ResetControlArgument		(lua_State* L);			// 恢復介面設定預設值


//轉換字串
int LuaFunc_GetReplaceSystemKeyword  (lua_State* L);

int LuaFunc_SendWedgePointReport		(lua_State* L);			// 卡點回報
int LuaFunc_CancelWedgePointReport		(lua_State* L);			// 取消卡點回報

// 擲骰
int LuaFunc_Roll						(lua_State* L);			// 擲骰

// 取得分流是否存在
int LuaFunc_GetNumZoneChannels			(lua_State* L);			// 取得分流各數
int LuaFunc_IsZoneChannelOnLine			(lua_State* L);			// 該分流是否存在

// 副本重置時間
int LuaFunc_GetInstanceRecordInfo		(lua_State* L);			// 副本進度資訊
int LuaFunc_AcceptInstanceRecord		(lua_State* L);			// 同意記錄副本進度
int LuaFunc_DeclineInstanceRecord		(lua_State* L);			// 不同意記錄副本進度
int LuaFunc_SwitchExtendInstanceRecord	(lua_State* L);			// 開關延長進度

// 雙人坐騎
int LuaFunc_InviteRideMount				(lua_State* L);			// 邀請搭乘雙人坐騎
int LuaFunc_UninviteRideMount			(lua_State* L);			// 取消邀請搭乘雙人坐騎
int LuaFunc_AcceptRideMount				(lua_State* L);			// 同意搭乘
int LuaFunc_DeclineRideMount			(lua_State* L);			// 拒絕搭乘
int LuaFunc_LeaveRideMount				(lua_State* L);			// 離開搭乘
int LuaFunc_CheckMultiMount				(lua_State* L);			// 是否可以多人坐騎

int LuaFunc_SetAggroPrompt				(lua_State* L);			// 是否顯示仇恨表
int LuaFunc_IsAggroPrompt				(lua_State* L);			// 仇恨是否顯示
int LuaFunc_SetAutoOpenGoodsPack		(lua_State* L);			// 開啟背包時自動開啟商城背包
int LuaFunc_IsAutoOpenGoodsPack			(lua_State* L);			// 開啟背包時自動開啟商城背包
int LuaFunc_SetChatDisplayClassColor	(lua_State* L);			// 是否顯示職業顏色
int LuaFunc_IsChatDisplayClassColor		(lua_State* L);			// 是否顯示職業顏色

int LuaFunc_SetHideOtherPlayerEffect	(lua_State* L);			// 是否隱藏玩家特效
int LuaFunc_IsHideOtherPlayerEffect		(lua_State* L);			// 是否隱藏玩家特效

// 載具
int LuaFunc_IsInImplement				(lua_State* L);			// 是否在載具上

int LuaFunc_OpenGMNotification			(lua_State* L);			// 通知開啟網頁

int LuaFunc_DisableBugReport			(lua_State* L);			// 關閉問題回報
int LuaFunc_GetSoulPoint				(lua_State* L);			// 靈能點數
int LuaFunc_MoveTo				(lua_State* L);	
// ----------------------------------------------------------------------------
void GameControl_MouseDown(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseDown(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_MouseUp(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseUp(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_MouseWheel(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseWheel(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_MouseMove(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseMove(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_MouseEnter(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseEnter(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_MouseLeave(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnMouseLeave(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_Click(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnClick(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_DoubleClick(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnDoubleClick(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_DragBegin(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnDragBegin(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_DragStop(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	g_pGameControl->OnDragStop(uiFrame, lParam, rParam);
}
// ----------------------------------------------------------------------------
void GameControl_ReceiveDrag(CUiFrame* uiFrame, INT64 lParam, INT64 rParam)
{
	//g_pGameControl->OnReceiveDrag(uiFrame, lParam, rParam);
}

// ----------------------------------------------------------------------------
CGameControl::CGameControl(CInterface* object)
	: CInterfaceBase(object)
{
	g_pGameControl = this;

	m_rotateCamera			= false;
	m_ignoreOnMouseDown		= false;
	m_isMouseEnter			= false;
	m_LButtonDown			= false;
	m_RButtonDown			= false;
	m_isAutoRun				= false;
	m_mouseOffsetX			= 0.0f;
	m_mouseOffsetY			= 0.0f;
	m_buttonDownTime		= 0.0f;

	m_wedgeLocked			= false;

	m_darkTexture			= NULL;

	m_keystate.clear();
}

// ----------------------------------------------------------------------------
CGameControl::~CGameControl()
{
	g_pGameControl = NULL;
}

// ----------------------------------------------------------------------------
void CGameControl::RegisterFunc()
{
	lua_State* L = GetLuaStateWorld();
	if (L)
	{		
		lua_register(L, "GetLanguage",					LuaFunc_GetLanguage);
		lua_register(L, "GetLocation",					LuaFunc_GetLocation);
		lua_register(L, "GetInstall",					LuaFunc_GetInstall);
		lua_register(L, "GetImageLocation",				LuaFunc_GetImageLocation);
		lua_register(L, "IsHideLoginUI",				LuaFunc_IsHideLoginUI);

		lua_register(L, "Logout",						LuaFunc_Logout);
		lua_register(L, "CancelLogout",					LuaFunc_CancelLogout);
		lua_register(L, "ChangeParallelID",				LuaFunc_ChangeParallelID);
		lua_register(L, "CancelChangeParallelID",		LuaFunc_CancelChangeParallelID);
		
		lua_register(L, "GetUIScale",					LuaFunc_GetUIScale);
		lua_register(L, "GetFramerate",					LuaFunc_GetFramerate);
		lua_register(L, "GetTime",						LuaFunc_GetTime);
		lua_register(L, "GetPing",						LuaFunc_GetPing);
		lua_register(L, "GetCursorPos",					LuaFunc_GetCursorPos);
		lua_register(L, "GetPlayerPosition",			LuaFunc_GetPlayerPosition);
		lua_register(L, "GetPlayerDirection",			LuaFunc_GetPlayerDirection);
		lua_register(L, "GetPlayerMaxExp",				LuaFunc_GetPlayerMaxExp);
		lua_register(L, "GetPlayerExp",					LuaFunc_GetPlayerExp);
		lua_register(L, "GetPlayerExtraPoint",			LuaFunc_GetPlayerExtraPoint);
		lua_register(L, "GetPlayerMoney",				LuaFunc_GetPlayerMoney);
		
		lua_register(L, "IsShiftKeyDown",				LuaFunc_IsShiftKeyDown);
		lua_register(L, "IsAltKeyDown",					LuaFunc_IsAltKeyDown);
		lua_register(L, "IsCtrlKeyDown",				LuaFunc_IsCtrlKeyDown);
		lua_register(L, "GetMouseMoveOffset",			LuaFunc_GetMouseMoveOffset);
		lua_register(L, "GetPlayerCombatState",			LuaFunc_GetPlayerCombatState);
		lua_register(L, "GetZoneID",					LuaFunc_GetZoneID);
		lua_register(L, "GetScreenWidth",				LuaFunc_GetScreenWidth);
		lua_register(L, "GetScreenHeight",				LuaFunc_GetScreenHeight);
		
		lua_register(L, "SpellIsTargeting",				LuaFunc_SpellIsTargeting);
		lua_register(L, "SpellTargetUnit",				LuaFunc_SpellTargetUnit);
		lua_register(L, "SpellStopCasting",				LuaFunc_SpellStopCasting);
		lua_register(L, "DropItemOnUnit",				LuaFunc_DropItemOnUnit);
		lua_register(L, "TargetUnit",					LuaFunc_TargetUnit);
		lua_register(L, "FocusUnit",					LuaFunc_FocusUnit);		
		
		lua_register(L, "HasSelfRevive",				LuaFunc_HasSelfRevive);
		lua_register(L, "UseSelfRevive",				LuaFunc_UseSelfRevive);
		lua_register(L, "GetResurrectTimeRemaining",	LuaFunc_GetResurrectTimeRemaining);
		lua_register(L, "BrithRevive",					LuaFunc_BrithRevive);
		lua_register(L, "AcceptResurrect",				LuaFunc_AcceptResurrect);

		lua_register(L, "PlaySoundByPath",				LuaFunc_PlaySoundByPath);
		lua_register(L, "SetFindPartyState",			LuaFunc_SetFindPartyState);
		lua_register(L, "GetFindPartyState",			LuaFunc_GetFindPartyState);

		lua_register(L, "FollowUnit",					LuaFunc_FollowUnit);
		lua_register(L, "AssistUnit",					LuaFunc_AssistUnit);

		lua_register(L, "MoveForwardStart",				LuaFunc_MoveForwardStart);
		lua_register(L, "MoveForwardStop",				LuaFunc_MoveForwardStop);
		lua_register(L, "MoveBackwardStart",			LuaFunc_MoveBackwardStart);
		lua_register(L, "MoveBackwardStop",				LuaFunc_MoveBackwardStop);
		lua_register(L, "TurnRightStart",				LuaFunc_TurnRightStart);
		lua_register(L, "TurnRightStop",				LuaFunc_TurnRightStop);
		lua_register(L, "TurnLeftStart",				LuaFunc_TurnLeftStart);
		lua_register(L, "TurnLeftStop",					LuaFunc_TurnLeftStop);
		lua_register(L, "StrafeRightStart",				LuaFunc_StrafeRightStart);
		lua_register(L, "StrafeRightStop",				LuaFunc_StrafeRightStop);
		lua_register(L, "StrafeLeftStart",				LuaFunc_StrafeLeftStart);
		lua_register(L, "StrafeLeftStop",				LuaFunc_StrafeLeftStop);

		lua_register(L, "Jump",							LuaFunc_Jump);
		lua_register(L, "ToggleAutoRun",				LuaFunc_ToggleAutoRun);
		lua_register(L, "ToggleRun",					LuaFunc_ToggleRun);
		lua_register(L, "ToggleSheath",					LuaFunc_ToggleSheath);
		lua_register(L, "SitOrStand",					LuaFunc_SitOrStand);

		lua_register(L, "TargetNearestEnemy",			LuaFunc_TargetNearestEnemy);
		lua_register(L, "TargetNearestFriend",			LuaFunc_TargetNearestFriend);

		lua_register(L, "ResetTutorialTrigger",			LuaFunc_ResetTutorialTrigger);

		lua_register(L, "StartDuelUnit",				LuaFunc_StartDuelUnit);
		lua_register(L, "AcceptDuel",					LuaFunc_AcceptDuel);
		lua_register(L, "CancelDuel",					LuaFunc_CancelDuel);
		lua_register(L, "PlayerDuelState",				LuaFunc_PlayerDuelState);

		lua_register(L, "TakeScreenshot",				LuaFunc_TakeScreenshot);
		lua_register(L, "SetSelectedRealmState",		LuaFunc_SetSelectedRealmState);
		lua_register(L, "GetSelectedRealmState",		LuaFunc_GetSelectedRealmState);

		lua_register(L, "GetLastAccountState",			LuaFunc_GetLastAccountState);
		lua_register(L, "SetLastAccountState",			LuaFunc_SetLastAccountState);

		lua_register(L, "GetPlayTimeQuota",				LuaFunc_GetPlayTimeQuota);
		lua_register(L, "IsEnterWorld",					LuaFunc_IsEnterWorld);
		lua_register(L, "ReloadUI",						LuaFunc_ReloadUI);

		// 測試用
		lua_register(L, "SetCameraMoveTo",				LuaFunc_SetCameraMoveTo);
		lua_register(L, "SetCameraPosition",			LuaFunc_SetCameraPosition);
		lua_register(L, "SetTargetCamera",				LuaFunc_SetTargetCamera);
		lua_register(L, "SetCameraUpVector",			LuaFunc_SetCameraUpVector);
		lua_register(L, "GetCameraMoveTime",			LuaFunc_GetCameraMoveTime);

		lua_register(L, "GetCameraPosition",			LuaFunc_GetCameraPosition);
		lua_register(L, "GetTargetCamera",				LuaFunc_GetTargetCamera);
		lua_register(L, "GetCameraUpVector",			LuaFunc_GetCameraUpVector);
		lua_register(L, "RotateCamera",					LuaFunc_RotateCamera);

		lua_register(L, "ResetUI",						LuaFunc_ResetUI);
		lua_register(L, "PasswordConfirm",				LuaFunc_PasswordConfirm);
		lua_register(L, "CheckPasswordState",			LuaFunc_CheckPasswordState);
	
		

		lua_register(L, "SetHyperlinkCursor",			LuaFunc_SetHyperlinkCursor);
		lua_register(L, "GetTipText",					LuaFunc_GetTipText);

		lua_register(L, "GetPlayerExpDebt",				LuaFunc_GetPlayerExpDebt);
		lua_register(L, "GetKeyboardFocus",				LuaFunc_GetKeyboardFocus);

		lua_register(L, "IsMouseEnter",					LuaFunc_IsMouseEnter);
		lua_register(L, "GetAccountBookEnterMoney",		LuaFunc_GetAccountBookEnterMoney);
		lua_register(L, "GetAccountBookLockMoney",		LuaFunc_GetAccountBookLockMoney);		
		lua_register(L, "GetAccountBookTotalMoney",		LuaFunc_GetAccountBookTotalMoney);
		lua_register(L, "GetAccountBookItems",			LuaFunc_GetAccountBookItems);

		lua_register(L, "IsBattleGroundZone",			LuaFunc_IsBattleGroundZone);
		lua_register(L, "GetBattleGroupNumPlayers",		LuaFunc_GetBattleGroundNumPlayers);
		lua_register(L, "IsPartyEnable",				LuaFunc_IsPartyEnable);
		lua_register(L, "ResetControlArgument",			LuaFunc_ResetControlArgument);

		lua_register(L, "GetReplaceSystemKeyword",      LuaFunc_GetReplaceSystemKeyword  );


		lua_register(L, "SendWedgePointReport",			LuaFunc_SendWedgePointReport );
		lua_register(L, "CancelWedgePointReport",		LuaFunc_CancelWedgePointReport );

		lua_register(L, "Roll",							LuaFunc_Roll );

		lua_register(L, "GetNumZoneChannels",			LuaFunc_GetNumZoneChannels );
		lua_register(L, "IsZoneChannelOnLine",			LuaFunc_IsZoneChannelOnLine );

		lua_register(L, "GetInstanceRecordInfo",		LuaFunc_GetInstanceRecordInfo );
		lua_register(L, "AcceptInstanceRecord",			LuaFunc_AcceptInstanceRecord );
		lua_register(L, "DeclineInstanceRecord",		LuaFunc_DeclineInstanceRecord );		
		lua_register(L, "SwitchExtendInstanceRecord",	LuaFunc_SwitchExtendInstanceRecord );

		lua_register(L, "InviteRideMount",				LuaFunc_InviteRideMount );
		lua_register(L, "UninviteRideMount",			LuaFunc_UninviteRideMount );
		lua_register(L, "AcceptRideMount",				LuaFunc_AcceptRideMount );
		lua_register(L, "DeclineRideMount",				LuaFunc_DeclineRideMount );
		lua_register(L, "LeaveRideMount",				LuaFunc_LeaveRideMount );
		lua_register(L, "CheckMultiMount",				LuaFunc_CheckMultiMount );

		lua_register(L, "SetAggroPrompt",				LuaFunc_SetAggroPrompt );
		lua_register(L, "IsAggroPrompt",				LuaFunc_IsAggroPrompt );

		lua_register(L, "SetAutoOpenGoodsPack",			LuaFunc_SetAutoOpenGoodsPack );
		lua_register(L, "IsAutoOpenGoodsPack",			LuaFunc_IsAutoOpenGoodsPack );

		lua_register(L, "SetChatDisplayClassColor",		LuaFunc_SetChatDisplayClassColor );
		lua_register(L, "IsChatDisplayClassColor",		LuaFunc_IsChatDisplayClassColor );

		lua_register(L, "SetHideOtherPlayerEffect",		LuaFunc_SetHideOtherPlayerEffect );
		lua_register(L, "IsHideOtherPlayerEffect",		LuaFunc_IsHideOtherPlayerEffect );

		lua_register(L, "IsInImplement",				LuaFunc_IsInImplement );

		lua_register(L, "OpenGMNotification",			LuaFunc_OpenGMNotification );
		lua_register(L, "DisableBugReport",				LuaFunc_DisableBugReport );
		lua_register(L, "GetSoulPoint",					LuaFunc_GetSoulPoint );
		lua_register(L, "MoveTo",					LuaFunc_MoveTo );
	}
}

// ----------------------------------------------------------------------------
void CGameControl::BuildFrames()
{
	if ( m_interface )
	{
		CUiFrame* root = m_interface->GetUiWorld()->GetRoot();
		CUi* pUi = m_interface->GetUiWorld();

		root->m_pfOnMouseDown	= GameControl_MouseDown;
		root->m_pfOnMouseUp		= GameControl_MouseUp;
		root->m_pfOnMouseWheel	= GameControl_MouseWheel;
		root->m_pfOnMouseMove	= GameControl_MouseMove;
		root->m_pfOnMouseEnter	= GameControl_MouseEnter;
		root->m_pfOnMouseLeave	= GameControl_MouseLeave;
		root->m_pfOnClick		= GameControl_Click;
		root->m_pfOnDoubleClick	= GameControl_DoubleClick;
		root->m_pfOnDragBegin	= GameControl_DragBegin;
		root->m_pfOnDragStop	= GameControl_DragStop;
		root->m_pfOnReceiveDrag	= GameControl_ReceiveDrag;

		// 背景圖形
		m_darkTexture = (CUiTexture*)pUi->CreateObject(CUiLayout::eUiType_Texture, "", root);
		m_darkTexture->SetAnchorSize(CUiSize(0.0f, 0.0f));
		m_darkTexture->SetAnchor(CUiAnchor());
		m_darkTexture->SetFile("interface\\Common\\Block-Fill");
		m_darkTexture->SetColor(0.0f, 0.0f, 0.0f);
		m_darkTexture->SetVisible(false);
		root->SetLayers(CUiFrame::eLayerType_Background, m_darkTexture);
	}

	m_mountInviteName.clear();
	m_mountInviteGItemID = 0;
}

// ----------------------------------------------------------------------------
void CGameControl::DestroyFrames()
{
	m_darkTexture = NULL;
}

// ----------------------------------------------------------------------------
void CGameControl::Update(float elapsedTime)
{
	CRoleSprite* player = g_pGameMain->GetPlayer();

	if ( !g_pGameMain->IsEnterWorld() )
	{
		m_rotateCamera			= false;
		m_mouseOffsetX			= 0.0f;
		m_mouseOffsetY			= 0.0f;
		m_buttonDownTime		= 0.0f;
		return;
	}
	else
	{
		CUiInput* input = m_interface->GetUiInput();

		m_duel.time += elapsedTime;
		switch (m_duel.step)
		{
		case eDuelStep_Request:
			if ( m_duel.time > 30.0f )
			{
				// 取消決鬥邀請
				SetDuelStep(eDuelStep_None, 0);
			}
			break;

		case eDuelStep_Receive:
		case eDuelStep_Refuse:
		case eDuelStep_Accept:
		case eDuelStep_Cancel:
		case eDuelStep_Ready:		
			break;

		case eDuelStep_Start:
			{
				static float lastTime = 0.0f;
				CRoleSprite* player = g_pGameMain->GetPlayer();

				lastTime += elapsedTime;
				if ( player )
				{
					if ( (player->GetPos() - m_duel.centerPos).Magnitude() > m_duel.range )
					{
						if ( lastTime > 1.0f )
						{
							lastTime = 0.0f;
							g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_WARNING_1"));
						}
					}
				}
			}			
			break;

		case eDuelStep_Finish:
			break;
		}
		
		// 取得游標所在物件
		if ( m_isMouseEnter )
		{
			CRuArrayList<CRoleSprite*> ignoreSprite;
			int x = g_pGameMain->GetCursorPtX();
			int y = g_pGameMain->GetCursorPtY();

			// 載具上忽略自己判斷
			if ( player )
			{	
				ignoreSprite.Add(player);
				if ( player->GetImplementSprite() != NULL )
					ignoreSprite.Add(player->GetImplementSprite());
			}

			g_pGameMain->SetMouseInPlayer(g_pGameMain->GetTopRoleSprite(x, y, ignoreSprite));
		}
		else
			g_pGameMain->SetMouseInPlayer(NULL);

		// 滑鼠左右鍵按住持續往前跑
		if ( m_LButtonDown && m_RButtonDown )
		{
			if ( player && player->GetImplementSprite() )
			{
			}
			else
			{
				m_isAutoRun = true;
				g_pGameMain->ToggleAutoRune(1);
			}
		}
		else if ( m_isAutoRun )
		{
			m_isAutoRun = false;
			g_pGameMain->ToggleAutoRune(2);
		}

		// 旋轉鏡頭
		if ( m_rotateCamera )
		{
			float xDelta = input->GetXDelta();
			float yDelta = input->GetYDelta();
			BYTE buttonState = input->GetButtonState(MOUSE_RBUTTON);

			m_mouseOffsetX += xDelta;
			m_mouseOffsetY += yDelta;
			if ( xDelta != 0.0f || yDelta != 0.0f )
			{
				float reverse = 1.0f;
				if ( g_pGameMain->GetCameraYReverse() )
					reverse = -1.0f;				

				g_pTutorailFrame->doState(CTutorialFrame::STATE_MOUSEROTATE);
				g_pGameMain->RotateCamera(xDelta * -0.005f, yDelta * -0.005f * reverse, buttonState & CUiInput::eKey_Press);
			}
		}
	}
}

// ----------------------------------------------------------------------------
bool CGameControl::WndMsgProc(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_CONTROL:
		case VK_MENU:
		case VK_SHIFT:		
		case VK_RETURN:
		case VK_TAB:
		case VK_SPACE:
		case VK_ESCAPE:
			break;

		default:
			OnKeyDown(wParam, lParam);
			{
				MSG msg;
				BOOL boMessage = PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
				if ( boMessage && msg.message == WM_CHAR )
				{
					PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
				}
			}
		}
		return true;

	case WM_CHAR:
	case WM_SYSCHAR:
		switch ( wParam )
		{		
		case VK_RETURN:
		case VK_TAB:
		case VK_SPACE:
		case VK_ESCAPE:
			OnKeyDown(wParam, lParam);
			break;
		}
		return true;
	
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// 特別按鈕無法觸發 KEYDOWN
		/*
		switch (wParam)
		{
	//	case VK_TAB: //CTRL-TAB
	
		case VK_SNAPSHOT:
			{
				int keyState = 0;
				if ( GetKeyState(VK_SHIFT) < 0 )
					keyState |= KEY_BINDING_FLAG_SHIFT;
				if ( GetKeyState(VK_CONTROL) < 0 )
					keyState |= KEY_BINDING_FLAG_CTRL;
				if ( GetKeyState(VK_MENU) < 0 )
					keyState |= KEY_BINDING_FLAG_ALT;

				// 記錄按下時按鍵資訊
				m_keystate.insert(make_pair(wParam, keyState));

				// 執行熱鍵腳本(Lua Script)
				g_pKeyBinding->RunBinding(keyState | wParam, false);
			}
			break;

		default:
			OnKeyUp(wParam, lParam);
		}
		*/
		OnKeyUp(wParam, lParam);
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
void CGameControl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( (lParam & 0x40000000) == 0 )
	{			
		int keyState = 0;
		if ( GetKeyState(VK_SHIFT) < 0 )
			keyState |= KEY_BINDING_FLAG_SHIFT;
		if ( GetKeyState(VK_CONTROL) < 0 )
			keyState |= KEY_BINDING_FLAG_CTRL;
		if ( GetKeyState(VK_MENU) < 0 )
			keyState |= KEY_BINDING_FLAG_ALT;

		// 清除舊有資料,確認按鍵不會重複按下
		// WndMsgProc(WM_KEYUP, wParam, lParam);

		// 記錄按下時按鍵資訊
		m_keystate.insert(make_pair(wParam, keyState));

		// 執行熱鍵腳本(Lua Script)
		g_pKeyBinding->RunBinding(int(keyState | wParam), false);
	}
}

// ----------------------------------------------------------------------------
void CGameControl::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	map<int, int>::iterator iter = m_keystate.find((int)wParam);
	if ( iter != m_keystate.end() )
	{
		g_pKeyBinding->RunBinding(int(iter->second | wParam), true);
		m_keystate.erase(iter);
	}
}

// ----------------------------------------------------------------------------
void CGameControl::OnKillFocus(bool ignoreOnMouseDown)
{
	if ( ignoreOnMouseDown )
		m_ignoreOnMouseDown = true;

	for ( map<int, int>::iterator iter = m_keystate.begin(); iter != m_keystate.end(); iter++ )
		g_pKeyBinding->RunBinding(iter->second | iter->first, true);
	m_keystate.clear();
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseDown(CUiFrame* frame, INT64 lParam, INT64 rParam)
{	
	if ( m_ignoreOnMouseDown )
	{
		m_ignoreOnMouseDown = false;
		return;
	}

	// 視窗模式,按鍵超出畫面不接收
	if ( g_pGameMain->IsWindowed() ) {
		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( g_pGameMain->GetWnd(), &pt );
		if ( pt.x < 0 || pt.x > g_pGameMain->GetScreenWidth() || pt.y < 0 || pt.y > g_pGameMain->GetScreenHeight() )
			return;
	}

	switch(lParam)
	{
	case MOUSE_LBUTTON:		
	case MOUSE_RBUTTON:
		if ( lParam == MOUSE_LBUTTON )
			m_LButtonDown = true;
		else if ( lParam == MOUSE_RBUTTON )
			m_RButtonDown = true;

		if ( g_pGameMain->GetLButtonCameraRotateEnable() == false && lParam == MOUSE_LBUTTON )
		{
			return;
		}

		// 當左鍵或右鍵被按下時,代表將旋轉視角
		if ( !m_rotateCamera )
		{
			m_rotateCamera		= true;
			m_mouseOffsetX		= 0.0f;
			m_mouseOffsetY		= 0.0f;
			m_buttonDownTime	= (float)g_pGameMain->GetFrameTime();

			g_pGameMain->LockCursorPos();
			g_pGameMain->SetCursorVisible(false);
		}
		break;

	case MOUSE_MBUTTON:
	case MOUSE_XBUTTON:
		break;
	}
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseUp(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	bool unlock = false;
	switch(lParam)
	{
	case MOUSE_LBUTTON:
		if ( frame->GetMouseDown(MOUSE_RBUTTON) != frame )
			unlock = true;
		if ( m_LButtonDown )
			m_LButtonDown = false;
		break;

	case MOUSE_RBUTTON:
		if ( frame->GetMouseDown(MOUSE_LBUTTON) != frame )
			unlock = true;
		if ( m_RButtonDown )
			m_RButtonDown = false;
		break;

	case MOUSE_MBUTTON:
	case MOUSE_XBUTTON:
		break;
	}

	if ( unlock )
	{
		m_rotateCamera		= false;
		m_mouseOffsetX		= 0.0f;
		m_mouseOffsetY		= 0.0f;

		g_pGameMain->UnlockCursorPos();
		g_pGameMain->SetCursorVisible(true);
	}
}

// ----------------------------------------------------------------------------
void CGameControl::OnClick(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	CRoleSprite* current	= g_pGameMain->GetMouseInPlayer();
	CRoleSprite* player		= g_pGameMain->GetPlayer();
	float frameTime			= (float)g_pGameMain->GetFrameTime();
	float offset			= (float)sqrt((m_mouseOffsetX*m_mouseOffsetX)+(m_mouseOffsetY*m_mouseOffsetY));

	switch (lParam)
	{
	case MOUSE_LBUTTON:
		// 滑鼠左鍵關閉鏡頭旋轉(並且觸發動作)
		if ( g_pGameMain->GetLButtonCameraRotateEnable() && (frameTime - m_buttonDownTime > 0.6f || offset > 5.0f) )
			return;

		// 範圍法術
		if ( g_pGameMain->GetCastSpellState() != CASTSPELL_NONE && g_pGameMain->SetBeginSpell(current) )
			return;
		// 物品觸發範圍法術
		if ( g_pBagFrame->GetUseItemToLocationID() != -1 )
		{
			g_pGameMain->UseBagItem(g_pBagFrame->GetUseItemToLocationID());
			return;
		}

		if ( g_pItemClipboard->GetType() != eClipboardItem_Type_None )
		{
			OnReceiveDrag(frame, lParam, rParam);
		}
		else if (player)
		{
			CRoleSprite* oldTarget	= player->GetTargetSprite();
			bool mouseMoveEnable	= g_pGameMain->GetMouseMoveEnable();

			if ( current )
			{			
				g_pGameMain->SetPlayerTarget(current);
			}
			else if ( g_pGameMain->GetLButtonCancelTarget() )
				g_pGameMain->SetPlayerTarget(NULL);
			
			if ( current )
			{
				if ( current == oldTarget || (current->HasQuestEvent() && !current->GetTargetState()) || current->GetCorpseState() ||
					current->GetCheckedStatus() == eChecked_FurnitureStorage || 
					current->GetCheckedStatus() == eChecked_FurnitureNormal  ||
					current->GetCheckedStatus() == eChecked_FurnitureSound   ||
					current->GetCheckedStatus() == eChecked_GuildHousesBuildPoint ) // 公會屋參考點
				{
					if ( mouseMoveEnable )
					{
						// 移動到目標旁並執行
						g_pGameMain->SetMoveToPoint(current->GetPos(), current);
					}
					else
						g_pGameMain->OnClick(current);
				
				}
			}
			else if ( mouseMoveEnable )
			{
				// 移動到空白地方
				int x = g_pGameMain->GetCursorPtX();
				int y = g_pGameMain->GetCursorPtY();

				g_pTutorailFrame->doState(CTutorialFrame::STATE_MOUSEMOVE);
				g_pGameMain->MoveToScreenPoint(x, y);
			}
		}
		break;

	case MOUSE_RBUTTON:
		if ( frameTime - m_buttonDownTime > 0.6f || offset > 10.0f )
			return;		

		switch (g_pGameMain->GetCastSpellState())
		{
		case CASTSPELL_WAITING_LOCATION:								// 清除範圍施法選取
		case CASTSPELL_WAITING_TARGET:
		case CASTSPELL_WAITING_LOCATION_EXTRA:
		case CASTSPELL_WAITING_TARGET_EXTRA:
			g_pGameMain->SetCastSpellState(CASTSPELL_NONE, true);
			break;

		default:
			if ( g_pItemClipboard->GetType() != eClipboardItem_Type_None )
				g_pItemClipboard->Clear();								// 清除拿起物品
			else if ( g_pBagFrame->GetUseItemIndex() != -1 )
				g_pBagFrame->UseItemCancel();							// 取消物品使用
			else if ( g_pBagFrame->GetUseItemToLocationID() != -1 )		
				g_pBagFrame->CancelUseItemToLocation();					// 取消物品使用
			else if ( g_pStoreFrame->GetRepairState() )
				g_pStoreFrame->SetRepairState(false);					// 取消修理狀態
			else if ( g_pGameMain->GetCastSpellToItemID() != 0 )
				g_pGameMain->ClearCastSpellToItem();					// 取消法術對物品使用
			else if ( g_pGameMain->GetRButtonCancelTarget() )
				g_pGameMain->SetPlayerTarget(NULL);						// 取消目標
			else if ( current )
			{
				g_pGameMain->SetPlayerTarget(current);
				g_pGameMain->OnClick(current);	
			}
		}		
		break;

	case MOUSE_MBUTTON:
	case MOUSE_XBUTTON:
		break;
	}
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseWheel(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	CRuCamera* camera = g_pGameMain->GetCamera();
	if (camera)
	{
		float zDelta = lParam * 0.02f;
		g_pGameMain->ZoomCamera(zDelta);
	}	
}

// ----------------------------------------------------------------------------
void CGameControl::OnDoubleClick(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseMove(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseEnter(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	m_isMouseEnter = true;
}

// ----------------------------------------------------------------------------
void CGameControl::OnMouseLeave(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	m_isMouseEnter = false;
	g_pGameControl->SendWorldEvent("UPDATE_MOUSE_LEAVE");
}

// ----------------------------------------------------------------------------
void CGameControl::OnDragBegin(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
}

// ----------------------------------------------------------------------------
void CGameControl::OnDragStop(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
}

// ----------------------------------------------------------------------------
void CGameControl::OnReceiveDrag(CUiFrame* frame, INT64 lParam, INT64 rParam)
{
	CRoleSprite* player = g_pGameMain->GetMouseInPlayer();
	lua_State* L = GetLuaStateWorld();

	ClipboardItemInfo itemInfo = g_pItemClipboard->GetItemInfo();
	ItemFieldStruct* pItemField = NULL;
	switch (g_pItemClipboard->GetType())
	{
	case eClipboardItem_Type_Bag:
		if (player && player->GetObjectType() == eSpriteObject_Player) {
			// open trade
			// 現在雙方確認暫時不做
		} else if ( (pItemField = CNetGlobal::RoleData()->GetBodyItem(itemInfo.bag.pos)) != NULL ) {
			DeleteItemConfirm(pItemField);
		}
		break;

	case eClipboardItem_Type_Bank:
		if ( (pItemField = CNetGlobal::RoleData()->GetBankItem(itemInfo.bank.pos)) != NULL ) {
			DeleteItemConfirm(pItemField);
		}
		break;

	case eClipboardItem_Type_Equipment:
		if (player && player->GetObjectType() == eSpriteObject_Player) {
			// open trade
			// 現在雙方確認暫時不做
		} else if ( (pItemField = CNetGlobal::RoleData()->GetEqItem(itemInfo.equipment.pos)) != NULL ) {
			DeleteItemConfirm(pItemField);
		}
		break;	

	case eClipboardItem_Type_Skill:
	case eClipboardItem_Type_Action:
	case eClipboardItem_Type_Emote:	
	case eClipboardItem_Type_HappyPartner:
		g_pItemClipboard->Clear();
		break;
	case eClipboardItem_Type_SuitSkill:
		g_pItemClipboard->Clear();
		break;
	case eClipboardItem_Type_SkillPlate:
		g_pSuitSkillFrame->SkillPlateDelete();
		break;



	

	case eClipboardItem_Type_Shop:
	case eClipboardItem_Type_Trade:
		break;
	case eClipboardItem_Type_House:
		{
			HouseItemDBStruct* pHouseItemDB = g_pHousesFrame->GetHousesItemDB( itemInfo.house.parentID , itemInfo.house.pos );
			if( pHouseItemDB )
			{
				DeleteItemConfirm(&pHouseItemDB->Item);				
   			}
		}
		break;
 	case eClipboardItem_Type_CoolClothBag:
		{
			int index = itemInfo.CoolClothBag.index;
			if( index < 0 || index >= _DEF_MAX_COOL_CLOTH_COUNT_ )
			{
				return ;
			}
			PlayerBaseRoleData* PlayerBaseData = CNetGlobal::RoleData()->PlayerBaseData;
			int ImageObjectID = PlayerBaseData->CoolClothList[index];

			ItemFieldStruct Item;
			Item.Init();
			Item.OrgObjID = ImageObjectID;
			Item.ImageObjectID = ImageObjectID;
			Item.Count = 1;

			DeleteItemConfirm( &Item );	
		}
		break;
	}
}

// ----------------------------------------------------------------------------
void CGameControl::SetDuelStep(DuelStepType step, int target)
{
	CRoleSprite* sprite = NULL;
	CRoleSprite* player = g_pGameMain->GetPlayer();
	char buf[512];

	switch(step)
	{
	case eDuelStep_None:
		break;

	case eDuelStep_Request:				// 要求決鬥
		if ( m_duel.step != eDuelStep_None )
		{
			// 你不能這麼做
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_1"));
			return;
		}

		// 取得角色資料
		sprite = g_pGameMain->FindSpriteByID(target);
		if ( sprite == NULL )
		{
			// 角色不存在
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_NOTEXIST"));
			return;
		}

		// 戰場中
		if ( g_pGameMain->IsDisableDuel() )
		{
			// 不能比試
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERR_INBATTLEGROUND"));
			return;
		}

		if ( g_pGameMain->GetDistanceWithPlayer(sprite) > 200.0f )
		{
			// 距離過遠
			sprintf(buf, g_ObjectData->GetString("SYS_DUEL_ERROR_DISTANCE"), sprite->GetName());
			g_pGameMain->SendWarningMsg(buf);
			return;
		}

		// 檢查等級差距
		if ( player )
		{			
			if ( player->GetLevel() - sprite->GetLevel() > 10 )
			{
				// 等級差距過大
				sprintf(buf, g_ObjectData->GetString("SYS_DUEL_ERROR_LEVEL"), sprite->GetName());
				g_pGameMain->SendWarningMsg(buf);
				return;
			}
		}

		// 送出要求
		sprintf(buf, g_ObjectData->GetString("SYS_DUEL_SEND_REQUEST"), sprite->GetName());
		g_pChatFrame->SendMsgEvent(CHAT_MSG_SYSTEM, buf, "");
		NetCli_PK::S_PKInviteRequest(target);
		break;

	case eDuelStep_Receive:			// 收到對方要求
		if ( m_duel.step != eDuelStep_None )
		{			
			// 忙錄中
			NetCli_PK::S_PKInviteResult(target, false);
			return;
		}

		// 取得角色資料
		sprite = g_pGameMain->FindSpriteByID(target);
		if ( sprite == NULL )
		{
			// 角色不存在
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_NOTEXIST"));
			return;
		}

		// 開啟界面
		sprintf(buf, g_ObjectData->GetString("SYS_DUEL_REQUEST"), sprite->GetName());
		g_pChatFrame->SendMsgEvent(CHAT_MSG_SYSTEM, buf, "");
		{
			lua_State* L = GetLuaState();
			lua_pushstring(L, sprite->GetName());
			lua_setglobal(L, "arg1");
			SendWorldEvent("DUEL_REQUESTED");
		}
		break;

	case eDuelStep_Refuse:			// 收到拒絕
		if ( m_duel.step != eDuelStep_Request )
		{
			// 你不能這麼做
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_1"));
			return;
		}
		step = eDuelStep_None;
		break;

	case eDuelStep_Accept:			// 同意
		if ( m_duel.step != eDuelStep_Request && m_duel.step != eDuelStep_Receive )
		{
			// 你不能這麼做
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_1"));
			return;
		}

		// 取得角色資料
		sprite = g_pGameMain->FindSpriteByID(m_duel.target);
		if ( sprite == NULL )
		{
			// 角色資料有誤
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DUEL_ERROR_NOTEXIST"));

			// 取消決鬥
			SetDuelStep(eDuelStep_Cancel, 0);
			return;
		}

		// 送出同意
		target = m_duel.target;
		NetCli_PK::S_PKInviteResult(m_duel.target, true);
		break;

	case eDuelStep_Cancel:			// 取消
		target = m_duel.target;
		switch ( m_duel.step )
		{
		case eDuelStep_None:
		case eDuelStep_Cancel:
			break;		

		case eDuelStep_Request:			// 要求決鬥,不能取消
			return;

		case eDuelStep_Receive:			// 拒?F對方要求
			NetCli_PK::S_PKInviteResult(m_duel.target, false);
			step = eDuelStep_None;
			break;

		// 已經進入決鬥,取消決鬥		
		case eDuelStep_Start:
		case eDuelStep_Finish:
		case eDuelStep_Accept:
		case eDuelStep_Ready:
			// 你取消決鬥
			g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", "%s %s", g_ObjectData->GetString("SYS_YOU"), g_ObjectData->GetString("SYS_DUEL_CANCEL"));

			NetCli_PK::S_PKCancel();
			break;
		}
		step = eDuelStep_None;
		break;

	case eDuelStep_Ready:			// 準備開始		
		target = m_duel.target;

		// 訊息通知開始
		g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_DUEL_AGREE"), m_duel.targetName.c_str());		

		// 開啟界面
		SendWorldEvent("DUEL_READY");
		break;

	case eDuelStep_Start:			// 決鬥中
		target = m_duel.target;
		// 訊息通知
		g_pChatFrame->SendMsgEvent(CHAT_MSG_SYSTEM, g_ObjectData->GetString("SYS_DUEL_START"), "");

		// 更新角色狀態,需要確定下列參數值為正確
		m_duel.step	= step;

		// 設定比試角色狀態
		if ( (sprite = g_pGameMain->FindSpriteByID(target)) != NULL )
			sprite->SetDuelState(true);
		break;

	case eDuelStep_Finish:			// 結束
		step = eDuelStep_None;

		// 取得角色資料
		if ( (sprite = g_pGameMain->FindSpriteByID(m_duel.target)) != NULL )
		{
			m_duel.step		= eDuelStep_None;
			m_duel.target	= 0;
			m_duel.time		= 0.0f;

			sprite->SetDuelState(false);
		}
		break;
	}

	if( step == eDuelStep_Start )
	{
 		g_pMinimapFrame->ShowDuelRange( m_duel.centerPos.x , m_duel.centerPos.z , m_duel.range );
	}
	else
	{
		g_pMinimapFrame->HideDuelRange();
	}

	// 取得對方名稱,避免超出畫面無法取得
	if ( sprite )
		m_duel.targetName = sprite->GetName();

	m_duel.step		= step;
	m_duel.target	= target;
	m_duel.time		= 0.0f;
}

// ----------------------------------------------------------------------------------
void CGameControl::SendWedgeReport()
{
	CRoleSprite* sprite = g_pGameMain->GetPlayer();	
	
	if ( sprite && sprite->GetDeadState() )
	{
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_WEDGE_PLAYER_DEAD_ERROR"));
		return;
	}

	if ( g_pGameMain->IsBattleGroundZone() )
		return;

	if ( m_wedgeLocked )
		return;

	m_wedgeLocked = true;

	// 關閉自動向前跑
	g_pGameMain->ToggleAutoRune(2);

	CRoleSprite* player = g_pGameMain->GetPlayer();

	CRuVector3 position(0.0f, 0.0f, 0.0f);	

	if ( player )
	{
		position = player->GetPos();

		if ( !player->GetDeadState() )
		{
			player->SetActorState(ruFUSION_ACTORSTATE_SIT_BEGIN);
		}
	}

	// 送出封包
	Net_Move::S_UnlockRolePosRequest( position.m_x, position.m_y, position.m_z, false );
}

// ----------------------------------------------------------------------------------
void CGameControl::CancelWedgeReport()
{
	Net_Move::S_UnlockRolePosRequest( 0.0f, 0.0f, 0.0f, true );
}

// ----------------------------------------------------------------------------------
void CGameControl::R_UnlockRolePosOK( bool Result )
{
	m_wedgeLocked = false;

	if ( g_pInterface )
	{
		g_pInterface->SendWorldEvent("FORCE_DIALOG_HIDE");
	}
}

// ----------------------------------------------------------------------------------
void CGameControl::R_PartyDice(int PlayerDBID, int Type, int RandValue)
{
	if ( Type == 0 )
	{
		// 取得角色名稱
		PartyMemberInfo* member = NetCli_PartyChild::GetGroupMemberByDBID(PlayerDBID);
		if ( member && member->sprite )
		{
			CRoleSprite* sprite = (CRoleSprite*)member->sprite;
			g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM_VALUE, "", g_ObjectData->GetString("SYS_ROLL_POINT"), sprite->GetName(), (abs(RandValue) % 100) + 1);
		}
	}
}

// ----------------------------------------------------------------------------------
void CGameControl::R_InviteMount(const char* name, int GItemID)
{
	if ( m_mountInviteGItemID == 0 )
	{
		lua_State* L = GetLuaState();

		if ( L )		
		{
			m_mountInviteName = name;
			m_mountInviteGItemID = GItemID;

			lua_pushstring(L, name);
			lua_setglobal(L, "arg1");

			SendWorldEvent("RIDE_INVITE_REQUEST");
		}
	}
	else
	{
		NetCli_Channel::S_CtoC_RideMountResult(name, g_pGameMain->GetPlayerName(), 2);
	}
}

// ----------------------------------------------------------------------------
void CGameControl::AnswerInviteMount(bool accept)
{
	if ( m_mountInviteGItemID != 0 )
	{
		CRoleSprite* player = g_pGameMain->GetPlayer();

		if ( player )
		{
			if ( accept )
			{
				CRoleSprite* sprite = g_pGameMain->FindSpriteByID(m_mountInviteGItemID);
				CRoleSprite* proxyControlSprite = (sprite) ? sprite->GetProxyControlSprite() : NULL;
				int carryCount = (sprite) ? sprite->GetMountCarryCount(): 0;								// 可以乘載數量
				int spriteCount = (proxyControlSprite) ? proxyControlSprite->GetLinkSpriteCount() - 1: 0;	// 目前已經乘載數量

				if ( sprite == NULL || g_pGameMain->GetDistanceWithPlayer(sprite) > MAX_MULTI_RIDE_RANGE )
				{
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_TARGET_FAR"));
				}
				else if ( sprite->GetRideState() == false || sprite->GetAttachParentId() != 0 )
				{
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ERROR"));
				}
				else if ( carryCount <= 0 || carryCount <= spriteCount )
				{
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ERROR_2"));
				}
				else if ( player->GetRuTempActor() != NULL )
				{
					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ERROR_1"));
				}
				else
				{
					Net_Move::S_AttachObjRequest( player->GetIdentify(), sprite->GetIdentify(), "p_mount01", "p_mount02", 2);
				}
			}
			else
			{
				g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_MOUNT_SELF_DECLINE"), m_mountInviteName.c_str());
				NetCli_Channel::S_CtoC_RideMountResult(m_mountInviteName.c_str(), player->GetName(), 1);
			}
		}
	}		

	m_mountInviteGItemID = 0;
	m_mountInviteName.clear();
}

// ----------------------------------------------------------------------------
void CGameControl::LeaveMount()
{
	CRoleSprite* player = g_pGameMain->GetPlayer();
	if ( player && player->GetAttachParentId() != 0 )
	{
		CRoleSprite* sprite = g_pGameMain->FindSpriteByID(player->GetAttachParentId());
		if ( sprite )
		{
			Net_Move::S_DetachObjRequest( player->GetIdentify(), sprite->GetIdentify() );
		}
	}
}

/*
// ----------------------------------------------------------------------------
void CGameControl::SetScripts()
{
	CUi* ui = m_interface->GetUiWorld();
	if (ui)
	{
		m_pUiWorldFrame = (CUiFrame*)ui->FindObject("WorldFrame");
		if (m_pUiWorldFrame)
		{			
			m_pUiWorldFrame->SetMouseEnable(true);
			m_pUiWorldFrame->RegisterForClicks(CUiFrame::eMouse_Flag_LButton | CUiFrame::eMouse_Flag_RButton);

			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_MouseDown,		NULL, UiWorld_OnMouseDown);
			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_MouseUp,		NULL, UiWorld_OnMouseUp);
			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_Click,			NULL, UiWorld_OnMouseClick);
			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_MouseWheel,	NULL, UiWorld_OnMouseWheel);			
			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_ReceiveDrag,	NULL, UiWorld_OnReceiveDrag);
			m_pUiWorldFrame->SetScripts(CUiFrame::eEventType_EnterPressed,	NULL, UiWorld_OnEnterPressed);
		}
	}
}
*/

// ----------------------------------------------------------------------------------
void CGameControl::DeleteItemConfirm(ItemFieldStruct* item)
{
	char itemName[256];
	if ( item == NULL || item->IsEmpty() )
		return;

	if ( item->Mode.ItemLock ) {
		g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_DELETE_ITEM_ERROR_LOCKED"));
		return;
	}
	
	RoleDataEx::GetItemFieldName(item, itemName, 256);
	lua_State* L = GetLuaState();
	if ( L ) {
		GameObjDbStructEx* itemDB = CNetGlobal::GetObj(item->OrgObjID);

		lua_pushstring(L, itemName);
		lua_setglobal(L, "arg1");

		if ( itemDB && itemDB->Classification == EM_ObjectClass_Item && itemDB->Item.ItemType == EM_ItemType_Plot )	{
			SendWorldEvent("DELETE_QUESETITEM_CONFIRM");
		} else {
			SendWorldEvent("DELETE_ITEM_CONFIRM");
		}		
	}
}

// ----------------------------------------------------------------------------------
int LuaFunc_GetLanguage	(lua_State* L)
{
	lua_pushstring(L, g_pGameMain->GetLanguage());
	return 1;
}
// ----------------------------------------------------------------------------------
int LuaFunc_GetInstall(lua_State* L)
{
	lua_pushstring(L, g_pGameMain->GetInstall());
	return 1;
}

// ----------------------------------------------------------------------------------
int LuaFunc_GetLocation(lua_State* L)
{
	lua_pushstring(L, g_pGameMain->GetCountry());
	return 1;
}

// ----------------------------------------------------------------------------------
int LuaFunc_GetImageLocation(lua_State* L)
{
	const char* section = NULL;
	if ( lua_isstring(L, 1) )
		section = lua_tostring(L, 1);

	lua_pushstring(L, g_pGameMain->GetImageLocation(section));
	return 1;
}

// ----------------------------------------------------------------------------------
int LuaFunc_IsHideLoginUI(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetUpdateInfo().hideLoginUI);
	return 1;
}

// ----------------------------------------------------------------------------------
int LuaFunc_Logout(lua_State* L)
{	
	g_pGameMain->SetCampState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_CancelLogout(lua_State* L)
{
	g_pGameMain->SetCampState(false);	
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_ChangeParallelID(lua_State* L)
{
	int parallel = luaL_checkint(L, 1) - 1;
	if ( parallel != g_pAccountLogin->GetParalleZoneID() )
	{
		g_pGameMain->ChangeParallelID(parallel);
	} else {
		g_pGameMain->SendWarningMsgEx(g_ObjectData->GetString("SYS_PARALLEL_ALREADY_ID"), parallel + 1);
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_CancelChangeParallelID(lua_State* L)
{
	g_pGameMain->ChangeParallelID(-1);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetUIScale(lua_State* L)
{
	lua_pushnumber(L, g_pInterface->GetScale());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetFramerate(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetFramerate());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetTime(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetFrameTime());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetPing(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetPing());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetCursorPos(lua_State* L)
{
	CUiInput* input = g_pInterface->GetUiInput();
	if ( input )
	{
		float x = (float)input->GetXPos();
		float y = (float)input->GetYPos();

		// 如果參數存在,取得縮放後游標位置
		if ( lua_isboolean(L, 1) ) {
			float scale = g_pInterface->GetScale();
			x /= scale;
			y /= scale;
		}
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		return 2;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerMaxExp(lua_State* L)
{
	lua_pushnumber(L, CNetGlobal::RoleData()->GetLVExp( CNetGlobal::RoleData()->BaseData.Voc ) );
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerExp(lua_State* L)
{
	lua_pushnumber(L, CNetGlobal::RoleData()->GetExp( CNetGlobal::RoleData()->BaseData.Voc ) );
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerExtraPoint(lua_State* L)
{
	lua_pushnumber(L, CNetGlobal::RoleData()->TempData.Attr.Ability->AddExp );
	lua_pushnumber(L, CNetGlobal::RoleData()->TempData.Attr.Ability->AddTp );
	return 2;
}
// ----------------------------------------------------------------------------
// 取得負債經驗值
int LuaFunc_GetPlayerExpDebt(lua_State* L)
{
	RoleDataEx* role = CNetGlobal::RoleData();

	if ( role->BaseData.Voc < EM_Vocation_GameMaster )
	{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	else
	{
		lua_pushnumber(L, role->PlayerBaseData->AbilityList[role->BaseData.Voc].DebtExp);
		lua_pushnumber(L, role->PlayerBaseData->AbilityList[role->BaseData.Voc].DebtTP);
	}

	if ( role->BaseData.Voc_Sub < EM_Vocation_GameMaster )
	{
		lua_pushnil(L);
		lua_pushnil(L);
	}
	else
	{
		lua_pushnumber(L, role->PlayerBaseData->AbilityList[role->BaseData.Voc_Sub].DebtExp);
		lua_pushnumber(L, role->PlayerBaseData->AbilityList[role->BaseData.Voc_Sub].DebtTP);
	}
	return 4;
}
// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerMoney(lua_State* L)
{
	const char* moneyType = lua_tostring(L, 1);
	if ( moneyType == NULL )
		return 0;
	else if ( strcmp(moneyType, MONEY_MODE_COPPER) == 0 )
		lua_pushnumber(L, CNetGlobal::RoleData()->BodyMoney());
	else if ( strcmp(moneyType, MONEY_MODE_ACCOUNT) == 0 )
		lua_pushnumber(L, CNetGlobal::RoleData()->PlayerBaseData->Body.Money_Account);
	else if ( strcmp(moneyType, MONEY_MODE_BONUS) == 0 )
		lua_pushnumber(L, CNetGlobal::RoleData()->PlayerBaseData->Money_Bonus);
	else if ( strcmp(moneyType, MONEY_MODE_BILLDIN) == 0 )
		lua_pushnumber(L, g_pBagFrame->GetBilldinCount());
	else
		lua_pushnumber(L,0);
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetZoneID(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetZoneID());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetScreenWidth(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetUIWidth());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetScreenHeight(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetUIHeight());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetPlayerPosition(lua_State* L)
{
	RoleDataEx* roleData = CNetGlobal::RoleData();
	CRoleSprite* player = g_pGameMain->GetPlayer();
	if ( player && roleData && roleData->PlayerBaseData->ManageLV > EM_Management_Nornal )
	{
		CRuVector3 pos = player->GetPos();
		lua_pushnumber(L, pos.m_x);
		lua_pushnumber(L, pos.m_y);
		lua_pushnumber(L, pos.m_z);		
		return 3;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerDirection(lua_State* L)
{
	RoleDataEx* roleData = CNetGlobal::RoleData();
	CRoleSprite* player = g_pGameMain->GetPlayer();
	if ( player && roleData && roleData->PlayerBaseData->ManageLV > EM_Management_Nornal )
	{
		CRuVector3 dir = player->GetDirection();
		dir.m_y = 0.0f;
		dir.Normalize();

		// 取得角色所朝方向
		float theta = acos( dir.Dot( CRuVector3(1.0f, 0.0f, 0.0f) ) );
		if ( dir.m_z > 0.0f )
			theta = PI + PI - theta;

		// 轉換為角度
		theta = theta * 180.0f / PI;
		lua_pushnumber(L, theta);
		lua_pushnumber(L, 0.0f);
		lua_pushnumber(L, 0.0f);
		return 3;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_IsShiftKeyDown(lua_State* L)
{
	lua_pushboolean(L, GetKeyState(VK_SHIFT) < 0);
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_IsAltKeyDown(lua_State* L)
{
	lua_pushboolean(L, GetKeyState(VK_MENU) < 0);
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_IsCtrlKeyDown(lua_State* L)
{
	lua_pushboolean(L, GetKeyState(VK_CONTROL) < 0);
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_GetMouseMoveOffset(lua_State* L)
{
	CUiInput* input = g_pInterface->GetUiInput();
	if (input)
	{		
		lua_pushnumber(L, input->GetXDelta());
		lua_pushnumber(L, input->GetYDelta());
		return 2;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetPlayerCombatState(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetCombatState());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_SpellIsTargeting(lua_State* L)
{
	switch ( g_pGameMain->GetCastSpellState() )
	{
	case CASTSPELL_WAITING_TARGET:
	case CASTSPELL_WAITING_TARGET_EXTRA:
		lua_pushboolean(L, true);
		break;

	default:
		lua_pushboolean(L, false);
		break;
	}
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_SpellTargetUnit(lua_State* L)
{
	const char* unit = NULL;
	if ( lua_isstring(L, 1) )
		unit = lua_tostring(L, 1);
	g_pGameMain->SetBeginSpell(g_pGameMain->GetUnitSprite(unit));
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_SpellStopCasting(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->SpellStopCasting());
	return 1;
}

// ----------------------------------------------------------------------------
int	LuaFunc_DropItemOnUnit(lua_State* L)
{	
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_TargetUnit(lua_State* L)
{
	const char* unit = NULL;
	if ( lua_isstring(L, 1) )
		unit = lua_tostring(L, 1);
	g_pGameMain->SetPlayerTarget(g_pGameMain->GetUnitSprite(unit));
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_FocusUnit(lua_State* L)
{
	int index = -1;
	if ( lua_isnumber(L, 1) )
		index = (int)lua_tonumber(L, 1) - 1;

	const char* unit = NULL;	
	if ( lua_isstring(L, 2) )
		unit = lua_tostring(L, 2);

	if ( index == -1 )
	{
		for ( int i = 0; i < MAX_FOCUS_SPRITES; i++ )
		{
			if ( g_pGameMain->GetFocusSprite(i) == NULL )
			{
				index = i;
				break;
			}
		}
	}
	g_pGameMain->SetFocusSprite(index, g_pGameMain->GetUnitSprite(unit));
	return 0;	
}

// ----------------------------------------------------------------------------
int	LuaFunc_HasSelfRevive(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetSelfRevive());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_UseSelfRevive(lua_State* L)
{
	// 檢查身上是否可以自己復活旗標
	if ( g_pGameMain->GetSelfRevive() )
	{
		NetCli_Attack::S_RequestResurrection( EM_RequestResurrection_RaiseMagic );
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetResurrectTimeRemaining(lua_State* L)
{
	lua_pushnumber(L, (int)g_pGameMain->GetResurrectTimeRemaining());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_BrithRevive(lua_State* L)
{
	NetCli_Attack::S_RequestResurrection( EM_RequestResurrection_Normal );
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_AcceptResurrect(lua_State* L)
{
	// 檢查身上是否復活法術
	if ( g_pGameMain->GetResurrectState() )
	{
		NetCli_Attack::S_RequestResurrection( EM_RequestResurrection_RaiseMagic );
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_PlaySoundByPath(lua_State* L)
{
	if ( lua_isstring(L, 1) )
	{
		g_pRuGlobalEventManager->PlaySoundByFile(lua_tostring(L, 1), false, 0.0f, ruSOUNDTYPE_INTERFACE);
	}
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_SetFindPartyState(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetPlayer();
	if ( sprite )
	{
		PlayerStateStruct playerState = sprite->GetPlayerState();
		bool state = lua_toboolean(L, 1) != 0;
		if ( playerState.FindParty != state )
		{
			playerState.FindParty = state;
			sprite->SetPlayerState(playerState);

			g_pGameControl->SendLoginEvent("PLAYER_FINDPARTY_UPDATE");
			NetCli_RoleValue::S_SetPlayerAllZoneState(playerState);
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_GetFindPartyState(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetPlayer();
	if ( sprite )
	{
		PlayerStateStruct playerState = sprite->GetPlayerState();
		lua_pushboolean(L, playerState.FindParty);
		return 1;
	}
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_FollowUnit(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetUnitSprite(luaL_checkstring(L, 1));
	if ( sprite && sprite->GetRuEntity() )
		g_pGameMain->FollowSprite(sprite);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_AssistUnit(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetUnitSprite(luaL_checkstring(L, 1));
	CRoleSprite* spriteTarget = (sprite) ? sprite->GetTargetSprite() : NULL;
	if ( spriteTarget ) {
		g_pGameMain->SetPlayerTarget(spriteTarget);
	}
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_MoveForwardStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_FORWARD);
	g_pGameMain->MoveForwardState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_MoveForwardStop(lua_State* L)
{
	g_pGameMain->MoveForwardState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_MoveBackwardStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_BACK);
	g_pGameMain->MoveBackwardState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_MoveBackwardStop(lua_State* L)
{
	g_pGameMain->MoveBackwardState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_TurnRightStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_TURNRIGHT);
	g_pGameMain->TurnRightState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_TurnRightStop(lua_State* L)
{	
	g_pGameMain->TurnRightState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_TurnLeftStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_TURNLEFT);
	g_pGameMain->TurnLeftState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_TurnLeftStop(lua_State* L)
{
	g_pGameMain->TurnLeftState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_StrafeRightStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_RIGHT);
	g_pGameMain->StrafeRightState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_StrafeRightStop(lua_State* L)
{
	g_pGameMain->StrafeRightState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_StrafeLeftStart(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_LEFT);
	g_pGameMain->StrafeLeftState(true);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_StrafeLeftStop(lua_State* L)
{
	g_pGameMain->StrafeLeftState(false);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_Jump(lua_State* L)
{
	g_pTutorailFrame->doState(CTutorialFrame::STATE_JUMP);
	g_pGameMain->Jump();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_ToggleAutoRun(lua_State* L)
{
	g_pGameMain->ToggleAutoRune();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_ToggleRun(lua_State* L)
{
	g_pGameMain->ToggleRun();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_ToggleSheath(lua_State* L)
{
	g_pGameMain->ToggleSheath();
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_SitOrStand(lua_State* L)
{
	g_pGameMain->SitOrStand();
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_TargetNearestEnemy(lua_State* L)
{
	CRoleSprite* target = g_pGameMain->GetTargetNearest(true, lua_toboolean(L, 1) != 0);
	if ( target != NULL )
		g_pGameMain->SetPlayerTarget(target);
	return 0;
}

// ----------------------------------------------------------------------------
int	LuaFunc_TargetNearestFriend(lua_State* L)
{
	CRoleSprite* target = g_pGameMain->GetTargetNearest(false, lua_toboolean(L, 1) != 0);
	if ( target != NULL )
		g_pGameMain->SetPlayerTarget(target);
	return 0;
}

// ----------------------------------------------------------------------------
int LuaFunc_ResetTutorialTrigger		(lua_State* L)
{
	g_pGameMain->ClearTutorialState();
	return 0;
}

// ----------------------------------------------------------------------------
// 決鬥要求
int LuaFunc_StartDuelUnit (lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetUnitSprite(luaL_checkstring(L, 1));
	if ( sprite )
	{
		g_pGameControl->SetDuelStep(eDuelStep_Request, sprite->GetIdentify());
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 同意決鬥
int LuaFunc_AcceptDuel(lua_State* L)
{
	g_pGameControl->SetDuelStep(eDuelStep_Accept, 0);
	return 0;
}

// ----------------------------------------------------------------------------
// 取消決鬥
int LuaFunc_CancelDuel(lua_State* L)
{
	DuelInfo info = g_pGameControl->GetDuelInfo();
	switch (info.step)
	{
	case eDuelStep_Accept:
	case eDuelStep_Cancel:
	case eDuelStep_Ready:
		return 0;
	}

	g_pGameControl->SetDuelStep(eDuelStep_Cancel, 0);
	return 0;
}

// ----------------------------------------------------------------------------
// 角色決鬥狀態
int LuaFunc_PlayerDuelState	(lua_State* L)
{
	DuelInfo duel = g_pGameControl->GetDuelInfo();
	switch (duel.step)
	{
	case eDuelStep_None:
	case eDuelStep_Refuse:
	case eDuelStep_Cancel:
	case eDuelStep_Request:
	case eDuelStep_Receive:
		lua_pushnil(L);
		break;
	
	case eDuelStep_Accept:	
	case eDuelStep_Ready:
	case eDuelStep_Start:
	case eDuelStep_Finish:
		lua_pushstring(L, "fighting");
		break;
	}
	return 1;
}

int LuaFunc_TakeScreenshot(lua_State* L)
{
	g_pGameMain->TakeScreenshot();
	return 0;
}

int LuaFunc_GetSelectedRealmState(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetLastRealmState());
	return 1;
}

int LuaFunc_SetSelectedRealmState(lua_State* L)
{
	g_pGameMain->SetLastRealmState(lua_toboolean(L, 1) != 0);
	return 0;
}

int LuaFunc_GetLastAccountState(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetLastAccountNameState());
	return 1;
}

int LuaFunc_SetLastAccountState(lua_State* L)
{
	g_pGameMain->SetLastAccountNameState(lua_toboolean(L, 1) != 0);
	return 0;
}

// ----------------------------------------------------------------------------
// 傳回防沈迷時間

int LuaFunc_GetPlayTimeQuota(lua_State* L)
{
	int time = CNetGlobal::RoleData()->PlayerBaseData->PlayTimeQuota;
	int maxTime = 5 * 60 * 60;			// 5小時

	if ( time < maxTime ) {
		lua_pushnumber(L, time);
		lua_pushnumber(L, maxTime);		
		return 2;
	}
	return 0;
}

// ----------------------------------------------------------------------------
// 否已進入遊戲世界
int LuaFunc_IsEnterWorld(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->IsEnterWorld());
	return 1;
}

// ----------------------------------------------------------------------------
int LuaFunc_ReloadUI(lua_State* L)
{
	g_pInterface->ReloadUI();
	return 0;
}


int LuaFunc_SetCameraMoveTo(lua_State* L)
{
	CRuVector3 targetPosition;
	targetPosition.m_x = (lua_isnumber(L, 1)) ? (float)lua_tonumber(L, 1) : 0.0f;
	targetPosition.m_y = (lua_isnumber(L, 2)) ? (float)lua_tonumber(L, 2) : 0.0f;
	targetPosition.m_z = (lua_isnumber(L, 3)) ? (float)lua_tonumber(L, 3) : 0.0f;
	float time = (lua_isnumber(L, 4)) ? (float)lua_tonumber(L, 4) : 0.0f;
	g_pGameMain->SetCameraMoveTo(targetPosition, time);
	return 0;
}

int LuaFunc_SetCameraPosition(lua_State* L)
{
	CRuVector3 position;
	position.m_x = (lua_isnumber(L, 1)) ? (float)lua_tonumber(L, 1) : 0.0f;
	position.m_y = (lua_isnumber(L, 2)) ? (float)lua_tonumber(L, 2) : 0.0f;
	position.m_z = (lua_isnumber(L, 3)) ? (float)lua_tonumber(L, 3) : 0.0f;
	g_pGameMain->SetCameraPos(position);
	return 0;
}

int LuaFunc_SetTargetCamera(lua_State* L)
{
	CRuVector3 targetPosition;
	targetPosition.m_x = (lua_isnumber(L, 1)) ? (float)lua_tonumber(L, 1) : 0.0f;
	targetPosition.m_y = (lua_isnumber(L, 2)) ? (float)lua_tonumber(L, 2) : 0.0f;
	targetPosition.m_z = (lua_isnumber(L, 3)) ? (float)lua_tonumber(L, 3) : 0.0f;
	g_pGameMain->SetCameraTargetPos(targetPosition);
	return 0;
}

int LuaFunc_SetCameraUpVector(lua_State* L)
{
	CRuVector3 position;
	position.m_x = (lua_isnumber(L, 1)) ? (float)lua_tonumber(L, 1) : 0.0f;
	position.m_y = (lua_isnumber(L, 2)) ? (float)lua_tonumber(L, 2) : 0.0f;
	position.m_z = (lua_isnumber(L, 3)) ? (float)lua_tonumber(L, 3) : 0.0f;
	g_pGameMain->SetCameraUpVector(position);
	return 0;
}

int LuaFunc_GetCameraMoveTime(lua_State* L)
{
	lua_pushnumber(L, g_pGameMain->GetCameraMoveTime());
	return 1;
}

int LuaFunc_GetCameraPosition(lua_State* L)
{
	CRuCamera* camera = g_pGameMain->GetCamera();
	if ( camera && !g_pGameMain->IsEnterWorld() ) {
		const CRuVector3& position = camera->GetCameraPosition();
		lua_pushnumber(L, position.m_x);
		lua_pushnumber(L, position.m_y);
		lua_pushnumber(L, position.m_z);
		return 3;
	}
	return 0;
}

int	LuaFunc_GetTargetCamera(lua_State* L)
{
	CRuCamera* camera = g_pGameMain->GetCamera();
	if ( camera && !g_pGameMain->IsEnterWorld()) {
		const CRuVector3& position = camera->GetTargetPosition();
		lua_pushnumber(L, position.m_x);
		lua_pushnumber(L, position.m_y);
		lua_pushnumber(L, position.m_z);
		return 3;
	}
	return 0;
}

int LuaFunc_GetCameraUpVector(lua_State* L)
{
	CRuCamera* camera = g_pGameMain->GetCamera();
	if ( camera && !g_pGameMain->IsEnterWorld() ) {
		const CRuVector3& position = camera->GetCameraUpVector();
		lua_pushnumber(L, position.m_x);
		lua_pushnumber(L, position.m_y);
		lua_pushnumber(L, position.m_z);
		return 3;
	}
	return 0;
}

int LuaFunc_RotateCamera(lua_State* L)
{
	CRuCamera* camera = g_pGameMain->GetCamera();
	if ( camera && !g_pGameMain->IsEnterWorld() ) {
		float cycAngle = (lua_isnumber(L, 1)) ? (float)lua_tonumber(L, 1) : 0.0f;
		CRuVector3 position;
		position.m_x = (lua_isnumber(L, 2)) ? (float)lua_tonumber(L, 2) : 0.0f;
		position.m_y = (lua_isnumber(L, 3)) ? (float)lua_tonumber(L, 3) : 0.0f;
		position.m_z = (lua_isnumber(L, 4)) ? (float)lua_tonumber(L, 4) : 0.0f;
		camera->Cycle(cycAngle, CRuVector3(position.m_x, position.m_y, position.m_z));
	}
	return 0;
}

int LuaFunc_ResetUI(lua_State* L)
{
	if ( g_pInterface )
		g_pInterface->ResetUI();

	return 0;
}

int LuaFunc_PasswordConfirm(lua_State* L)
{
	g_pGameMain->SendSecondPassword(luaL_checkstring(L, 1));
	return 0;
}

int LuaFunc_CheckPasswordState(lua_State* L)
{
	if ( lua_isfunction(L, 1) ) {
		lua_setglobal(L, LUA_PASSWORD_FUNC);
	}
	lua_pushboolean(L, g_pGameMain->CheckSecondPassword());
	return 1;
}

int LuaFunc_SetHyperlinkCursor(lua_State* L)
{
	g_pGameMain->SetHyperlinkCursor(lua_toboolean(L, 1) != 0);
	return 0;
}

int LuaFunc_GetTipText(lua_State* L)
{
	lua_pushstring(L, g_ObjectData->GetTipText(luaL_checkint(L, 1)));
	return 1;
}

int LuaFunc_GetKeyboardFocus(lua_State* L)
{
	if ( g_pInterface )
	{
		if ( g_pInterface->GetUiWorld() )
		{
			CUiFrame* frame = g_pInterface->GetUiWorld()->GetKeyboardFocus();
			CUiLuaLayout::PushLayoutTable(L, frame);
			return 1;
		}
	}
	return 0;
}

int LuaFunc_IsMouseEnter(lua_State* L)
{
	lua_pushboolean(L, g_pGameControl->IsMouseEnter());
	return 1;
}

int LuaFunc_GetAccountBookEnterMoney(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;
	// 剩於天數可以使用
	if ( index >= 0 && index < Def_AccountMoneyLockTime_Max_Count_ )
	{
		RoleDataEx*	pRole = CNetGlobal::RoleData();
		if ( pRole )
		{
			lua_pushnumber(L, pRole->PlayerBaseData->LockAccountMoney[index]);
			return 1;
		}
	}
	/*
	// 永久鎖定
	else ( index == Def_AccountMoneyLockTime_Max_Count_ )
	{
		RoleDataEx*	pRole = CNetGlobal::RoleData();
		if ( pRole )
		{
			lua_pushnumber(L, pRole->PlayerBaseData->LockAccountMoney_Forever);
			return 1;
		}
	}
	*/
	return 0;
}

// 取得被鎖定不能使用幣值
int LuaFunc_GetAccountBookLockMoney(lua_State* L)
{
	int money = 0;
	RoleDataEx*	pRole = CNetGlobal::RoleData();
	if ( pRole )
	{
		money = pRole->PlayerBaseData->LockAccountMoney_Forever;
	}
	lua_pushnumber(L, money);
	return 1;
}

int LuaFunc_GetAccountBookItems(lua_State* L)
{
	lua_pushnumber(L, Def_AccountMoneyLockTime_Max_Count_);
	return 1;
}

int LuaFunc_GetAccountBookTotalMoney(lua_State* L)
{
	lua_pushnumber(L, CNetGlobal::RoleData()->PlayerTempData->LockAccountMoney);
	return 1;
}

int LuaFunc_IsBattleGroundZone(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->IsBattleGroundZone() );
	return 1;
}

int LuaFunc_GetBattleGroundNumPlayers(lua_State* L)
{
	int size = 0;
	if ( g_pGameMain->IsBattleGroundZone() )
	{
		size = g_pGameMain->GetBattleGroundNumPlayers();
	}
	lua_pushnumber(L, size);
	return 1;
}

int LuaFunc_IsPartyEnable(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->IsDisableParty() == false );
	return 1;
}

int LuaFunc_ResetControlArgument(lua_State* L)
{
	g_pGameMain->ResetControlArgument();
	return 0;
}

//------------------------------------------------------
int LuaFunc_GetReplaceSystemKeyword( lua_State* L )
{
	const char* szText = luaL_checkstring( L, 1 );
	std::string	strText	= CNetCli_Script_Child::ReplaceSystemKeyword( szText );
	lua_pushstring( L, strText.c_str() );
	return 1;
}

//------------------------------------------------------
// 卡點回報
int LuaFunc_SendWedgePointReport(lua_State* L)
{
	g_pGameControl->SendWedgeReport();
	return 0;
}


//------------------------------------------------------
// 取消卡點回報
int LuaFunc_CancelWedgePointReport(lua_State* L)
{
	g_pGameControl->CancelWedgeReport();
	return 0;
}

//------------------------------------------------------
int LuaFunc_Roll(lua_State* L)
{
	NetCli_Other::SL_PartyDice(0);
	return 0;
}

//------------------------------------------------------
int LuaFunc_GetNumZoneChannels(lua_State* L)
{
	int num = 0;
	set<int>& zoneChannel = g_pGameMain->GetZoneChannel();
	if ( !zoneChannel.empty() )
	{
		set<int>::iterator iter = zoneChannel.end();
		iter--;

		num = *iter / 1000 + 1;
	}
	lua_pushnumber(L, num);
	return 1;
}

//------------------------------------------------------
// 該分流是否存在
int LuaFunc_IsZoneChannelOnLine	(lua_State* L)
{
	int index = luaL_checkint(L, 1);

	set<int>& zoneChannel = g_pGameMain->GetZoneChannel();
	int zoneId = g_pGameMain->GetZoneID();
	for ( set<int>::iterator iter = zoneChannel.begin(); iter != zoneChannel.end(); iter++ )
	{
		// zone 1 分流 6 = 6001
		if ( (*iter % 1000) == (zoneId % 1000) && (*iter / 1000) + 1 == index )
		{
			lua_pushnumber(L, true);
			return 1;
		}
	}

	lua_pushboolean(L, true);
	return 0;
}

//-----------------------------------------------------------------------------
// 副本進度資訊
int LuaFunc_GetInstanceRecordInfo		(lua_State* L)
{	
	int index = luaL_checkint(L, 1) - 1;

	string instanceName; 
	int remainTime = 0;
	int extendTime = 0;
	int instanceNumber = -1;

	time_t gameTime = g_pGameMain->GetGameTime();	
	if ( index >= 0 && index < _MAX_INSTANCE_SETTING_COUNT_ )
	{
		char temp[512];

		// 取得副本編號
		sprintf_s(temp, 512, "InstanceKey%02d", index);
		int zoneId = g_ObjectData->GetSysKeyValue(temp);

		instanceName	= Func_GetLocalMapName( zoneId );
		if ( CNetGlobal::RoleData()->PlayerBaseData->InstanceSetting.KeyID[index] > 0 )
			instanceNumber	= CNetGlobal::RoleData()->PlayerBaseData->InstanceSetting.KeyID[index] + 10000;
		else
			instanceNumber	= -1;

		extendTime		= (int)((time_t)CNetGlobal::RoleData()->PlayerBaseData->InstanceSetting.ExtendTime[index] - gameTime);
		remainTime		= (int)((time_t)CNetGlobal::RoleData()->PlayerBaseData->InstanceSetting.LiveTime[index] - gameTime);
		
		if ( extendTime < 0 ) extendTime = 0;
		if ( remainTime < 0 ) remainTime = 0;		
	}

	lua_pushstring(L, instanceName.c_str());	// 副本名稱
	lua_pushnumber(L, instanceNumber);			// 副本編號
	lua_pushnumber(L, remainTime);				// 副本重置剩於時間	
	lua_pushnumber(L, extendTime);				// 副本延長時間
	return 4;
}

//-----------------------------------------------------------------------------
// 同意記錄副本進度
int LuaFunc_AcceptInstanceRecord		(lua_State* L)
{
	g_pGameMain->AcceptInstanceRecord();
	return 0;
}

//-----------------------------------------------------------------------------
// 不同意記錄副本進度
int LuaFunc_DeclineInstanceRecord		(lua_State* L)
{
	g_pGameMain->DeclineInstanceRecord();
	return 0;
}

//-----------------------------------------------------------------------------
int LuaFunc_SwitchExtendInstanceRecord	(lua_State* L)
{
	int index = luaL_checkint(L, 1) - 1;

	g_pPartyFrame->SwitchExtendInstanceTime(index);
	return 0;
}

//-----------------------------------------------------------------------------
// 邀請同騎坐騎
int LuaFunc_InviteRideMount(lua_State* L)
{
	CRoleSprite* sprite = g_pGameMain->GetUnitSprite(luaL_checkstring(L, 1));
	CRoleSprite* player = g_pGameMain->GetPlayer();

	if ( sprite && player )
	{
		// 是否有騎乘坐騎
		if ( player->GetRideState() )
		{
			// 是否有主控權
			if ( player->GetAttachParentId() == 0 )
			{
				CRoleSprite* proxyControlSprite = (player) ? player->GetProxyControlSprite() : NULL;
				int carryCount = player->GetMountCarryCount();						// 可以乘載數量
				int spriteCount = proxyControlSprite->GetLinkSpriteCount() - 1;		// 目前已經乘載數量

				if ( carryCount > 0 )
				{
					if ( carryCount > spriteCount )
					{
						if ( sprite->GetAttachParentId() == 0 && sprite->GetRideState() == false )
						{
							if ( !sprite->GetCanAttackState() )
							{
								if ( g_pGameMain->GetDistanceWithPlayer(sprite) < MAX_MULTI_RIDE_RANGE )
								{
									// 送出邀請搭乘
									g_pChatFrame->SendChatMessageEx(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_MOUNT_INVITE_TO"), sprite->GetName());
									NetCli_Channel::S_CtoC_AskRideMount(sprite->GetName(), player->GetName(), player->GetIdentify());

								}
								else
								{
									// 對方距離過遠
									g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_MOUNT_DISTANCE_ERROR"));
								}
							}
							else
							{
								// 對方距離過遠
								g_pChatFrame->SendChatMessage(CHAT_MSG_SYSTEM, "", g_ObjectData->GetString("SYS_MOUNT_TARGET_STATE_ERROR"));
							}
						}
						else
						{
							// 對方無法塔乘你的坐騎
							g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ALREADY"));
						}				
					}
					else
					{
						// 已經超過最大人數限制
						g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ERROR_2"));
					}
				}
				else
				{

					g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_CANT_CARRY"));
				}				
			}
			else
			{
				// 你沒有邀請別人搭乘權限
				g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_NOT_ACTIVE"));
			}
		}
		else
		{
			// 你沒有做坐騎或者已在坐騎上
			g_pGameMain->SendWarningMsg(g_ObjectData->GetString("SYS_MOUNT_ERROR_NONE"));
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
int LuaFunc_UninviteRideMount(lua_State* L)
{
	return 0;
}

//-----------------------------------------------------------------------------
// 邀請同騎坐騎
int LuaFunc_AcceptRideMount(lua_State* L)
{	
	g_pGameControl->AnswerInviteMount(true);	
	return 0;
}

//-----------------------------------------------------------------------------
// 邀請同騎坐騎
int LuaFunc_DeclineRideMount(lua_State* L)
{
	g_pGameControl->AnswerInviteMount(false);	
	return 0;
}

//-----------------------------------------------------------------------------
// 離開座騎
int LuaFunc_LeaveRideMount(lua_State* L)
{
	g_pGameControl->LeaveMount();	
	return 0;
}

//-----------------------------------------------------------------------------
// 是否可以多人坐騎
int LuaFunc_CheckMultiMount(lua_State* L)
{
	CRoleSprite* player = g_pGameMain->GetPlayer();
	bool isMulti = false;
	bool inEnable = false;

	// 是否有騎乘坐騎並且有主控權可以多載
	if ( player && false )
	{
		if ( player->GetRideState() && player->GetAttachParentId() == 0 && player->GetMountCarryCount() > 0 )
		{
			isMulti = true;
		}

		if ( lua_isstring(L, 1) )
		{
			CRoleSprite* sprite = g_pGameMain->GetUnitSprite(lua_tostring(L, 1));
			if ( sprite &&
				 sprite->GetAttachParentId() == 0 &&
				 sprite->GetRaidState() == false &&
				 g_pGameMain->GetDistanceWithPlayer(sprite) < MAX_MULTI_RIDE_RANGE )
			{
				inEnable = true;
			}
		}
	}

	lua_pushboolean(L, isMulti);
	lua_pushboolean(L, inEnable);
	return 2;
}

//-----------------------------------------------------------------------------
int LuaFunc_SetAggroPrompt(lua_State* L)			// 是否顯示仇恨表
{
	g_pGameMain->SetAggroPrompt(lua_toboolean(L, 1) != 0);
	return 0;
}

//-----------------------------------------------------------------------------
int LuaFunc_IsAggroPrompt(lua_State* L)				// 仇恨是否顯示
{
	lua_pushboolean(L, g_pGameMain->GetAggroPrompt());
	return 1;
}
//-----------------------------------------------------------------------------
int LuaFunc_SetAutoOpenGoodsPack(lua_State* L)
{
	g_pGameMain->SetAutoOpenGoodsPack(lua_toboolean(L, 1) != 0);
	return 0;
}
//-----------------------------------------------------------------------------
int LuaFunc_IsAutoOpenGoodsPack(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetAutoOpenGoodsPack());
	return 1;
}
//-----------------------------------------------------------------------------
// 是否顯示職業顏色
int LuaFunc_SetChatDisplayClassColor	(lua_State* L)
{
	g_pGameMain->SetChatDisplayClassColor(lua_toboolean(L, 1) != 0);
	return 0;
}

//-----------------------------------------------------------------------------
// 是否顯示職業顏色
int LuaFunc_IsChatDisplayClassColor		(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetChatDisplayClassColor());
	return 1;
}

//-----------------------------------------------------------------------------
int LuaFunc_IsInImplement(lua_State* L)
{
	CRoleSprite* player = g_pGameMain->GetPlayer();
	if ( player )
		lua_pushboolean(L, player->GetImplementSprite() != 0);
	else
		lua_pushboolean(L, false);	
	return 1;
}

//-----------------------------------------------------------------------------
int LuaFunc_OpenGMNotification(lua_State* L)
{
	const UpdateInfInfo& info = g_pGameMain->GetUpdateInfo();
	if ( strlen(info.gmNotification) > 0 ) {
		string url = info.gmNotification;

		size_t begin;
		while ( (begin = url.find("[$ACOUNT]")) != string::npos )
			url.replace(begin, 9, g_pAccountLogin->GetAccountName());
		while ( (begin = url.find("[$NAME]")) != string::npos )
			url.replace(begin, 7, g_pGameMain->GetPlayerName());

		ShellExecute(NULL, "open", url.c_str(), 0, 0, SW_SHOW);
		lua_pushboolean(L, true);
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
int LuaFunc_DisableBugReport(lua_State* L)
{
	const UpdateInfInfo& info = g_pGameMain->GetUpdateInfo();
	lua_pushboolean(L, info.disableBugReport != 0);
	return 1;
}
int LuaFunc_MoveTo(lua_State* L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	int z = luaL_checkint(L, 3);
	CRuVector3 point;
	point.x = x;
	point.y = y;
	point.z = z;
	g_pGameMain->SetMoveToPoint(point);
	return 0;
}
//-----------------------------------------------------------------------------
int LuaFunc_GetSoulPoint(lua_State* L)
{
	CRoleSprite* pPlayer = g_pGameMain->GetPlayer();
	RoleDataEx* pRoleData = CNetGlobal::RoleData();

	int soulPoint = -1;
	int pointStatus = 0;
	if ( pRoleData && pPlayer ) {
		if ( pPlayer->GetVocation() == EM_Vocation_Harpsyn ) {
			soulPoint = pRoleData->TempData.SoulPoint;
			for ( int i = 0; i < pPlayer->GetBuffSize(); i++ ) {
				RoleBuffInfo* buff = pPlayer->GetBuffInfo(i);
				if ( buff ) {
					if ( buff->id == 501571 ) {
						pointStatus = 1;
						break;
					}
					if ( buff->id == 501572 ) {
						pointStatus = 2;
						break;
					}
				}
			}
		}
	}

	lua_pushnumber(L, soulPoint);
	lua_pushnumber(L, pointStatus);
	return 2;
}

//-----------------------------------------------------------------------------
int LuaFunc_SetHideOtherPlayerEffect(lua_State* L)
{
	g_pGameMain->SetHideOtherPlayerEffect(lua_toboolean(L, 1) != 0);
	return 1;
}

//-----------------------------------------------------------------------------
int LuaFunc_IsHideOtherPlayerEffect(lua_State* L)
{
	lua_pushboolean(L, g_pGameMain->GetHideOtherPlayerEffect());
	return 1;
}