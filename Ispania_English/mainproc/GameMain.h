#ifndef _GAMEMAIN_H
#define _GAMEMAIN_H

#include <vector>
#include <set>
#include <string>

#include "RuneDev.h"
#include "GameMainDef.h"
#include "ACTActionEventsManager.h"
#include "ReadUpdateIni.h"
#include "..\tools\CpuUsage\CpuUsage.h"
#ifdef KALYDO
// Add the Kalydo ROM application class so we can use the download defines.
#include "../../Kalydo/SocialIntegration/TRunesOfMagicApplication.h"
#endif // KALYDO
#define REPLACE_PLAYER_NUM					50
#define REPLACE_MAX_PLAYER_NUM				250
#define REPLACE_MEMORY_SIZE					1.35 * 1024
#define REDUCE_TEXTURE_DETAIL_SIZE			0.9 * 1024

//可以座到椅子的距離
#define DF_SIT_TO_CHAIR_MAX_DISTANCE        20

// 語系字串
#define LANGUAGE_CODE_RW					"RW"
#define LANGUAGE_CODE_TW					"TW"
#define LANGUAGE_CODE_CN					"CN"
#define LANGUAGE_CODE_DE					"DE"
#define LANGUAGE_CODE_JP					"JP"
#define LANGUAGE_CODE_KR					"KR"
#define LANGUAGE_CODE_ENUS					"ENUS"
#define LANGUAGE_CODE_ENEU					"ENEU"
#define LANGUAGE_CODE_SG					"SG"
#define LANGUAGE_CODE_BR					"BR"
#define LANGUAGE_CODE_TR					"TR"
#define LANGUAGE_CODE_RU					"RU"
#define LANGUAGE_CODE_FR					"FR"
#define LANGUAGE_CODE_PT					"PT"
#define LANGUAGE_CODE_PL					"PL"
#define LANGUAGE_CODE_VN					"VN"
#define LANGUAGE_CODE_ES					"ES"				// 西班牙
#define LANGUAGE_CODE_ID					"ID"				// 印尼
#define LANGUAGE_CODE_NL					"NL"				// 荷蘭
#define LANGUAGE_CODE_RO					"RO"				// 羅馬尼亞
#define LANGUAGE_CODE_PH					"PH"				// 菲律賓
#define LANGUAGE_CODE_PL					"PL"				// 波蘭
#define LANGUAGE_CODE_TH					"TH"				// 泰國
#define LANGUAGE_CODE_SAES					"SAES"				// 阿南美洲西班牙文
#define LANGUAGE_CODE_SAPT					"SAPT"				// 阿南美洲萄葡牙文
#define LANGUAGE_CODE_AE					"AE"				// 阿拉伯聯合大公國
#define LANGUAGE_CODE_ENAR					"ENAR"				// 阿拉伯聯合大公國(英文)
#define LANGUAGE_CODE_AUS					"AUS"				// 澳州
#define LANGUAGE_CODE_IT					"IT"				// 義大利
#define MAX_LANGUAGES						29

#define MAX_FOCUS_SPRITES					12
#define TOMB_MAP_ICON_ID					0x7f000000

#define LUA_PASSWORD_FUNC					"__uiLuaPassword_Callback__"
#define LUA_CHECKPLAYER_FUNC				"__uiLuaCheckPlayer_Callback__"

#define IS_DETECT_MEMORY_LEAKS_ENABLE 1 // 是否偵測記憶體洩漏.

// **********************************************************************************************
// Globals
class	CGameMain;

extern	CGameMain							*g_pGameMain;
extern	char								*g_languageCode[MAX_LANGUAGES];

// **********************************************************************************************

using namespace std;

/*
typedef struct 
{
	union {
		int value;
		struct {
			unsigned char background;
			unsigned char foreground;
			unsigned char transient;
			unsigned char auxiliary;
		};
	}actor;
	union {
		int value;
		struct {
			bool	isUnholster				:1;		// true 取出武器
			bool	isWalk					:1;		// true 走路
			bool	isCombat				:1;		// true 戰鬥中
			bool	isDead					:1;		// true 死亡中
			bool	isSit					:1;		// true 坐下
		};
	}state;
}PLAYER_ACTOR_STATE;
*/

struct PetCommandStruct
{
	union {
		int _value;
		struct {
			bool		movable : 1;				// 移動(跟隨玩家)
			bool		strikeBack : 1;				// 返擊
		};
	};
};

// 寵物基本資料
struct PetInfo
{
	int						GUID;					// 
	int						ItemID;					//
	int						MagicID;				//
	PetCommandStruct		Status;					// 寵物狀態
};

typedef void (*pfSpellRangeCallback) (int spellID, float x, float y, float z);

enum CASTSPELLERROR
{
	CASTSPELL_ERROR_EXIST				= -1,			// 不存在
	CASTSPELL_ERROR_ACTIVE				= -2,			// 被動技能不能施法
	CASTSPELL_ERROR_CLASS				= -3,			// 職業誤錯,非本職業
	CASTSPELL_ERROR_HP					= -4,			// 生命值不足
	CASTSPELL_ERROR_MP					= -5,			// 魔力值不足
	CASTSPELL_ERROR_SP_WARRIOR			= -6,			// warrior sp 不足
	CASTSPELL_ERROR_SP_RANGER			= -7,			// Ranger sp 不足
	CASTSPELL_ERROR_SP_ROGUE			= -8,			// Rogue sp 不足
	CASTSPELL_ERROR_GUN					= -9,			// 需求槍
	CASTSPELL_ERROR_AMMO				= -10,			// 彈藥不足
	CASTSPELL_ERROR_BOW					= -11,			// 需求弓
	CASTSPELL_ERROR_ARROW				= -12,			// 箭矢不足
	CASTSPELL_ERROR_ITEM				= -13,			// 物品需求不足
	CASTSPELL_ERROR_COLDOWN				= -14,			// 冷?j時間
	CASTSPELL_ERROR_TARGET				= -15,			// 需要一個目標
	CASTSPELL_ERROR_DITANCE				= -16,			// 距離過遠
	CASTSPELL_ERROR_MOVE				= -17,			// 不能移動施法
	CASTSPELL_ERROR_DISABLEJOBSKILL		= -18,			// 無法使用職業技能
	CASTSPELL_ERROR_LEVEL				= -19,			// 等級不足
	CASTSPELL_ERROR_CANTCTRL			= -20,			// 無法控制 (心控中

	//需求
	CASTSPELL_ERROR_NEED_WEAPON			= -100,			//需要某武器
	CASTSPELL_ERROR_NEED_EQ				= -101,			//需要某裝備
	CASTSPELL_ERROR_NEED_SUIT			= -102,			//需要某套裝
	CASTSPELL_ERROR_NEED_BUFF			= -103,			//需要某Buff	
	CASTSPELL_ERROR_NEED_NOBUFF			= -104,			//不能有某Buff
	CASTSPELL_ERROR_NEED_UNARMED		= -105,			//需要空手
	CASTSPELL_ERROR_NEED_BLADE			= -106,			//需要裝劍
	CASTSPELL_ERROR_NEED_DAGGER			= -107,			//需要裝匕首
	CASTSPELL_ERROR_NEED_WAND			= -108,			//需要裝手杖
	CASTSPELL_ERROR_NEED_AXE			= -109,			//需要裝斧頭
	CASTSPELL_ERROR_NEED_BLUDGEON		= -110,			//需要裝單手鎚
	CASTSPELL_ERROR_NEED_CLAYMORE		= -111,			//需要裝雙手劍
	CASTSPELL_ERROR_NEED_STAFF			= -112,			//需要裝雙手杖
	CASTSPELL_ERROR_NEED_2H_AXE			= -113,			//需要裝雙手斧
	CASTSPELL_ERROR_NEED_2H_HAMMER		= -114,			//需要裝雙手鎚
	CASTSPELL_ERROR_NEED_POLEARM		= -115,			//需要裝長矛
	CASTSPELL_ERROR_NEED_SWORD_TYPE		= -116,			//需要裝劍類武器
	CASTSPELL_ERROR_NEED_AXE_TYPE		= -117,			//需要裝斧類武器
	CASTSPELL_ERROR_NEED_SHIELD			= -118,			//需要裝盾
	CASTSPELL_ERROR_NEED_HP_LOWER		= -119,			//需要HP低一些
	CASTSPELL_ERROR_NEED_HP_HIGHER		= -120,			//需要HP高一些
	CASTSPELL_ERROR_NEED_JOB			= -121,			//職業有問題
	CASTSPELL_ERROR_NEED_LONGWEAPON		= -122,			//需要裝長距離武器
	CASTSPELL_ERROR_NEED_HAMMER_TYPE	= -123,			//需要裝鎚類武器
	CASTSPELL_ERROR_NEED_2H_WEAPON_TYPE	= -124,			//需要裝雙手武器
	CASTSPELL_ERROR_NEED_BUFF_GROUP		= -125,			//需要某一種類的BUFF

	CASTSPELL_ERROR_NEED_CRITICAL		= -126,			//致命一擊後才可使用
	CASTSPELL_ERROR_NEED_BECRITICAL		= -127,			//被人致命一擊後才可使用
	CASTSPELL_ERROR_NEED_DODGE			= -128,			//閃避後才可使用
	CASTSPELL_ERROR_NEED_MISS			= -129,			//MISS後才可使用
	CASTSPELL_ERROR_NEED_PARRY			= -130,			//隔檔後才可使用
	CASTSPELL_ERROR_NEED_BEPARRY		= -131,			//被人隔檔後才可使用
	CASTSPELL_ERROR_NEED_BEDODGE		= -132,			//被人閃避後才可使用

	CASTSPELL_ERROR_NEED_COMBAT			= -133,			//需要戰鬥中
	CASTSPELL_ERROR_NEED_NOCOMBAT		= -134,			//非戰鬥中
	CASTSPELL_ERROR_NEED_DITANCE		= -135,			//距離過近無法施法

	CASTSPELL_ERROR_TARGET_NEED_BUFF		= -136,		//對象目標需要某BUFF
	CASTSPELL_ERROR_TARGET_NEED_NOBUFF		= -137,		//對象目標需要某BUFF
	CASTSPELL_ERROR_TARGET_NEED_NOCOMBAT	= -138,		//對象目標需要戰鬥中
	CASTSPELL_ERROR_TARGET_NEED_COMBAT		= -139,		//對象目標非戰鬥中	
};

enum ItemCastingType
{
	ITEM_CASTING_NONE					= 0,
	ITEM_CASTING_USEITEM				= 1,			// 使用背包內物品
	ITEM_CASTING_DISSOLUTION			= 2,			// 使用分解技能[特殊]
	ITEM_CASTING_PARTNER		        = 3,            //呼叫快樂的夥伴

	// 以下類型可能無做用
	ITEM_CASTING_REFINE					= 4,
};

enum eCastSpellState
{
	CASTSPELL_NONE						= 0,
	CASTSPELL_WAITING_LOCATION			= 1,
	CASTSPELL_WAITING_TARGET			= 2,
	CASTSPELL_CASTING					= 3,
	CASTSPELL_WAITING_LOCATION_EXTRA	= 4,
	CASTSPELL_WAITING_TARGET_EXTRA		= 5,
};

enum ItemCommandEnum
{
	ITEM_COMMAND_NONE					= 0,
	ITEM_COMMAND_USE_BAG				= 1,
	ITEM_COMMAND_BAG_TO_BANK			= 2,
};

enum PacketReduceState
{
	PACKET_REDUCE_NONE					= 0,			// 沒有做用(未做封包衰減)
	PACKET_REDUCE_PLAYER_MOVE			= 1,			// 移除玩家移動封包(還是有定位封包)
	PACKET_REDUCE_PLAYER_MOVE_ALL		= 2,			// 移除玩家所有移動封包(也沒有定位封包,看到其他玩家應該會是用跳的)
	PACKET_REDUCE_MOVE_ALL				= 3,			// 移除所有移動封包(NPC與玩家)
};

enum TargetLineEffectType
{	
	TargetLineEffect_None				= 0,
	TargetLineEffect_Line				= 1,			// 線段
	TargetLineEffect_Parabola			= 2,			// 拋物線
};

struct ItemCommand
{
	ItemCommandEnum type;
	float			startTime;
	union {
		int data[8];

		// ITEM_COMMAND_USE_BAG
		struct {
			int index;
			bool fromBag;
			int objiectID;
			int	createTime;
			int	serial;
		};

		struct {
			int src;
			int dest;
		};
	};
};

typedef union {
	int value;
	struct {
		unsigned char background;
		unsigned char foreground;
		unsigned char transient;
		unsigned char auxiliary;
	};
}PLAYER_ACTOR_STATE;

// 保留CLIENT端設定資訊
struct ClientSaveSetting
{
	ClientSaveSetting()
	{
		memset(this, 0, sizeof(ClientSaveSetting));
	}

	void Init()
	{
		isUseServerData				= false;
		isNPCTitleVisible			= true;
		isPlayerTitleVisible		= true;
		isAllSCTVisible				= false;
		isMouseMoveEnable			= true;
		isDisableTitleHide			= false;
		isSelfCast					= true;
		isSelfTitleVisible			= true;
		isLastSelectedRealm			= false;
		isLastAccountName			= false;
		isShowGemePromrt			= true;
		isNPCRealHealth				= true;
		isTitleVisible				= true;
		isGuildVisible				= true;
		isTitleIconVisible			= true;
		isCameraYReverse			= false;
		isLButtonCancelTarget		= true;
		isRBuutonCancelTarget		= false;
		isLButtonCameraRotateEnable	= true;
		isCameraFollowEnable		= false;
		isDisableDisplayNPCTalk		= true;
		isCameraSelectTarget		= false;
		isPlayerLootMotion			= true;
		isAutoTakeLoot				= false;
		isChatDisplayClassColor		= false;
		isAggroPrompt				= false;
		isAutoOpenGoodsPack			= true;
		isHideOtherPlayerEffect		= false;
	}

	union 
	{
		int _value[4];
		struct {
			bool			isUseServerData					:1;			// 是否開啟伺服器儲存資料
			bool			isNPCTitleVisible				:1;			// 顯示NPC物件頭像名稱
			bool			isPlayerTitleVisible			:1;			// 顯示玩家角色頭像名稱
			bool			isAllSCTVisible					:1;			// 顯示所有攻擊數值
			bool			isMouseMoveEnable				:1;			// 是否開啟滑鼠移動
			bool            isDisableTitleHide				:1;			// 在稱號裝備介面中 不可裝備的稱號要不要顯示
			bool			isSelfCast						:1;			// 啟動自我施法(當法術無需要目標時)			
			bool			isSelfTitleVisible				:1;			// 顯示自己頭像名稱
			bool			isLastSelectedRealm				:1;			// 使用最後選擇的伺服器登入
			bool			isLastAccountName				:1;			// 記錄最後登入帳號
			bool			isShowGemePromrt				:1;			// 顯示遊戲提示
			bool			isNPCRealHealth					:1;			// 顯示非玩家友善類型的血量
			bool			isTitleVisible					:1;			// 顯示稱號
			bool			isGuildVisible					:1;			// 顯示公會
			bool			isTitleIconVisible				:1;			// 顯示角色職業與善惡值ICON
			bool			isCameraYReverse				:1;			// 鏡頭Y軸反轉
			bool			isLButtonCancelTarget			:1;			// 滑鼠左鍵取消目標
			bool			isRBuutonCancelTarget			:1;			// 滑鼠右鍵取消目標
			bool			isLButtonCameraRotateEnable		:1;			// 滑鼠左鍵鏡頭旋轉
			bool			isCameraFollowEnable			:1;			// 尾隨鏡頭
			bool			isDisableDisplayNPCTalk			:1;			// 不顯示NPC交談訊息
			bool			isCameraSelectTarget			:1;			// 使用鏡頭坐標點選取角色
			bool			isPlayerLootMotion				:1;			// 撥放拾取動作
			bool			isAutoTakeLoot					:1;			// 自動拾取
			bool			isChatDisplayClassColor			:1;			// 交談職業顯示顏色
			bool			isAggroPrompt					:1;			// 仇恨警告提示
			bool			isAutoOpenGoodsPack				:1;			// 自動開啟商城背包
			bool			isRequestServerSaveData			:1;			// 詢問是否由伺服器記錄資料
			bool			isHideOtherPlayerEffect			:1;			// 隱藏其他玩家特效
		};
	};
};

struct SystemFunctionState
{
	SystemFunctionState()
	{
		guildBattleGround = 1;
		ch3Login = 1;
		vivoxUserAgreement = 0;
		loginEpisode = 0;
	};

	union {
		int _value;
		struct {
			bool				guildBattleGround			: 1;
			bool				ch3Login					: 1;
			bool				vivoxUserAgreement			: 1;
			char				loginEpisode				: 5;		// 第幾章(0, 1都代表第一章)
		};
	};
};

struct EilteBellStruct 
{
	float m_destroyTime;			// 消失時間
	int m_GItemID;					// 區域編號
}; 

struct AttachObjectInfo
{
	int sourceGItemID;				// 來源 Server GItem
	int targetGItemID;				// 目標 Server GItem
	int AttachType;					// 連接模式
	string sourceLinePoint;			// 來源參考點
	string targetLinePoint;			// 目標參考點
};

//-----------------------------------------------------------------------------
// 觸發事件
typedef void (*ZoneDescriptorCallback)	(CRuWorld_ZoneDescriptor* zoneDescriptor);

class CGameMain : public CRuneDev
{
public:
	CGameMain();
	virtual ~CGameMain();

	virtual void				LoadData();
	virtual void				Init();
	virtual void				Release();
	virtual void				Update();
	virtual void				SendCreateRoleSprite( CRoleSprite* pRoleSprite );
	virtual void				SendDeleteRoleSprite( CRoleSprite* pRoleSprite );
	virtual void				SendPlayerGItemIDChange( CRoleSprite* pRoleSprite, int newGItemID );
	virtual LRESULT				WndProc( HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam );
#ifdef KALYDO
	// in browser version always use the language set by either the website, or default = eneu
	virtual	void				LoadINI( const char* fileName, const char* language);
#else
	virtual	void				LoadINI( const char* fileName );
#endif // KALYDO
	virtual void				WriteINI( const char* fileName );
	virtual	void				LoadLuaFile(const char* fileName);
	virtual	void				SpriteMsgProc(IBaseSprite* sprite, SpriteMessageType msg, INT64 lParam, INT64 rParam);
	virtual	void				UnitMsgProc(const char* unit, SpriteMessageType msg, CRoleSprite* sprite, int lParam, int rParam);
	virtual	void				LoadLanguage();

	// Font
	virtual string				GetDefaultFontPath();

	virtual void				SetGItemID(int GItemID);
	virtual int					GetGItemID()							{ return m_GItemID; }
	virtual	void				SetIP(const char* ip)					{ m_ip = ip; }
	virtual	const char*			GetIP()									{ return m_ip.c_str(); }

	virtual	void				SetMac(const char* mac)					{ m_macaddress = mac; }
	virtual	const char*			GetMac()								{ return m_macaddress.c_str(); }

	virtual	void				SetPort(int port)						{ m_port = port; }
	virtual	int					GetPort()								{ return m_port; }
	virtual void				SetVersion(const char* version)			{ SecureZeroMemory(m_version, sizeof(m_version)); sprintf_s(m_version, sizeof(m_version), "%s", version);}
	virtual	const char*			GetVersion()							{ return m_version; }
	virtual void				SetVersionNO(const char* version)			{ SecureZeroMemory(m_versionNO, sizeof(m_versionNO)); sprintf_s(m_versionNO, sizeof(m_versionNO), "%s", version);}
	virtual	const char*			GetVersionNO()							{ return m_versionNO; }
	virtual void				SetVersionURL(const char* versionURL)	{ m_versionURL = versionURL;}
	virtual const char*			GetVersionURL()							{ return m_versionURL.c_str();}
	virtual void				SetNoticeURL(const char* noticeURL)		{ m_noticeURL = noticeURL; }
	virtual const char*			GetNoticeURL()							{ return m_noticeURL.c_str(); }
	virtual void				SetActivateURL(const char* activateURL)	{ m_activateURL = activateURL; }
	virtual const char*			GetActivateURL()						{ return m_activateURL.c_str(); }
	virtual void				SetActivateURLArg(const char* activateURLArg)	{ m_activateURLArg = activateURLArg; }
	virtual const char*			GetActivateURLArg()						{ return m_activateURLArg.c_str(); }
	virtual const UpdateInfInfo&	GetUpdateInfo()						{ return m_updateInf; }
	virtual int					GetCpuUsage()										{ return (int)m_CpuUsage.GetUsage(); }
	//virtual MEMORYSTATUSEX	GetMemoryUsage()								{ return m_ProcessMemory;}
	
	virtual void				SetDownloadPort(int port)				{ m_downloadPort = port;}
	virtual int					GetDownloadPort()						{ return m_downloadPort;}
	virtual	void				SetGameTime(int time, int timeZone);
	virtual	int					GetDBID()								{ return m_DBID; }
	virtual void				SetZoneDBID(int DBID)					{ m_zoneDBID = DBID; }
	virtual int					GetZoneDBID()							{ return m_zoneDBID; }
	virtual	int					GetPlayerWorldGUID()					{ return m_playerID; }
	virtual	void				SetPlayerPos(CRuVector3& pos);
	virtual	void				SetZoneID(int zoneID);
	virtual	int					GetZoneID()								{ return m_zoneID; }
	virtual void				SetRoomID(int roomID);
	virtual int					GetRoomID(int roomID)					{ return m_roomID; }
	virtual	void				SetMapName(const char* name);
	virtual	const char*			GetMapName()							{ return m_mapName.c_str(); }
	virtual bool				IsGlobalLoadComplete();
	virtual bool				InsertGlobalObject(int GItemID);
	virtual	void				SetMouseInPlayer(CRoleSprite* sprite);
	virtual	CRoleSprite*		GetMouseInPlayer()						{ return m_pMouseInPlayer; }
	virtual	void				SetPlayer(CRoleSprite* sprite, CRuVector3& pos, CRuVector3& dir);
	virtual	CRoleSprite*		GetPlayer()								{ return m_pPlayer; }	
	virtual	void				SetPlayerID(int playerID);
	virtual	void				SetPlayerTarget(CRoleSprite* sprite);
	virtual	CRoleSprite*		GetPlayerTarget();	
	virtual CRoleSprite*		GetPlayerPet();
	virtual void				SetFocusSprite(int index, CRoleSprite* sprite);
	virtual CRoleSprite*		GetFocusSprite(int index);
	virtual void				ClearFocusSprite(CRoleSprite* sprite);
	virtual	CRoleSprite*		GetTargetNearest(bool enemy=true, bool reverse=false);
	virtual	bool				IsEnterWorld()							{ return m_isEnterWorld; }
	virtual	const char*			GetPlayerName()							{ return m_playerName.c_str(); }
	virtual	void				EnterWorld(int DBID, const char* playerName);
	virtual	void				LeaveWorld();	
	virtual CRoleSprite*		GetUnitSprite(const char* str, CRoleSprite* src=NULL);

	virtual const char*			GetLocalIP()							{ return m_localIP.c_str(); }
	
	virtual	void				SendWarningMsg(const char* msg);
 	virtual	void				SendWarningMsgEx(const char* msg, ...);
	virtual	void				SendSystemMsg(const char* msg);
	virtual void				SendSystemMsgEx(const char* msg, ...);
	virtual void				SendSystemChatMsg(const char* msg);
	virtual void				SendSystemChatMsgEx(const char* msg, ...);

	virtual	void				SendScrollBannerMessage(const char* msg);			//跑馬燈
	virtual void				SendScrollBannerMessageEx(const char* msg, ...);
	virtual void				SendMessageDialog(const char* msg);				    //訊息對話盒 一段訊息 一個關閉按鈕
	virtual void				SendMessageDialogEx(const char* msg, ...);

	virtual void				SendPlayTimeQuataNotify(const char* msg);		//防沉迷通知用pop dialog, 一分鐘自動關閉, 無任何按鈕

	virtual const char*			GetZoneName()							{ return m_zoneName.c_str(); }
	virtual const char*			GetZoneName2()							{ return m_zoneName2.c_str(); }
	virtual int					GetWorldMapID()							{ return m_WorldMapID; }

	virtual	float				GetDistanceWithPlayer(CRoleSprite* sprite);
	virtual	float				GetDistanceWithPlayer(int GItemID)		{ return GetDistanceWithPlayer(FindSpriteByID(GItemID)); }
	virtual	void				SetCombatState(bool combatState);
	virtual	bool				GetCombatState()						{ return m_isCombatState; }	
	virtual	void				AttackTarget(CRoleSprite* target);
	virtual	void				CancelAttack();
	virtual	void				SetPlayerAction(int action);		
	virtual	void				Jump();
	virtual	void				DoEmotion(CRuFusion_ActorState emotion);
	virtual	void				TranslateBaseValue(CRoleSprite* sprite, int mp, int sp, int sub_sp, int* outValue);
	virtual	void				ReceivePing( int iTime );
	virtual	int					GetPing()								{ return m_ping; }

	virtual	void				WorldToViewport(CRuVector3 &point);
	virtual	void				WorldToScreen(CRuVector3 &point);
	virtual	void				SetCursor(CursorType type, const char* dragItem=NULL);

	virtual	void				PlayerDead(bool selfRevive, int resurrectTimeRemaining);
	virtual	void				ResurrectRequest(int source, int magicBaseID);
	virtual	void				PlayerResurrect();
	virtual	bool				GetSelfRevive()							{ return m_selfRevive; }
	virtual	float				GetResurrectTimeRemaining()				{ return m_resurrectTime; }		
	virtual bool				GetResurrectState()						{ return m_resurrect; }

	// 取得遊戲時間(0 ~ 239) 240 = 遊戲1天
	virtual	int					GetCurrentGameTime()					{ return m_currentGameTime; }
	virtual time_t				GetGameTime()							{ return m_gameTime + (time_t)(m_frameTime - m_startFrameTime); }
	virtual time_t				GetServerTimeZone()						{ return m_serverTimeZone; }
	virtual	void				SetPlayerCameraPos(CRuVector3& pos);	

	// 施法檢查
	virtual	int					CanUseSkill(int skillID, int* outNeedType=NULL, int* outNeedValue=NULL);
	virtual	bool				CastSpellByID(int ID, CRoleSprite* target=NULL);
	virtual bool				CastSpellByName(const char* name, int level, CRoleSprite* target=NULL);
	virtual	bool				SetBeginSpell(CRoleSprite* target);
	virtual	void				SetCastSpellState(eCastSpellState state, bool forceReset=false);	
	virtual	eCastSpellState		GetCastSpellState()						{ return m_castSpellState; }
	virtual	bool				SpellStopCasting();
	virtual void				ClearCastSpellCount()					{ m_sendCastSpellCount = 0; }
	virtual void				ClearCastSpellTime()					{ m_lastCastSpellTime = 0.0; }
	virtual void				ClearCancelSpellState()					{ m_isCancelCastSpell = false; }
	virtual	void				CancelCastSpell();
	virtual void				ClearSendSpell()						{ m_sendSpellTarget = NULL; m_sendSpellMagicID = 0; }

	// Control Argument
	virtual	void				SetPlayerTitleVisible(bool show);
	virtual	void				SetNPCTitleVisible(bool show);
	virtual	void				SetSelfTitleVisible(bool show);
	virtual	void				SetTitleVisible(bool show);
	virtual	void				SetGuildVisible(bool show);
	virtual	void				SetTitleIconVisible(bool show);
	virtual	void				SetMouseMoveEnable(bool enable)			{ m_clientSetting.isMouseMoveEnable = enable; }
	virtual	void				SetDisableTitleHide(bool hide)			{ m_clientSetting.isDisableTitleHide = hide; }
	virtual	void				SetDisplayAllSCT(bool show)				{ m_clientSetting.isAllSCTVisible = show; }
	virtual	void				SetSelfCastState(bool selfCast)			{ m_clientSetting.isSelfCast = selfCast; }
	virtual	void				SetLastRealmState(bool selected)		{ m_clientSetting.isLastSelectedRealm = selected; }
	virtual	void				SetLastAccountNameState(bool selected)	{ m_clientSetting.isLastAccountName = selected; }
  	virtual	void				SetShowGemePromrt(bool selected)		{ m_clientSetting.isShowGemePromrt = selected; }
	virtual	void				SetNPCRealHealthState(bool selected)	{ m_clientSetting.isNPCRealHealth = selected; }
	virtual void				SetCameraYReverse(bool reverse)			{ m_clientSetting.isCameraYReverse = reverse; }
	virtual void				SetLButtonCancelTarget(bool state)		{ m_clientSetting.isLButtonCancelTarget = state; }
	virtual void				SetRButtonCancelTarget(bool state)		{ m_clientSetting.isRBuutonCancelTarget = state; }
	virtual void				SetLButtonCameraRotateEnable(bool state){ m_clientSetting.isLButtonCameraRotateEnable = state; }	
	virtual void				SetCameraFollowEnable(bool state)		{ m_clientSetting.isCameraFollowEnable = state; }
	virtual void				SetDisableDisplayNPCTalk(bool state)	{ m_clientSetting.isDisableDisplayNPCTalk = state; }
	virtual void				SetCameraSelectTarget(bool state)		{ m_clientSetting.isCameraSelectTarget = state; }
	virtual void				SetLootPlayerMotion(bool state)			{ m_clientSetting.isPlayerLootMotion = state; }	
	virtual void				SetChatDisplayClassColor(bool enbale)	{ m_clientSetting.isChatDisplayClassColor = enbale; }
	virtual void				SetAutoTakeLoot(bool enbale)			{ m_clientSetting.isAutoTakeLoot = enbale; }	
	virtual void				SetAggroPrompt(bool enbale)				{ m_clientSetting.isAggroPrompt = enbale; }		
	virtual void				SetAutoOpenGoodsPack(bool enbale)		{ m_clientSetting.isAutoOpenGoodsPack = enbale; }	
	virtual void				SetHideOtherPlayerEffect(bool enable)	{ m_clientSetting.isHideOtherPlayerEffect = enable; }
	virtual	bool				GetPlayerTitleVisible()					{ return m_clientSetting.isPlayerTitleVisible; }
	virtual	bool				GetNPCTitleVisible()					{ return m_clientSetting.isNPCTitleVisible; }
	virtual	bool				GetSelfTitleVisible()					{ return m_clientSetting.isSelfTitleVisible; }
	virtual	bool				GetMouseMoveEnable()					{ return m_clientSetting.isMouseMoveEnable; }
	virtual	bool				GetDisableTitleHide()					{ return m_clientSetting.isDisableTitleHide; }
	virtual	bool				GetDisplayAllSCT()						{ return m_clientSetting.isAllSCTVisible ; }
	virtual	bool				GetSelfCastState()						{ return m_clientSetting.isSelfCast; }
	virtual	bool				GetLastRealmState()						{ return m_clientSetting.isLastSelectedRealm; }
	virtual	bool				GetLastAccountNameState()				{ return m_clientSetting.isLastAccountName; }
 	virtual	bool				GetShowGemePromrt()						{ return m_clientSetting.isShowGemePromrt; }
	virtual	bool				GetNPCRealHealthState()					{ return m_clientSetting.isNPCRealHealth; }
	virtual	bool				GetTitleVisible()						{ return m_clientSetting.isTitleVisible; }
	virtual	bool				GetGuildVisible()						{ return m_clientSetting.isGuildVisible; }
	virtual bool				GetTitleIconVisible()					{ return m_clientSetting.isTitleIconVisible; }
	virtual bool				GetCameraYReverse()						{ return m_clientSetting.isCameraYReverse; }
	virtual bool				GetLButtonCancelTarget()				{ return m_clientSetting.isLButtonCancelTarget; }
	virtual bool				GetRButtonCancelTarget()				{ return m_clientSetting.isRBuutonCancelTarget; }
	virtual bool				GetLButtonCameraRotateEnable()			{ return m_clientSetting.isLButtonCameraRotateEnable; }
	virtual bool				GetCameraFollowEnable()					{ return m_clientSetting.isCameraFollowEnable; }
	virtual bool				GetDisableDisplayNPCTalk()				{ return m_clientSetting.isDisableDisplayNPCTalk; }
	virtual bool				GetCameraSelectTarget()					{ return m_clientSetting.isCameraSelectTarget; }
	virtual bool				GetLootPlayerMotion()					{ return m_clientSetting.isPlayerLootMotion; }	
	virtual bool				GetChatDisplayClassColor()				{ return m_clientSetting.isChatDisplayClassColor; }
	virtual bool				GetAutoTakeLoot()						{ return m_clientSetting.isAutoTakeLoot; }
	virtual bool				GetAggroPrompt()						{ return m_clientSetting.isAggroPrompt; }
	virtual bool				GetAutoOpenGoodsPack()					{ return m_clientSetting.isAutoOpenGoodsPack; }
	virtual bool				GetHideOtherPlayerEffect()				{ return m_clientSetting.isHideOtherPlayerEffect; }
	virtual bool				CheckDisplayEffect(int id, int targetId=0);
	virtual bool				CheckDisplayEffect(CRoleSprite* ownerSprite, CRoleSprite* targetSprite=NULL);
	virtual void				ResetControlArgument();	
	
	// virtual client setting
	virtual void				SetServerSaveClientData(bool state)		{ m_clientSetting.isUseServerData = state; }
	virtual bool				GetServerSaveClientData()				{ return m_clientSetting.isUseServerData; }

	virtual bool				IsChatLog()								{ return m_IsChatLog;}

	// Item Casting
	virtual bool				ItemCastingStart(ItemCastingType type, int srcPos, int destType, int destPos);
	virtual void				ItemCastingInterrupt();
	virtual void				ItemCastingStop();
	virtual	void				ItemCastingReset();
	virtual	bool				ItemCastingEnable()						{ return m_itemCasting.type != ITEM_CASTING_NONE; }
	virtual	bool				UseBagItem(int index, int x=0.0f, int y=0.0f, int z=0.0f);

	// Follow
	virtual	void				FollowSprite(CRoleSprite* sprite);
	virtual CRoleSprite*		GetFollowSprite()						{ return m_follow.target; }

	// Move
	virtual	void				MoveForwardState(bool start);
	virtual	void				MoveBackwardState(bool start);
	virtual	void				TurnRightState(bool start);
	virtual	void				TurnLeftState(bool start);
	virtual	void				StrafeRightState(bool start);
	virtual	void				StrafeLeftState(bool start);
	virtual	void				ToggleAutoRune(int forceState=0);
	virtual	void				ToggleRun();
	virtual	void				ToggleSheath();
	virtual	void				SitOrStand();
	
	virtual	int					OnClick(CRoleSprite* target);	

	//MoveToScreenPoint
   	virtual	void				MoveToScreenPoint_AddSprite( int SpriteID );
   	virtual	void				MoveToScreenPoint_DelSprite( int SpriteID );
	virtual	void				MoveToScreenPoint_ClearSprite( );

	virtual	void				MoveToScreenPoint(int x,  int y);
	virtual	void				SetMoveToPoint(CRuVector3& point, CRoleSprite* sprite=NULL, bool forceMove=true);
	virtual	void				ClearMoveToPoint();
	virtual	bool				IsMoveingToPoint()						{ return m_mouseMove.enable; }

	virtual	void				RotateCamera(float xDelta, float yDelta, bool updatePlayerDir=false);
	virtual void				SetCameraForward(CRuVector3 forward, bool immediately=true);

	// Tutorial Flag
	virtual	void				SetTutorialState(int index, bool state);
	virtual	bool				GetTutorialState(int index);
	virtual	void				ClearTutorialState();	

	// 
	virtual void				ClearCastSpellToItem()					{ m_castSpellToItem = 0; }
	virtual	int					GetCastSpellToItemID()					{ return m_castSpellToItem; }

	virtual	void				TakeScreenshot();	

	virtual void				ReloadClientLua()						{ m_isReloadClientLua = true; }
	virtual	void				LoadClientLua();

	virtual	void				SaveEnvironmentVariables();
	virtual	void				LoadEnvironmentVariables();

	// Login Screen
	virtual void				LoadLoginScreen();

	// Mount
	virtual void				Dismount();	

	// WDB / FDB Journaling
	CRuDataStore_Disk*			m_wdbJournalDS;

	void						FDBJournal_Begin(const char *journalName)
	{
		g_ruResourceManager->BeginJournal(journalName);
	}

	void						FDBJournal_End()
	{
		g_ruResourceManager->EndJournal();
	}

	void						WDBJournal_Begin(const char *journalName)
	{
		// Create journaling WDB
		m_wdbJournalDS = ruNEW CRuDataStore_Disk();
		m_wdbJournalDS->Create(journalName);
		m_wdbJournalDS->DisablePagePadding(TRUE);

		// Begin data store journal
		CRuDataStore_Disk *srcDS = static_cast<CRuDataStore_Disk *>(static_cast<CRuOptimalTerrain_DataSource_DS *>(GetRuTerrain()->GetDataSource())->GetDataStore());
		m_wdbJournalDS->BeginJournal(srcDS);

		// Copy required tables in their entirety
		srcDS->CopyTo(m_wdbJournalDS, "header");
		srcDS->CopyTo(m_wdbJournalDS, "backdrop_header");
		srcDS->CopyTo(m_wdbJournalDS, "texture_list");
		srcDS->CopyTo(m_wdbJournalDS, "water_descriptor");
		srcDS->CopyTo(m_wdbJournalDS, "worldDoodadContainer_DoodadTemplate");
		srcDS->CopyTo(m_wdbJournalDS, "world_zone_descriptors");
	}

	void						WDBJournal_End()
	{
		m_wdbJournalDS->EndJournal();
		m_wdbJournalDS->Flush();
		ruSAFE_RELEASE(m_wdbJournalDS);
	}

	// Pattern
	virtual void				PushPatternString(const BYTE* pattern);	
	virtual void				RemovePatternString(const BYTE* pattern , int type);

	virtual bool				IsPatternStringVisible(const BYTE* pattern);

	virtual void				MatchPatternString(BYTE* text);	
	virtual bool				CheckPatternString( const char* text);
	virtual void				ReturnCharacterSelect();

	virtual bool				IsShowServerDebugMsg()					{ return m_displaySrvDebugMsg; }

	virtual bool				CheckRightfulName(const char* name);
	virtual float				GetTerrainLoadStatus();
	
	virtual void				ResetSpriteNameVisible(CRoleSprite* sprite);
	virtual void				SetSpriteGuildID(CRoleSprite* sprite, int guildID);
	virtual void				GuildEnemyUpdate();

	// 更換公會名稱
	virtual void				ChangeGuildName(int guildID);	

	virtual void				SetCampState(bool state);
	virtual bool				GetCampState()							{ return m_campState; }
	virtual void				ChangeParallelID(int id);
	virtual int					GetChangeParallelID()					{ return m_parallelID; }
	virtual void				ClearChangeParalleID()					{ m_parallelID = -1; }

	virtual float				GetTotalFPS()							{ return m_totalFps; }
	virtual float				GetTotalPing()							{ return m_totalPing; }
	virtual int					GetTotalCount()							{ return m_totalCount; }
	virtual void				ClearTotalCount();	
	virtual bool				GetOSDisplayString(char* pszOS , size_t cchLen);

	// 取得玩家PVP狀態
	virtual bool				GetSpritePVPState(CRoleSprite* sprite);

	// 區域PK
	virtual void				UpdatePVPZoneState(int state = -1);
	// 設定玩家PVP旗標(BUFF 產生)	
	virtual void				UpdateSpritePKBuff();
	// 陣營改變
	virtual void				SetSpriteCampId(CRoleSprite* sprite, int campId, bool force=false);

	// 重設玩家背後旗子顯像物件
	virtual void				ResetObjectGuildFlag(int guildID, const char* insignia, int insigniaMainColor, int insigniaOffColor, const char* banner, int bannerMainColor, int bannerOffColor);

	// 取得二次密碼	
	virtual bool				CheckSecondPassword();
	
	virtual const char*			GetSecondPassword()						{ return m_secondPassword.c_str(); }
	virtual void				SetSecondPassword(const char* password);
	virtual void				SendSecondPassword(const char* password);
	virtual void				ClearSecondPassword();

	// 設定關係表
	virtual void				SetCampTableObj(int id);

	virtual const char*			GetLanguage()							{ return m_language.c_str(); }
	virtual const char*			GetCountry()							{ return m_country.c_str(); }
	virtual const char*			GetInstall()							{ return m_install.c_str(); }
	virtual const char*			GetImageLocation(const char* section=NULL);

	virtual bool				PathMoveTo(CRuVector3 target);
	virtual void				ClearPathMove();
	virtual bool				IsPlayerMoveLocked();

	virtual void				SetHyperlinkCursor(bool enable)			{ m_cursorHyperlink = enable; }
	virtual void				SetPVEState(bool isPVE);
	virtual bool				GetPVEState()							{ return m_isPVE; }

	// 比對相同名稱玩家
	void						ComparePlayerName();	

	// 設定時間
	void						SetTimeGroupType(int groupType)			{ m_timeGroupType = groupType; }
	
	// 墓碑物件
	void						SetTombInfo(int zoneId, CRuVector3 position, float leftTime, int debtExp, int debtTp, int exp);
	void						ClearTomb();
	void						UpdateTomb();

	// 寵物
	void						CreatePet(int petType, int petGUID, int petItemID, int petMagicID);
	void						DeletePet(int petType, int petGUID);
	void						UpdatePetSkill();
	void						SetPetMovable(bool enable);
	bool						IsPetMovable()							{ return m_petStatus.movable; }
	void						SetPetStrikeBackEnable(bool enable);
	bool						IsPetStrikeBackEnable()					{ return m_petStatus.strikeBack; }
	void						UpdatePetSprite(CRoleSprite* player);

	const char*					GetDataLanguage()						{ return m_DataLanguage.c_str(); }
	const char*					GetStringLanguage()						{ return m_StringLanguage.c_str(); }	

	bool						IsAttackAction()						{ return m_attackWorldGUID != 0; }
	bool						IsCycleSpellID(int id)					{ return m_cycleMagicId == id; }
	void						SetCycleSpell(int id, int index);
	void						ClearCycleSpell();

	void						AddPartitionPosition(CRoleSprite* sprite, CRuVector3 position);
	CRuVector3*					FindPartitionPosition(CRoleSprite* sprite);
	void						ClearPartitionPosition()				{ m_partitionPosition.clear(); }	

	int							GetPetGUID();

	void						SwitchUpdateElapsedTime(bool show)		{ m_showUpdateElapsedTime = show; }

	// 精英警告
	void						AddEliteBell(int GItemID);
	void						UpdateEliteBell();


	// 物品後續執行內容
	int							PushItemCommand(ItemCommand command);
	void						ExcuteItemCommand(int id);
	void						RemoveItemCommand(int id);

	// 
	const char*					GetGameServerIP()						{ return m_gameServerIP.c_str(); }
	int							GetGameServerPort()						{ return m_gameServerPort; }

#ifdef KALYDO
	void						SetGameServerIP(const char* ip)			{ m_gameServerIP = ip; }
	void						SetGameServerPort(int port)				{ m_gameServerPort = port; }
#endif // KALYDO

	// 特殊事件
	string						GetSpecialEventString(int id);	

	// 區域資訊
	void						SetDisableTrade(bool disable)			{ m_isDisableTrdae = disable; }	
	void						SetDisableDuel(bool disable)			{ m_isDisableDuel = disable; }
	void						SetDisableParty(bool disable)			{ m_isDisableParty = disable; }
	void						SetEnableBGParty(bool enable)			{ m_isEnableBGParty = enable; }
	void						SetBattleGroundZone(bool inBattle)		{ m_isBattleGround = inBattle; }
	void						SetNoSwimming(bool noSwimming)			{ m_isNoSwimming = noSwimming; }
	bool						IsDisableTrade()						{ return m_isDisableTrdae; }
	bool						IsDisableDuel()							{ return m_isDisableDuel; }	
	bool						IsDisableParty()						{ return m_isDisableParty; }	
	bool						IsEnableBGParty()						{ return m_isEnableBGParty; }	
	bool						IsBattleGroundZone()					{ return m_isBattleGround; }
	bool						IsNoSwimming()							{ return m_isNoSwimming; }
	
	int                         GetBattleGroundType();
	bool						IsEnableBattleGroup();
	void						SetWorldBattleGround(bool state)		{ m_isWorldBattleGround = state; }
	bool						IsWorldBattleGround()					{ return m_isWorldBattleGround; }

	// 戰場
	void						ClearAllBattleMember();
	bool						AddBattleMember(int GItemID, CRoleSprite* sprite);
	bool						DeleteBettleMember(int GItemID);
	CRoleSprite*				FindBettleMemberByGItemID(int GItemID);
	CRoleSprite*				FindBettleMemberByDBID(int DBID);
	CRoleSprite*				FindBettleMemberByWorldID(int WorldID, const char* playerName);
	CRoleSprite*				FindBattleMemberByName(const char* playerName);
	int							GetBattleGroundNumPlayers()				{ return m_numBattleGroundPlayers; }

	// 帳密
	void						SetAccountName(const char* account)				{ m_account = account; }
	void						SetAccountPassword(const char* password)		{ m_password = password; }
	const char*					GetAccountName()								{ return m_account.c_str(); }
	const char*					GetAccountPassword()							{ return m_password.c_str(); }

	// 關連物件
	void						AddAttachObj(int source, int target, int mode, const char* sourceLink, const char* targetLink);
	void						DeleteAttachObj(int GItemID);
	void						ClearAttachObj();
	void						UpdateAttachObj();

	// 記錄玩家個數
	int							GetPlayerSpriteCount();
	void						UpdateDummySprite();

	bool						InStressZone();
	void						SetServerInfo(const char* country, int IPNum);
	const char*					GetServerCountry()						{ return m_serverCountry.c_str(); }
	int							GetServerIPNum()						{ return m_IPNum; }

	void						AddPlayerRefCount()						{ m_playerRefCount++; }
	void						DelPlayerRefCount()						{ m_playerRefCount--; }
	int							PlayerRefCount()						{ return m_playerRefCount; }
	bool						IsDiaplayMaintainServer()				{ return m_displayMaintainServer; }	

	bool						IsUIUpdate();	

	bool						SaveRoleData(const wchar_t* name);
	bool						LoadRoleData(const wchar_t* name);

	// 是否在水中
	bool						IsInWater();

	// 設定分流個數
	void						SetZoneChannel(int ZoneID, bool boOpen);
	set<int>&					GetZoneChannel()						{ return m_zoneOpen; }
	
	void						SetBotLocked(bool state);
	bool						GetBotLocked()							{ return m_botLocked; }

	// 清除副本訊問進度
	void						RequestInstanceRecord();
	void						ClearInstanceWaitTime();
	void						AcceptInstanceRecord();
	void						DeclineInstanceRecord();

	void						CheckMemoryStatus(bool forceUpdate=false);
	DWORD						GetMemoryLoad()							{ return m_memoryLoad; }
	DWORD						GetCurrentMemory()						{ return m_currentMemory; }

	// 連結物件
	void						AttachPoint(CRuVector3 position, CRuVector3 dir, CRuFusion_ActorState actorState);

	//坐到Entity 上 如果他有 p_sit_chair01 參考點
	bool						SitToEntity( CRuEntity* pEntity );

	//Entity 是不是椅子 如果他有 p_sit_chair01 參考點
	//poutDistance 玩家到參考點的距離
 	bool						EntityIsChair( CRuEntity* pEntity, float* poutDistance = NULL );

	// Client Data
	void						SaveClientSetting(bool Immediately=false);
	void						SaveClientData(int keyID, int size, char* data);
	void						SendLoadClientData();
	void						ReceiveLoadClientData(int keyID, int size, char* data);

	// VoiceSettings
	void						SetVoiceDisable( bool isDisable ) { m_bIsVoiceSettingsDisable = isDisable; }
	bool						GetVoiceDisable() { return m_bIsVoiceSettingsDisable; }

	bool						CheckCountry( const char* country );

	// Room Event Flag
	void						PushRoomValue(int keyID, int value);
	map<int,int>::iterator		RemoveRoomValue(int keyID);
	map<int,int>::iterator		RemoveRoomValue(map<int, int>::iterator iter);
	void						ClearRoomValue();

	void						SetFreeCameraEnalbe(bool enable)					{ m_isFreeCamera = enable; }
	bool						GetFreeCameraEnalbe(bool enable)					{ return m_isFreeCamera; }
	void						SetCameraFollowSprite(CRoleSprite* sprite);

	void						CreateTargetEffect(TargetLineEffectType targetType, CRoleSprite* referenceSprite);
	void						ReleaseTargetEffect();
	bool						RotateTargetEffect(float xDelta, float yDelta);	
	bool						UpdateTargetEffectPosition();
	void						SetTargetEffectDirection(CRuVector3 dir);
	void						SetTargetEffectType(TargetLineEffectType targetType, int height);
	void						SetTargetEffectParabolaHeight(float height)			{ m_targetParabolaHeight = height; }
	float						GetTargetEffectParabolaHeight()						{ return m_targetParabolaHeight; }
	void						SetTargetEffectMaxRange(float range)				{ m_targetMaxRange = range; }
	float						GetTargetEffectMaxRange()							{ return m_targetMaxRange; }
	void						SetTargetEffectMaxAngle(float angle)				{ m_targetMaxAngle = angle; }
	float						GetTargetEffectMaxAngle()							{ return m_targetMaxAngle; }
	TargetLineEffectType		GetTargetEffectType()								{ return m_targetLineType; }
	

	void						RegisterZoneChangeCallBack(ZoneDescriptorCallback event);

	BOOL						QueryCollision(CRuVector3& beginPoint, CRuVector3& endPoint, CRuVector3& outCollisionPoint, bool boTerrain = true, bool spriteCollision = false, CRuEntity* ignoreEntity=NULL);
	float						GetMagicFlyTime(CRuVector3& p1, CRuVector3& p2, int magicID, int parabolaHeight=0);

	bool						InsertAnchorHeightSprite(int id, CRuVector3& position);
	bool						IsAnchorHeightSpriteLoadComplete();

	void						SetPaperdollTextureDetail(int level);
	int							GetPaperdollTextureDetail();
	void						SetTextureDetail(int level);
	int							GetTextureDetail();


	void						SetGuildBattleGroundState(bool enable)				{ m_systemEnable.guildBattleGround = enable; }
	bool						IsGuildBattleGroundEnable()							{ return m_systemEnable.guildBattleGround; }

	void						SetCh3LoginState(bool enable)						{ m_systemEnable.ch3Login = enable; }
	bool						IsCh3LoginEnable()									{ return m_systemEnable.ch3Login; }

	void						SetLoginEpisodeVersion(int episodeVersion)			{ m_systemEnable.loginEpisode = episodeVersion; }
	int							GetLoginEpisodeVersion()							{ return m_systemEnable.loginEpisode; }

	void						SetVivoxUserAgreementState(bool enable)				{ m_systemEnable.vivoxUserAgreement = enable; }
	bool						IsVivoxUserAgreementEnable()						{ return m_systemEnable.vivoxUserAgreement; }

	void						MovePartition(bool enable);

	bool						getMerchant()										{return m_bMerchantFrame;}
	void						setMerchant(bool enable=true)						{m_bMerchantFrame=enable;}

	void						SetUpdateInfInfo(const UpdateInfInfo& info)			{ m_updateInf = info; }

	bool						TestHeight(CRuVector3 position, float& outHeight);

	//密碼狀態相關
	bool						GetIsMD5Password()									{return m_IsMD5Password;}
	bool						GetIsAutoHashPassword()								{return m_IsAutoHashPassword;}
	int							GetPasswordHashType()								{return m_PasswordHashType;}

	void						SkipCastSpellCheck(bool isSkip)						{ m_skipCastSpellCheck = isSkip; }

	//其他
	int							GetItemMallSessionID();
	int							GetZShopSessionID();
	void						SetCameraControlMode(bool enabled)					{ m_cameraControl = enabled; }
	bool						GetCameraControlMode()								{ return m_cameraControl; }

	int							GetPlayerCount();

	//ZShop
	bool						GetEnableZShop()									{ return m_enableZShop; }
	void						SetEnableZShop(bool value)							{ m_enableZShop = value; }
	const char*					GetZShopURL()										{ return m_ZShopURL.c_str(); }
	void						SetZShopURL(const char* value)						{ m_ZShopURL = value; }
	

	int							GetWBWidth()										{ return m_iWBWidth; }
	int							GetWBHeight()										{ return m_iWBHeight; }

	void						SetWBWidth( int iw )								{ m_iWBWidth = iw; }
	void						SetWBHeight( int ih )								{ m_iWBHeight = ih; }

	//Game Force platform
	bool						IsGameForgeCountries();

protected:	
	virtual	void				UpdateWorld();
	virtual	void				CheckTouchSprite();
	virtual void				FixedPosition();
	virtual	void				SendPlayerPos(bool forceSend=false, bool setPos=false);
	virtual	void				SendPlayerActor();
	virtual	void				UpdateCursor();
	virtual	void				ControlCamera();
	virtual	void				UpdateSneakSprite();
	virtual	void				MovePlayerPos(CRuVector3& pos);

	virtual	void				UpdateMouseMove();
	virtual void				UpdatePathMove();
	virtual void				SendPacketReduceMode(PacketReduceState state);

	virtual void				CheckPlayerPositionHeight();

protected:
	// net information
	string						m_ip;
	string						m_macaddress;
	int							m_port;
	string						m_gameServerIP;
	int							m_gameServerPort;
	int							m_currentGameTime;				// 目前遊戲時間
	int							m_gameTime;						// 遊戲時間
	int							m_serverTimeZone;				// server 時區時間	

	double						m_startFrameTime;				// 開始計時時間

	string						m_StringLanguage;				
	string						m_DataLanguage;		

	string						m_playerName;					// 玩家名稱
	string						m_zoneName;						// 區域名稱
	string						m_zoneName2;					// 區域名稱2
	int							m_WorldMapID;					// 地圖id(UI用)
	int							m_playerID;						// 玩家角色編號(世界編號)
	int							m_GItemID;						// 儲存角色編號(區域編號)
	int							m_DBID;							// SERVER角色唯一代碼
	int							m_zoneID;						// 區域名稱
	int							m_roomID;						// 房間號碼
	string						m_mapName;						// 地圖名稱	
	bool						m_receiveWaitLoading;			// 等待載入中
	int							m_changeTerrainStep;			// 更換地圖
	bool						m_isEnterWorld;					// 是否進入遊戲世界
	bool						m_isFirstViewpoint;				// 第一人稱視角	
	bool						m_isCombatState;				// 進入攻擊狀熊,此狀態為接收SERVER送出訊息
	bool						m_isPVE;						// PVE 狀態
	int							m_playerRefCount;				// 玩家計數器
	CRuVector3					m_changeTerrainPos;	
	
	CRoleSprite*				m_pMouseInPlayer;	
	float						m_followTime;
	int							m_attackWorldGUID;
	CRuArrayList<int>			m_activeColTriggersThisCycle;
	CRuArrayList<int>			m_activeColTriggersLastCycle;

	CRuVector3					m_moveVector;
	CRuVector3					m_direction;
	CRuVector3					m_position;
	CRuFusion_ActorState		m_sendActorState;
	int							m_parentMountId;
	float						m_sendMoveTime;	
	bool						m_firstJump;
	bool						m_pingState;
	bool						m_falling;
	int							m_startPing;
	int							m_ping;	
	
	PLAYER_ACTOR_STATE			m_actorState;
	RoleSpriteStatus			m_playerState;
	CursorType					m_cursorType;
	bool						m_cursorHyperlink;				// 是否在Hyperlink元件上
	string						m_dragItemStr;
	
	bool						m_selfRevive;
	bool						m_resurrect;
	float						m_resurrectTime;
	
	eCastSpellState				m_castSpellState;				// 施法狀態
	int							m_castSpellID;					// 施法編號
	int							m_castSpellIndex;				// 施法索引編號(一般為-1,否則則代表在等待點選目標)
	int							m_castSpellToItem;				// 對物品使用	
	int							m_sendCastSpellCount;			// 已經送出的施法個數
	bool						m_isCancelCastSpell;			// 取消施法
	double						m_lastCastSpellTime;			// 最後送出的施法時間

	CRoleSprite*				m_sendSpellTarget;				// 送出的施法對象
	int							m_sendSpellMagicID;				// 送出的施法編號

	bool						m_IsChatLog;
	char						m_version[256];                 // 現在Client端的版本
	char						m_versionNO[256];               // 現在Client端data的版本 
	string                      m_versionURL;                   // 更新伺服器上Version.Txt的URL
	string						m_noticeURL;					// 公告資訊
	string						m_activateURL;					// K2 激活帳號
	string						m_activateURLArg;				// 參數
	int                         m_downloadPort;                 // 更新伺服器http port

	// 地表
	bool						m_needLoadTerrain;				// 需要重新載入地圖
	float						m_loadTerrainDelayTime;			// 一般載入地圖時間過長,先讓LOADING畫面出現在做載入動作
	float						m_sendLoadOkDelayTime;			// 送回SERVER開始加入物件延遲時間

	// 顯示SERVER來的訊息
	bool						m_displaySrvDebugMsg;

	bool						m_IsMD5Password;				//登入時, 使用者輸入的密碼是否為MD5 Hash過的密碼
	bool						m_IsAutoHashPassword;			//自動Hash使用者密碼旗標
	int							m_PasswordHashType;				//Hash密碼方式 0=md5, 1=bcrypt 自動Hash使用者密碼旗標開啟時有效

	// 移動旗標
	union {
		int Bytes;
		struct {
			bool			forward			:1;			// 向前移動
			bool			backward		:1;			// 向後移動
			bool			strafeRight		:1;			// 向右移動
			bool			strafeLeft		:1;			// 向左移動
			bool			turnRight		:1;			// 原地向右轉向
			bool			turnLeft		:1;			// 原地向左轉向
			bool			autoRun			:1;			// 自動向前跑
			bool			walk			:1;			// 走路
		};
	}m_moveState;

	// 恐懼
	struct {
		int					dir;						// 往前方向
		float				time;						// 更新時間
	}m_fear;

	// 滑鼠移動
	struct {
		CRuVector3			target;
		CRoleSprite*		sprite;
		bool				enable;
		float				update;
		float				distance;
	}m_mouseMove;

	// 路徑移動	

	// 跟隨
	struct {
		CRoleSprite*		target;						// 跟隨對象
		float				touchRange;					// 作用距離
		float				update;						// 重新取得座標延遲時間
		float				distance;					// 上次更新與目標點距離值
	}m_follow;	

	// 記錄物品對物品使用中(需要跑時間軸,有等待時間)
	struct {
		ItemCastingType		type;						// 來源物品使用類型
		int					bagPos;						// 背包位置
		int					destType;					// 目的類型 0背包 1裝備 2銀行
		int					destPos;					// 目的類型所在位置
		int					actor;						// 動作代碼
		bool				hasEffect;					// 是否有特效產生
	}m_itemCasting;

	vector<CRuAlgorithm_PatternMatch_FJS*> m_patternMatch;

	CACTActionEventsManager	m_ACTActionEventsManager;	
	float					m_worldCameraFarPlane;
	int						m_PVPZoneState;				// 區域PK旗標

	bool					m_campState;				// 登出狀態(true 登出中)
	float					m_campUpdateTime;			// 觸發時間
	int						m_parallelID;				// 分流狀態	

	float					m_totalFps;					// 總張數
	float					m_totalPing;				// 總合Ping
	double					m_totalLastTime;			// 最後更新時間
	int						m_totalCount;				// 上傳資料計數器

	CRuVector3					m_rightPosition;		// 最後確認座標點
	CRuArrayList<CRuVector3>	m_obstructPosition;		// 遇到阻檔的座標點

	string					m_secondPassword;			// 二次密碼
	bool					m_sendPassword;

	string					m_install;					// 安裝國家代碼
	string					m_country;					// 國家代碼
	string					m_language;					// 語言代碼
	string					m_saveFolder;				// 儲存目錄名稱
	int						m_campTableId;				// 關係表

	vector<CRuVector3>		m_pathPositions;			// 路徑資料
	CRuVector3				m_pathTargetPosition;		// 路徑移動目標點	
	float					m_pathMoveTime;				// 路徑移動到?揖媦虳珨摁伅?	
	float					m_pathMoveUpdate;			// 路徑移動更新時間

	int						m_resetFontIndexStep;		// 更新字型
 
	set<int>				m_MoveToScreenPointSpriteIDs; //MoveToScreenPoint時 也需要檢測有沒有點到的SpriteID

	CRoleSprite*			m_focusSprites[MAX_FOCUS_SPRITES];	

	CRuVector3				m_cameraCurrentVector;		// 攝影機目前方向
	CRuVector3				m_cameraFinalVector;		// 最後攝影機朝向方向

	int						m_timeGroupType;			// 設定時間類型(1~4)	

	CRuVector3				m_tombPosition;				// 座標位置
	float					m_tombLeftTime;				// 剩於時間 ( > 0 ) 存在
	int						m_tombZoneId;				// 區域編號
	int						m_tombDebtExp;				// 負債經驗值
	int						m_tombDebtTp;				// 負債技能點數
	int						m_tombExp;					// 剩於經驗值
	
	// 寵物資訊
	int						m_petGUID;					// 
	int						m_petItemID;				//
	int						m_petMagicID;				//
	PetCommandStruct		m_petStatus;				// 寵物狀態
	map<int, PetInfo>		m_petInfo;

	bool					m_isWorldBattleGround;		// 世界戰場
	bool					m_isBattleGround;			// 是否在戰場中
	bool					m_isDisableTrdae;			// 不能交易
	bool					m_isDisableDuel;			// 不能比試
	bool					m_isDisableParty;			// 不能邀請組隊
	bool					m_isEnableBGParty;			// 開啟戰場中組隊
	bool					m_inWater;					// 是否在水中
	bool					m_isNoSwimming;				// 區域不能游泳
	int						m_numBattleGroundPlayers;	// 戰場最大人數設定

	int						m_cycleMagicId;				// 連續施展法術編號
	int						m_cycleMagicPos;			// 連續施展法術書位置	

	CRuRendererPlugin_SubmergeFX*	m_submergeFX;		// Submerge FX plugin

	map<CRoleSprite*, CRuVector3>	m_partitionPosition;

	bool					m_isWaitGlobalObject;		// 等待全域物件載入
	vector<int>				m_globalObject;				// 全域物件

	bool					m_showUpdateElapsedTime;	// 顯示每次Frame更新時間

	map<int, float>			m_eliteBell;				// 精英提示警告

	int						m_playTimeQuota;			// 防沉迷訊息
	float					m_QuotaUpdateTime;

	map<int, ItemCommand>	m_itemCommand;
	int						m_lastItemCommandID;		// 最後一個執行編號

	string					m_localIP;					// 由 Server 傳回的本地 Client IP
	bool					m_isReloadClientLua;			// 重新載入

	map<int, string>		m_specialEventMap;			// 特殊字串索引

	map<int, CRoleSprite*>	m_BattleMember;				// 戰場特殊玩家資料

	string					m_account;					// 橘子系統由外部傳入帳密
	string					m_password;					// 橘子系統由外部傳入帳密	

	vector<AttachObjectInfo>	m_attachObject;

	double					m_lastDummySpriteUpdateTime;
	string					m_serverCountry;
	int						m_IPNum;

	PacketReduceState		m_packetReduceState;			// 封包衰減狀態
	double					m_nextPacketReduceUpdateTime;	// 下次更新時間
	int						m_packetReducePing;				// 上次Ping時間

	float					m_magicDistance;			// 施法距離
	bool					m_displayMaintainServer;	// 顯示維修中伺服器

	bool					m_isFirstLoading;			// 初次LOADING
	float					m_playerHeight;				// 玩家所在Y軸高度	
	UINT					m_checkPlayerHeight;		//
	float					m_playerCheckValue;			// 檢查Y軸座標值是否正確
	float					m_playerHeightErrorTime;	// 玩角座標錯誤時間
	float					m_lastCheckHeightTime;		// 上次檢查相點座標斜率	
	CRuVector3				m_lastCheckHeightPosition;	// 上次檢查座標點
	int						m_illegalPositionCount;		// 檢查座標點錯誤次數

	int						m_zoneDBID;					// 跨區ZONE,一般為戰場內DBID
	set<int>				m_zoneOpen;					// 已經開啟的區域
	
	bool					m_botLocked;				// 機器人行為被鎖住
	
	float					m_instanceWaitTime;			// 等待玩家決定是否同意副本進度
	bool					m_requestInstance;			// 重新要求副本同意
	
	DWORD					m_currentMemory;			// 目前記憶體使用量
	DWORD					m_memoryLoad;				// 記憶體使用比例	
	double					m_lastMemoryUpdateTime;
	bool					m_reduceTextureState;		// 降低材質精細度狀態
	RuPropertyType			m_paperdollTextureDetail;	// 角色原始貼圖精細度
	RuPropertyType			m_textureDetail;			// 一般貼圖精細度

	int						m_attachMountId;			// 附加物件編號
	bool					m_isLoadClientData;			// 是否由SERVER載入存檔資料
	map<int, float>			m_sendKeyTime;				// 送出取得特定旗標時間
	bool					m_inMount;					// 是否在載具上
	bool					m_inImplement;				// 是否在載具上
	float					m_delayGetImplementTime;	// 等待時間抓取法術類型

	bool					m_bIsVoiceSettingsDisable;	// 判斷是否不開放語音設定
	map<int, int>			m_roomValue;				// 進入獨立房間特有參數

	ClientSaveSetting		m_clientSetting;			// 客戶端記錄資料
	ClientSaveSetting		m_clientSettingBackup;		// 客戶端最後儲存資料
	double					m_lastClientSaveTime;		// 最後改變儲存時間
	bool					m_isFreeCamera;				// 自動移動鏡頭
	CRoleSprite*			m_cameraFollowSprite;		// 鎖定角色鏡頭在另一角物件上

	TargetLineEffectType	m_targetLineType;			// 
	CRoleSprite*			m_targetReferenceSprite;	// 目的參考物件
	CRuVector3				m_targetDirection;			// 砲塔角度
	float					m_targetParabolaHeight;		// 拋物線高度
	float					m_targetMaxRange;			// 最遠攻擊距離
	float					m_targetMaxAngle;			// 目的角度
	float					m_targetDisplayDuration;	// 最後顯示時間

	// 更換地圖
	vector<ZoneDescriptorCallback>	m_ZoneDescriptorList;
	map<int, CRuVector3>			m_spriteAnchorHeight;		// 等待玩家都載入在測試高度


	SystemFunctionState		m_systemEnable;

	bool					m_movePartitionEnable;		// 移動重新設定分割區
	bool					m_vipDisplay;
	bool					m_vipValid;		
	bool					m_bMerchantFrame;
	bool					m_cameraControl;			// 攝影機轉換為 script 自動控制

	UpdateInfInfo			m_updateInf;

	int						m_mountObjectId;
	float					m_mountHeight;
	bool					m_skipCastSpellCheck;	
	CRuVector3				m_lastFixedPosition;
	float					m_lastFixedPositionTime;

	//效能記錄用
	vector<float>			m_Fpslog;
	vector<float>			m_Cpulog;
	vector<float>			m_Memlog;
	CpuUsage				m_CpuUsage;
	double					m_checkMemoryTime;
	bool					m_boReportSelf;
	DWORD					m_checkHeightCount;

	//ZShop
	bool					m_enableZShop;				//ZShop開關
	string					m_ZShopURL;					//ZShop網址

	int						m_iWBWidth;
	int						m_iWBHeight;	

#ifdef KALYDO
	static void detectWDBParts(CRuDataStore_MultiFile* mapFileDataStore);

	void handleWDB(unsigned int errorType, unsigned int errorCode, void* id, 
					   void* message, unsigned int mLength);

	void finishWDB();
	// A handler for the package result for downloading the WDB's.
	THandler* m_HandleWDBPackage;

	// A boolean whether or not we are downloading a WDB.
	bool					m_isDownloading;
	// A boolean whether or not the 'SetCameraPos' function has already been called;
	// as this function contains the loading of the WDB files.
	bool					m_shouldSetCameraPos;
	// The position given to the SetCameraPos function.
	CRuVector3				m_cameraPos;

	// The progress for the WDB download
	float					m_WDBDownloadProgress;
#endif // KALYDO
};

#endif //_GAMEMAIN_H