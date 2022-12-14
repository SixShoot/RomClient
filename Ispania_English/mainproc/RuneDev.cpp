#include "RuneDev.h"
#include "../resource.h"
#include "../tools/CIni/Ini.h"
#include "../tools/getfontnamefromfile/getfontnamefromfile.h"

#ifdef KALYDO
#include "../../Kalydo/SocialIntegration/TRunesOfMagicApplication.h"
#endif // KALYDO

#define REMOVE_ROLE_TIME			2.0f
#define SELECT_AREA_OVER_ENTITY		"model\\item\\ui_object\\target_marker\\act_forbid.ros"
//#define SELECT_AREA_ENTITY			"model\\item\\ui_object\\target_marker\\act_forbid.ros"
#define SELECT_AREA_ENTITY			"model\\fx\\ae_range\\act_ae_range_t_freeze_down.ros"
#define ACT_MOVE_TARGET_PATH		"model\\item\\ui_object\\target_marker\\act_target_marker.ros"

#define MAX_CAMERA_DISTANCE					150.0f
#define MIN_CAMERA_DISTANCE					10.0f

const TCHAR						g_WCNameStr[] = _T("Radiant Arcana");
const INT32						g_resourceArenaSize = 8388608 * 5;

// Pathfinder settings
const INT32						g_pathMaximumExpansion = 1024;

CRuneDev* g_pRuneDev = NULL;
bool							g_out = false;
float							g_handleTime[20];

BOOL							g_enableProfilerOutput = FALSE;
INT32							g_probe_Update_Terrain;
INT32							g_probe_Update_Sprite;
INT32							g_probe_Update_Interface;
INT32							g_probe_Update_Environment;
INT32							g_probe_Update_UpdateInvokeCount;
INT32							g_probe_Update_EventRaiseCount;
INT32							g_probe_Update_ListenerInvokeCount;
INT32							g_probe_Submit_Terrain;
INT32							g_probe_Submit_Sprite;
INT32							g_probe_Submit_Interface;
INT32							g_probe_Submit_Environment;
INT32							g_probe_C0;
INT32							g_probe_C1;
INT32							g_probe_C2;
INT32							g_probe_C3;

BOOL							g_debug_EnableThrottle = FALSE;
BOOL							g_debug_EnableRender = TRUE;
BOOL							g_debug_FlipPresent = FALSE;

struct TestQuality {
	float time;
	const char* name;
};

TestQuality						g_qualityRecord[16] = {
	0.0f, "Terrain Update",		//0
	0.0f, "Sky Update",			//1
	0.0f, "Sprite Update",		//2
	0.0f, "UI Update",			//3
	0.0f, "Update",				//4
	0.0f, "BeginScene",			//5
	0.0f, "Terrain Render",		//6
	0.0f, "Sky Render",			//7
	0.0f, "Sprite Render",		//8
	0.0f, "3D Scene End",		//9
	0.0f, "Blur Render",		//10
	0.0f, "UI Render",			//11
	0.0f, "Other 2D Render(Text, Cursor..)",			//12
	0.0f, "End Scene",			//13
	0.0f, "Present",			//14
	0.0f, "Render",				//15
};

LRESULT CALLBACK	FrameWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void				SpriteMessageProc(IBaseSprite* sprite, SpriteMessageType msg, INT64 arg1, INT64 arg2);
void				FontTextureRally();

// ------------------------------------------------------------------------------------------------
CRuneDev::CRuneDev()
{
	g_pRuneDev = this;

	m_hInstance = NULL;
	m_hFrameWnd = NULL;
	m_isWindowed = false;
	m_isExit = false;
	m_isActived = false;
	m_windowStyle = 0;
	m_xScreen = 0;
	m_yScreen = 0;
	m_frameTime = 0;
	m_frameTimeMS = 0;
	m_elapsedTime = 0.0f;
	m_fps = 0.0f;
	m_isHardwaveCursor = true;
	m_isCursorVisible = true;
	m_isCursorLock = false;
	m_isRuneRelease = false;
	m_isAdjustWindow = false;

	m_pRuVM = NULL;
	m_pRuAudioLibrary = NULL;
	m_pRuSwapChain = NULL;
	m_failedEngineResetCount = 0;
	m_failedPresentCount = 0;
	m_pRuTimer = NULL;
	m_pRuTerrain = NULL;
	m_pOldRuTerrain = NULL;
	m_pRuMinimap = NULL;
	m_pCamera = NULL;
	m_isFirstPersonCamera = FALSE;
	m_pRuWorldEnvironment = NULL;
	m_activeZoneType = RuWorld::ruZONETYPE_OUTDOOR;
	m_timeSinceLastWorldEnvironmentUpdate = 1000.0f;
	m_forceMainLightColor = 0;
	m_timeGroup = 0;
	m_pRuCameraController = NULL;
	m_pInputCOM = NULL;
	m_pKeyboard = NULL;
	m_pMouse = NULL;
	m_pCursorIcon = NULL;
	m_pDragCursorIcon = NULL;
	m_pCursorIcon_HW = NULL;
	m_pDragCursorIcon_HW = NULL;
	m_pCursor_HW = NULL;
	m_pSysFontType = NULL;
	m_pSysFontType_Small = NULL;
	m_pRuHighlighter = NULL;

	m_pRuPathFinder = NULL;
	m_pRuPathHierarchy = NULL;
	m_pathPoints.Clear();

	m_pRuWidgetGrid = NULL;
	m_gridPos = CRuVector3(0.0f, 0.0f, 0.0f);
	m_isGridLock = false;
	m_gridHalfSize = 100;
	m_gridSpacing = 5;

	m_cameraYPosition = 50.0f;
	m_cameraDistance = 70.0f;
	m_cameraMoveTime = 0.0f;
	m_cameraMoveVector = CRuVector3(0.0f, 0.0f, 0.0f);

	m_interface = NULL;
	m_spriteContainer.clear();
	m_DBIDMap.clear();
	m_playerPos.Set(0.0f, 0.0f, 0.0f);

	m_updateCameraController = false;
	m_cameraNearPlane = 1.0f;
	m_cameraFarPlane = 1800.0f;
	m_cameraFOV = 0.9599f;
	m_cameraBackdropNearPlane = 960.0f;
	m_cameraBackdropFarPlane = 4800.0f;
	m_cameraTempFarPlane = 0.0f;
	m_cameraZoomLocked = true;

	m_isSprite_Update = true;
	m_isSprite_Render = true;
	m_isShowFps = false;
	m_isGlow = false;
	m_isEnvironmentUpdate = true;
	m_isEnvironmentRender = true;
	m_isStableFrame = false;

	m_fogMin = 500;
	m_fogMax = 1000;

	memset(&m_clientRect, 0, sizeof(m_clientRect));
	memset(&m_windowRect, 0, sizeof(m_windowRect));
	memset(m_windowName, 0, sizeof(m_windowName));
	memset(m_resourceRootDirectoryStr, 0, sizeof(m_resourceRootDirectoryStr));
	memset(m_specialEventFilter, 0, sizeof(m_specialEventFilter));
	strcpy(m_specialEventFilter, "none");

	m_editingModeOn = false;
	m_editingDragOn = false;
	m_editingSprite = NULL;

	m_flagEditor_DragOn = false;
	m_flagEditor_EditFlagIdx = -1;
	m_flagEditor_EditFlag = NULL;

	m_paperdollCache = ruNEW CRuPaperdollCache();
	m_paperdollCache->SetCachePath("cache");
	m_paperdollCache_ActivateWrites = FALSE;

	m_pRuSelectAreaEntity = NULL;
	m_pRuSelectAreaStopEntity = NULL;
	m_pRuMoveTargetEntity = NULL;
	m_selectAreaDistance = 0.0f;

	m_isBlind = false;

	m_uifx_Model = NULL;
	m_uifx_ModelEntity = NULL;

	m_deleteRoleSpriteCount = 0;
	m_lastFrameRoleSpriteCount = 0;
	m_lastLoadTerrainTime = 0.0f;

	m_debaseTexture = false;
	m_disableMemoryWarning = false;
	m_targetEntity = NULL;			// 目的物件
	m_targetPolyLineEntity = NULL;			// 目的物件串接特效
	m_targetParentEntity = NULL;
	m_targetEntityVisible = true;
	m_targetAttachPoint.clear();

	memset(&m_restoredWindowRect, 0, sizeof(m_restoredWindowRect));
}

// ------------------------------------------------------------------------------------------------
CRuneDev::~CRuneDev()
{
	RuneDevRelease();
}

// ------------------------------------------------------------------------------------------------
bool CRuneDev::CreateWnd(HINSTANCE hInstance, int screenWidth, int screenHeight, HWND parent)
{
	WNDCLASSEX WndClass;

	m_hInstance = hInstance;

	// Register frame window
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = 0;
	WndClass.lpfnWndProc = FrameWndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = g_WCNameStr;
	WndClass.hIconSm = NULL;
	RegisterClassEx(&WndClass);

	RECT rect;
	if (parent)
	{
		m_windowStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		GetClientRect(parent, &rect);
	}
	else
	{
		m_windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		// Set the window's initial width
		SetRect(&rect, 0, 0, screenWidth, screenHeight);
		AdjustWindowRect(&rect, m_windowStyle, FALSE);
	}

	m_hFrameWnd = CreateWindowEx(0, g_WCNameStr, m_windowName, m_windowStyle, 0, 0,
		rect.right - rect.left, rect.bottom - rect.top, parent, NULL, hInstance, NULL);

	if (m_hFrameWnd)
	{
		TRACKMOUSEEVENT track;

		UpdateWindow(m_hFrameWnd);
		ShowWindow(m_hFrameWnd, SW_SHOW);
		GetWindowRect(m_hFrameWnd, &m_windowRect);
		GetClientRect(m_hFrameWnd, &m_clientRect);

		memset(&track, 0, sizeof(track));
		track.hwndTrack = m_hFrameWnd;
		track.cbSize = sizeof(TRACKMOUSEEVENT);
		track.dwFlags = TME_HOVER;
		track.dwHoverTime = 1;
		BOOL result = TrackMouseEvent(&track);
		return TRUE;
	}

	return FALSE;
}

// --------------------------------------------------------------------------------------
bool CRuneDev::SaveSystemVariables()
{
	char filename[512];
	const char* savePath = g_ruResourceManager->GetRootDirectory();
	if (savePath && strlen(savePath) > 0)
		sprintf(filename, "%s%s", savePath, "client.config.ini");
	else
		strcpy(filename, "client.config.ini");

	CIni sysConfig(filename);
	BOOL boResult;

	boResult = sysConfig.WriteInt("display", "windowed width", m_windowedResolution.cx);
	boResult = sysConfig.WriteInt("display", "windowed height", m_windowedResolution.cy);
	boResult = sysConfig.WriteInt("display", "fullscreen width", m_fullscreenResolution.cx);
	boResult = sysConfig.WriteInt("display", "fullscreen height", m_fullscreenResolution.cy);
	boResult = sysConfig.WriteBool("display", "windowed mode", m_isWindowed);

	RuPropertyType propertyValue;

	boResult = sysConfig.WriteInt("display", "quality", g_ruEngineSettings->GetSetting("display:quality", propertyValue) ? boost::get<INT32>(propertyValue) : 0);

	boResult = sysConfig.WriteBool("ui", "ui scale mode", g_ruEngineSettings->GetSetting("uiscale:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE);
	boResult = sysConfig.WriteDouble("ui", "ui scale", g_ruEngineSettings->GetSetting("ui:scale", propertyValue) ? boost::get<REAL>(propertyValue) : 1.0f);

	sysConfig.WriteDouble("visual detail", "view distance", (g_ruEngineSettings->GetSetting("world:view distance", propertyValue) ? boost::get<REAL>(propertyValue) : 2000.0f) - 500.0f);
	sysConfig.WriteDouble("visual detail", "camera distance", m_cameraDistance);
	sysConfig.WriteInt("visual detail", "terrain detail class", g_ruEngineSettings->GetSetting("terrain:detail class", propertyValue) ? boost::get<INT32>(propertyValue) : 3);
	sysConfig.WriteInt("visual detail", "sky detail", g_ruEngineSettings->GetSetting("gfx:sky detail", propertyValue) ? boost::get<INT32>(propertyValue) : 1);
	sysConfig.WriteInt("visual detail", "texture detail", g_ruEngineSettings->GetSetting("texture:max detail level", propertyValue) ? boost::get<INT32>(propertyValue) : 4);
	sysConfig.WriteBool("visual detail", "debase texture", m_debaseTexture);
	sysConfig.WriteBool("visual detail", "disable memory warning", m_disableMemoryWarning);
	sysConfig.WriteInt("visual detail", "splat detail", g_ruEngineSettings->GetSetting("terrain:splat detail", propertyValue) ? boost::get<INT32>(propertyValue) : 3);
	sysConfig.WriteInt("visual detail", "terrain shader detail", g_ruEngineSettings->GetSetting("terrain:splat technique", propertyValue) ? boost::get<INT32>(propertyValue) : 2);

	sysConfig.WriteInt("character detail", "paperdoll detail", g_ruEngineSettings->GetSetting("paperdoll:max mip level", propertyValue) ? boost::get<INT32>(propertyValue) : 2);

	sysConfig.WriteBool("visual detail", "rtlightmap enable", g_ruEngineSettings->GetSetting("RTLightMap:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE);
	sysConfig.WriteInt("visual detail", "rtlightmap size", g_ruEngineSettings->GetSetting("RTLightMap:texture size", propertyValue) ? boost::get<INT32>(propertyValue) : 256);
	sysConfig.WriteInt("visual detail", "shadow detail", g_ruEngineSettings->GetSetting("shadow:detail level", propertyValue) ? boost::get<INT32>(propertyValue) : 3);

	sysConfig.WriteBool("visual effects", "bloom", g_ruEngineSettings->GetSetting("glowFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE);
	sysConfig.WriteBool("visual effects", "water reflection", g_ruEngineSettings->GetSetting("gfx:water reflection", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE);
	sysConfig.WriteBool("visual effects", "water refraction", g_ruEngineSettings->GetSetting("gfx:water refraction", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE);
	sysConfig.WriteBool("visual effects", "distort fx", g_ruEngineSettings->GetSetting("distortFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE);
	sysConfig.WriteBool("visual effects", "specular highlight", g_ruEngineSettings->GetSetting("gfx:specular highlight", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE);

	sysConfig.WriteDouble("audio", "master volume", g_ruEngineSettings->GetSetting("audioFX:master volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f);
	sysConfig.WriteDouble("audio", "ambience volume", g_ruEngineSettings->GetSetting("audioFX:ambience volume", propertyValue) ? boost::get<REAL>(propertyValue) : 0.500f);
	sysConfig.WriteDouble("audio", "music volume", g_ruEngineSettings->GetSetting("audioFX:music volume", propertyValue) ? boost::get<REAL>(propertyValue) : 0.500f);
	sysConfig.WriteDouble("audio", "sound fx volume", g_ruEngineSettings->GetSetting("audioFX:sound fx volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f);
	sysConfig.WriteDouble("audio", "interface sfx volume", g_ruEngineSettings->GetSetting("audioFX:interface sfx volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f);
	sysConfig.WriteDouble("audio", "music frequency", g_ruEngineSettings->GetSetting("audioFX:music frequency", propertyValue) ? boost::get<REAL>(propertyValue) : 50.000f);

	// Vivox Settings
	sysConfig.WriteBool(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_ENABLE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_ENABLE, propertyValue) ? boost::get<BOOL>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_ENABLE_DEF);
	sysConfig.WriteBool(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_MUTE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MUTE, propertyValue) ? boost::get<BOOL>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_MUTE_DEF);
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_MODE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MODE, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_MODE_DEF); //1-2
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX_DEF);
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_VOL, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_VOL, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_VOL_DEF); //0-100
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_SEN, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_SEN, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_SEN_DEF); //0-100
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX_DEF);
	sysConfig.WriteInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_OUTPUT_VOL, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_VOL, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_VOL_DEF); //0-100

	return true;
}

// --------------------------------------------------------------------------------------
bool CRuneDev::LoadSystemVariables()
{
	char filename[512];
	const char* savePath = g_ruResourceManager->GetRootDirectory();
	if (savePath && strlen(savePath) > 0)
		sprintf(filename, "%s\\%s", savePath, "client.config.ini");
	else
		GetFullPathName("client.config.ini", MAX_PATH, filename, NULL);

	CIni sysConfig(filename);

	m_windowedResolution.cx = sysConfig.GetInt("display", "windowed width", 1024);
	m_windowedResolution.cy = sysConfig.GetInt("display", "windowed height", 768);
	m_fullscreenResolution.cx = sysConfig.GetInt("display", "fullscreen width", 1024);
	m_fullscreenResolution.cy = sysConfig.GetInt("display", "fullscreen height", 768);
#ifdef KALYDO
	// Kalydo always starts windowed.
	m_isWindowed = true;
#else
	m_isWindowed = (sysConfig.GetBool("display", "windowed mode", false) != 0);
#endif

	RECT windowRect;
	GetWindowRect(GetDesktopWindow(), &windowRect);

	if (m_isWindowed)
	{
		if (m_windowedResolution.cx > windowRect.right)
			m_windowedResolution.cx = windowRect.right;
		if (m_windowedResolution.cy > windowRect.bottom)
			m_windowedResolution.cy = windowRect.bottom;
	}
	else
	{
		bool isAdjustSize = true;
		bool desttopWindowsSize = false;

		for (INT32 i = 0; i < g_ruEngine->GetNumAvailableDisplayModes(); ++i)
		{
			RuDisplayMode dispMode = g_ruEngine->GetAvailableDisplayModeByIndex(i);

			if (dispMode.m_format == ruSURFACEFMT_X8R8G8B8)
			{
				if (dispMode.m_width == m_fullscreenResolution.cx && dispMode.m_height == m_fullscreenResolution.cy)
				{
					isAdjustSize = false;
					break;
				}
				else if (dispMode.m_width == windowRect.right && dispMode.m_height == windowRect.bottom)
				{
					desttopWindowsSize = true;
				}
			}
		}

		// 搜尋適合大小
		if (isAdjustSize)
		{
			// 如果桌面大小適合的
			if (desttopWindowsSize)
			{
				m_fullscreenResolution.cx = windowRect.right;
				m_fullscreenResolution.cy = windowRect.bottom;
			}
			else
			{
				m_fullscreenResolution.cx = 1024;
				m_fullscreenResolution.cy = 768;
			}
		}
	}

	RuPropertyType propertyValue;

	g_ruEngineSettings->SetSetting("display:quality", static_cast<INT32>(sysConfig.GetInt("display", "quality", g_ruEngineSettings->GetSetting("display:quality", propertyValue) ? boost::get<INT32>(propertyValue) : 0)));

	g_ruEngineSettings->SetSetting("uiscale:enable", static_cast<BOOL>(sysConfig.GetBool("ui", "ui scale mode", g_ruEngineSettings->GetSetting("uiscale:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE)));
	g_ruEngineSettings->SetSetting("ui:scale", static_cast<REAL>(sysConfig.GetDouble("ui", "ui scale", g_ruEngineSettings->GetSetting("ui:scale", propertyValue) ? boost::get<REAL>(propertyValue) : 1.0f)));

	g_ruEngineSettings->SetSetting("world:view distance", static_cast<REAL>(sysConfig.GetDouble("visual detail", "view distance", (g_ruEngineSettings->GetSetting("world:view distance", propertyValue) ? boost::get<REAL>(propertyValue) : 2000.0f)) + 500.0f));
	m_cameraDistance = (float)sysConfig.GetDouble("visual detail", "camera distance", m_cameraDistance);

	g_ruEngineSettings->SetSetting("terrain:detail class", static_cast<INT32>(sysConfig.GetInt("visual detail", "terrain detail class", g_ruEngineSettings->GetSetting("terrain:detail class", propertyValue) ? boost::get<INT32>(propertyValue) : 3)));
	g_ruEngineSettings->SetSetting("gfx:sky detail", static_cast<INT32>(sysConfig.GetInt("visual detail", "sky detail", g_ruEngineSettings->GetSetting("gfx:sky detail", propertyValue) ? boost::get<INT32>(propertyValue) : 1)));
	g_ruEngineSettings->SetSetting("texture:max detail level", static_cast<INT32>(sysConfig.GetInt("visual detail", "texture detail", g_ruEngineSettings->GetSetting("texture:max detail level", propertyValue) ? boost::get<INT32>(propertyValue) : 4)));
	m_debaseTexture = static_cast<BOOL>(sysConfig.GetBool("visual detail", "debase texture", m_debaseTexture));
	m_disableMemoryWarning = static_cast<BOOL>(sysConfig.GetBool("visual detail", "disable memory warning", m_disableMemoryWarning));

	g_ruEngineSettings->SetSetting("terrain:splat detail", static_cast<INT32>(sysConfig.GetInt("visual detail", "splat detail", g_ruEngineSettings->GetSetting("terrain:splat detail", propertyValue) ? boost::get<INT32>(propertyValue) : 3)));
	g_ruEngineSettings->SetSetting("terrain:splat technique", static_cast<INT32>(sysConfig.GetInt("visual detail", "terrain shader detail", g_ruEngineSettings->GetSetting("terrain:splat technique", propertyValue) ? boost::get<INT32>(propertyValue) : 2)));

	g_ruEngineSettings->SetSetting("paperdoll:max mip level", static_cast<INT32>(sysConfig.GetInt("character detail", "paperdoll detail", g_ruEngineSettings->GetSetting("paperdoll:max mip level", propertyValue) ? boost::get<INT32>(propertyValue) : 2)));

	//g_ruEngineSettings->SetSetting("RTLightMap:enable", static_cast<BOOL>(sysConfig.GetBool("visual detail", "rtlightmap enable", FALSE)));
	//g_ruEngineSettings->SetSetting("RTLightMap:enable", static_cast<BOOL>(sysConfig.GetBool("visual detail", "rtlightmap enable", g_ruEngineSettings->GetSetting("RTLightMap:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE)));
	//g_ruEngineSettings->SetSetting("RTLightMap:texture size", static_cast<INT32>(sysConfig.GetInt("visual detail", "rtlightmap size", g_ruEngineSettings->GetSetting("RTLightMap:texture size", propertyValue) ? boost::get<INT32>(propertyValue) : 256)));
	g_ruEngineSettings->SetSetting("RTLightMap:enable", static_cast<BOOL>(FALSE));
	g_ruEngineSettings->SetSetting("shadow:detail level", static_cast<INT32>(sysConfig.GetInt("visual detail", "shadow detail", g_ruEngineSettings->GetSetting("shadow:detail level", propertyValue) ? boost::get<INT32>(propertyValue) : 3)));

	g_ruEngineSettings->SetSetting("glowFX:enable", static_cast<BOOL>(sysConfig.GetBool("visual effects", "bloom", g_ruEngineSettings->GetSetting("glowFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE)));
	g_ruEngineSettings->SetSetting("gfx:water reflection", static_cast<BOOL>(sysConfig.GetBool("visual effects", "water reflection", g_ruEngineSettings->GetSetting("gfx:water reflection", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE)));
	g_ruEngineSettings->SetSetting("gfx:water refraction", static_cast<BOOL>(sysConfig.GetBool("visual effects", "water refraction", g_ruEngineSettings->GetSetting("gfx:water refraction", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE)));
	g_ruEngineSettings->SetSetting("distortFX:enable", static_cast<BOOL>(sysConfig.GetBool("visual effects", "distort fx", g_ruEngineSettings->GetSetting("distortFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE)));
	g_ruEngineSettings->SetSetting("gfx:specular highlight", static_cast<BOOL>(sysConfig.GetBool("visual effects", "specular highlight", g_ruEngineSettings->GetSetting("gfx:specular highlight", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE)));

	g_ruEngineSettings->SetSetting("audioFX:master volume", static_cast<REAL>(sysConfig.GetDouble("audio", "master volume", g_ruEngineSettings->GetSetting("audioFX:master volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f)));
	g_ruEngineSettings->SetSetting("audioFX:ambience volume", static_cast<REAL>(sysConfig.GetDouble("audio", "ambience volume", g_ruEngineSettings->GetSetting("audioFX:ambience volume", propertyValue) ? boost::get<REAL>(propertyValue) : 0.500f)));
	g_ruEngineSettings->SetSetting("audioFX:music volume", static_cast<REAL>(sysConfig.GetDouble("audio", "music volume", g_ruEngineSettings->GetSetting("audioFX:music volume", propertyValue) ? boost::get<REAL>(propertyValue) : 0.500f)));
	g_ruEngineSettings->SetSetting("audioFX:sound fx volume", static_cast<REAL>(sysConfig.GetDouble("audio", "sound fx volume", g_ruEngineSettings->GetSetting("audioFX:sound fx volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f)));
	g_ruEngineSettings->SetSetting("audioFX:interface sfx volume", static_cast<REAL>(sysConfig.GetDouble("audio", "interface sfx volume", g_ruEngineSettings->GetSetting("audioFX:interface sfx volume", propertyValue) ? boost::get<REAL>(propertyValue) : 1.000f)));
	g_ruEngineSettings->SetSetting("audioFX:music frequency", static_cast<REAL>(sysConfig.GetDouble("audio", "music frequency", g_ruEngineSettings->GetSetting("audioFX:music frequency", propertyValue) ? boost::get<REAL>(propertyValue) : 50.000f)));

	// Vivox Settings
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_ENABLE, static_cast<BOOL>(sysConfig.GetBool(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_ENABLE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_ENABLE, propertyValue) ? boost::get<BOOL>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_ENABLE_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MUTE, static_cast<BOOL>(sysConfig.GetBool(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_MUTE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MUTE, propertyValue) ? boost::get<BOOL>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_MUTE_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MODE, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_MODE, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_MODE, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_MODE_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_DEV_IDX_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_VOL, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_VOL, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_VOL, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_VOL_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_SEN, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_INPUT_SEN, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_INPUT_SEN, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_INPUT_SEN_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_DEV_IDX_DEF)));
	g_ruEngineSettings->SetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_VOL, static_cast<INT32>(sysConfig.GetInt(CONFIG_SETTINGS_VOICE_CHAT, CONFIG_SETTINGS_VOICE_CHAT_OUTPUT_VOL, g_ruEngineSettings->GetSetting(ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_VOL, propertyValue) ? boost::get<INT32>(propertyValue) : ENGINE_SETTINGS_VOICE_CHAT_OUTPUT_VOL_DEF)));

	////////////////////////////////////////////////////////

	if (sysConfig.IsSectionExist("display") == FALSE)
	{
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------
bool CRuneDev::RuneDevInitialize(HINSTANCE hInstance, int screenWidth, int screenHeight, bool isWindowed, HWND window /* = NULL */)
{
	if (m_hFrameWnd)
		return FALSE;
	if (!CreateWnd(hInstance, screenWidth, screenHeight, window))
		return FALSE;

	// 
	CErrorList::Initialize(hInstance, m_hFrameWnd);

	BOOL coreInitializeOK = RuInitialize_Core();

	// 開啟引擎並保存回傳 SwapChain
	if ((m_pRuSwapChain = RuInitialize_D3D(m_hFrameWnd, m_hFrameWnd, g_resourceArenaSize)) == NULL)
	{
		return FALSE;
	}

	BOOL sceneInitializeOK = RuInitialize_Scene(m_pRuSwapChain);

	// 開啟音效系統
	if (RuSymphony_Initialize() == FALSE)
	{
		return FALSE;
	}

	// 開啟音效庫
	m_pRuVM = lua_open();
	RuFusion_AudioLibrary_Register(m_pRuVM);
	m_pRuAudioLibrary = RuFusion_GetAudioLibrary();

	// Initialize performance probes
	g_probe_Update_Terrain = g_ruProfiler->CreateProbe("Update Terrain");
	g_probe_Update_Sprite = g_ruProfiler->CreateProbe("Update Sprite");
	g_probe_Update_Interface = g_ruProfiler->CreateProbe("Update Interface");
	g_probe_Update_Environment = g_ruProfiler->CreateProbe("Update Environment");
	g_probe_Update_UpdateInvokeCount = g_ruProfiler->CreateProbe("Update Invoke Cnt");
	g_probe_Update_EventRaiseCount = g_ruProfiler->CreateProbe("Event Raise Cnt");
	g_probe_Update_ListenerInvokeCount = g_ruProfiler->CreateProbe("Listener Invoke Cnt");

	g_probe_Submit_Terrain = g_ruProfiler->CreateProbe("Submit Terrain");
	g_probe_Submit_Sprite = g_ruProfiler->CreateProbe("Submit Sprite");
	g_probe_Submit_Interface = g_ruProfiler->CreateProbe("Submit Interface");
	g_probe_Submit_Environment = g_ruProfiler->CreateProbe("Submit Environment");

	g_probe_C0 = g_ruProfiler->CreateProbe("C0");
	g_probe_C1 = g_ruProfiler->CreateProbe("C1");
	g_probe_C2 = g_ruProfiler->CreateProbe("C2");
	g_probe_C3 = g_ruProfiler->CreateProbe("C3");

	// 建立主鏡頭
	m_isRuneRelease = true;
	m_pCamera = ruNEW CRuCamera;
	m_pCamera->EnableYSmoothing(TRUE);
	m_pCamera->SetCameraPosition(CRuVector3(0.0f, 20.0f, -30.0f));	// 鏡頭所在位置
	m_pCamera->SetTargetPosition(CRuVector3(0.0f, 10.0f, 0.0f));		// 鏡頭目的位置
	m_pCamera->SetCameraUpVector(CRuVector3(0.0f, 1.0f, 0.0f));		// 鏡頭上方所朝方向	

	g_ruResourceManager->CreateRenderTargetSubAtlas(512, 256, ruSURFACEFMT_X1R5G5B5);
	g_ruResourceManager->CreateRenderTargetSubAtlas(512, 256, ruSURFACEFMT_X1R5G5B5);
	g_ruResourceManager->CreateRenderTargetSubAtlas(512, 256, ruSURFACEFMT_X1R5G5B5);
	g_ruResourceManager->CreateRenderTargetSubAtlas(512, 256, ruSURFACEFMT_X1R5G5B5);

	m_pRuCameraController = ruNEW CRuController_Camera();
	m_pRuCameraController->SetTarget(m_pCamera);
	g_ruAudible->SetCamera(m_pCamera);
	// Initialize hardware cursor texture
//	m_pCursor_HW = g_ruResourceManager->CreateTexture("ruCURSOR", 32, 32, 1, ruSURFACEFMT_A8R8G8B8);
	m_pCursor_HW = g_ruEngine->CreateTexture(32, 32, 1, ruSURFACEFMT_A8R8G8B8, ruRESPOOL_MANAGED);

	void* pData;
	INT32 patch;
	IRuSurface* Sur = m_pCursor_HW->GetLevelSurface(0);
	if (Sur->Lock((void**)&pData, &patch))
	{

		Sur->Unlock();
	}
	// 界面初始化
	CInterface* pInterface = ruNEW CInterface(m_resourceRootDirectoryStr, m_hFrameWnd);

	m_interface = pInterface;
	// 更新子系統姆目錄設定
	UpdateSubSystemResourcePaths();

	// 載入 DB Data
	LoadData();

	// Load system variables
	bool sysVarLoadSuccessful = LoadSystemVariables();

	// 貼圖初始化
	CTextureImage::Initialize();

	// 字型初始化
	//CRuFontChar::Initialize( FontTextureRally );
	CRuFontString::Initialize(FontTextureRally);

	// 產生系統使用文字物件
	const char* fullPathname = GetFullFilePath(GetDefaultFontPath().c_str());
	int lResult = AddFontResourceEx(fullPathname, FR_PRIVATE, 0);

	string fontName = GetFontNameFromFile(fullPathname);
	m_pSysFontType = CRuFontType::GetFontType(fontName.c_str(), 18, ruFONTWEIGHT_BOLD, ruFONTOUTLINE_NORMAL);
	m_pSysFontType_Small = CRuFontType::GetFontType(fontName.c_str(), 12, ruFONTWEIGHT_BOLD, ruFONTOUTLINE_NORMAL);

	IBaseSprite::Initialize();
	IBaseSprite::SetFontType(m_pSysFontType);
	IBaseSprite::SetPaperdollCache(m_paperdollCache);

	// 設定天空
	InitialEnvironment();

	// 將音效庫設入事件管理器
	g_pRuGlobalEventManager->SetAudioLibrary(m_pRuAudioLibrary);

	// Setup internal path finder
	m_pRuPathFinder = ruNEW CRuPathFinder_AStar();
	m_pRuPathFinder->SetMaximumExpansions(g_pathMaximumExpansion);

	m_pRuTimer = ruNEW CRuPerformanceTimer();
	m_pRuTimer->Initialize(0);
	m_pRuTimer->ElapsedTime();

	m_pRuHighlighter = ruNEW CRuController_Highlighter();
	m_pRuHighlighter->SetPulsePeriod(4.0f);
	m_pRuHighlighter->SetHighlightColor0(CRuColor4(1.0f, 0.1f, 0.1f, 0.1f));
	m_pRuHighlighter->SetHighlightColor1(CRuColor4(1.0f, 0.2f, 0.2f, 0.2f));

	// 產生輸入裝置
	m_pInputCOM = ruNEW CInputCOM;
	m_pInputCOM->Init(m_hFrameWnd, hInstance);
	m_pKeyboard = ruNEW CInputDevice;
	m_pKeyboard->Create(m_pInputCOM, eID_Keyboard);
	m_pMouse = ruNEW CInputDevice;
	m_pMouse->Create(m_pInputCOM, eID_Mouse);

	// Initialize default settings if necessary
	if (!sysVarLoadSuccessful)
	{
		RuneDevInitializeDefaultSettings();
	}

	/*
	RECT clientRect;
	GetClientRect( m_hFrameWnd, &clientRect );
	m_windowedResolution.cx = clientRect.right - clientRect.left;
	m_windowedResolution.cy = clientRect.bottom - clientRect.top;
	*/

	// 設定畫面解析度
	if (m_isWindowed)
	{
		SetScreenSize(m_windowedResolution.cx, m_windowedResolution.cy, m_isWindowed, TRUE);
	}
	else
	{
		SetScreenSize(m_fullscreenResolution.cx, m_fullscreenResolution.cy, m_isWindowed, TRUE);
	}

	// ********************************************************************************************
	//
	// 若有其他系統需要初始化可在這裏初始
	//
	// ********************************************************************************************	
	IBaseSprite::SetSpriteMsgProc(SpriteMessageProc);

	CInterface* pInterface2 = GetInterface();

	if (pInterface2 == pInterface)

		m_interface = pInterface2;
	// Continue GameMain initialization
	Init();

#ifdef RUINCLUDE_MEMORYMANAGER
	// Bookmark initial memory status
	m_memoryBookmark = CRuMemoryHeapFactory::GetMemoryBookmark();
#endif

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
bool CRuneDev::RuneDevInitializeDefaultSettings()
{
	// Get system memory information
	MEMORYSTATUSEX memStatus;
	memset(&memStatus, 0, sizeof(MEMORYSTATUSEX));
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);

	if (GlobalMemoryStatusEx(&memStatus) == 0)
	{
		// Report an error
	}

	// Read display device database
	CRuTernaryStringTree<INT32>	deviceMap;
	CRuFileStream* deviceSettingsStream = ruNEW CRuFileStream();

	if (deviceSettingsStream->Open(GetFullFilePath("devices.ini")) == ruSEC_OK)
	{
		INT32 bufferSize = deviceSettingsStream->GetStreamSize() + 1;
		char* deviceSettingsBuffer = ruNEW char[bufferSize];

		memset(deviceSettingsBuffer, 0, bufferSize);
		deviceSettingsStream->Seek(0, ruSSM_Begin);
		deviceSettingsStream->Read(deviceSettingsBuffer, bufferSize - 1);

		CRuString deviceSettingsString(deviceSettingsBuffer);

		CRuStringTokenizer tokenizer;
		tokenizer.Tokenize(deviceSettingsString, "\n\r");

		for (INT32 i = 0; i < tokenizer.GetNumTokens(); ++i)
		{
			CRuStringTokenizer entryTokenizer;
			entryTokenizer.Tokenize(tokenizer.GetToken(i), ",");

			if (entryTokenizer.GetNumTokens() == 4)
			{
				CRuString deviceKey(entryTokenizer.GetToken(0));
				deviceKey.Append(entryTokenizer.GetToken(1));
				deviceKey.ToLower();

				DWORD settingLevel = atoi(entryTokenizer.GetToken(3));

				deviceMap.Add(deviceKey, settingLevel);
			}
		}

		delete[] deviceSettingsBuffer;
	}

	delete deviceSettingsStream;

	// Build display device key
	char curDeviceKey[128];
	sprintf(curDeviceKey, "0x%04x0x%04x", g_ruEngine->GetAdapterVendorID(), g_ruEngine->GetAdapterDeviceID());

	// Select setting level based on device key, or default to level 5 if the device is not found in the list
	INT32 settingLevel = deviceMap.ContainsKey(curDeviceKey) ? settingLevel = deviceMap.Get(curDeviceKey) : 5;

	// Setup basic display settings based on setting level
	switch (settingLevel)
	{
	case 0:
	{
	}

	break;

	case 1:
	{
		g_ruEngineSettings->SetSetting("world:view distance", 1000.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 0);
		g_ruEngineSettings->SetSetting("shadow:detail level", 0);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 0);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 0);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 0);
		g_ruEngineSettings->SetSetting("gfx:water reflection", FALSE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", FALSE);
		g_ruEngineSettings->SetSetting("glowFX:enable", FALSE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 2);
	}

	break;

	case 2:
	{
		g_ruEngineSettings->SetSetting("world:view distance", 1250.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 0);
		g_ruEngineSettings->SetSetting("shadow:detail level", 0);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 0);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 0);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 0);
		g_ruEngineSettings->SetSetting("gfx:water reflection", FALSE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", FALSE);
		g_ruEngineSettings->SetSetting("glowFX:enable", FALSE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 1);
	}

	break;

	case 3:
	{
		g_ruEngineSettings->SetSetting("world:view distance", 1500.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 2);
		g_ruEngineSettings->SetSetting("shadow:detail level", 1);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 1);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 0);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 1);
		g_ruEngineSettings->SetSetting("gfx:water reflection", FALSE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", FALSE);
		g_ruEngineSettings->SetSetting("glowFX:enable", FALSE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 1);
	}

	break;

	case 4:
	{
		g_ruEngineSettings->SetSetting("world:view distance", 1750.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 3);
		g_ruEngineSettings->SetSetting("shadow:detail level", 2);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 3);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 0);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 1);
		g_ruEngineSettings->SetSetting("gfx:water reflection", FALSE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", FALSE);
		g_ruEngineSettings->SetSetting("glowFX:enable", FALSE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 1);
	}

	break;

	case 5:
	{
		g_ruEngineSettings->SetSetting("display:quality", 2);
		g_ruEngineSettings->SetSetting("world:view distance", 2000.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 3);
		g_ruEngineSettings->SetSetting("shadow:detail level", 3);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 3);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 2);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 1);
		g_ruEngineSettings->SetSetting("gfx:water reflection", TRUE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", TRUE);
		g_ruEngineSettings->SetSetting("glowFX:enable", TRUE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 1);
	}

	break;

	case 6:
	{
		g_ruEngineSettings->SetSetting("world:view distance", 2300.0f);
		g_ruEngineSettings->SetSetting("terrain:detail class", 3);
		g_ruEngineSettings->SetSetting("shadow:detail level", 3);
		g_ruEngineSettings->SetSetting("terrain:splat detail", 3);
		g_ruEngineSettings->SetSetting("terrain:splat technique", 2);
		g_ruEngineSettings->SetSetting("gfx:sky detail", 1);
		g_ruEngineSettings->SetSetting("gfx:water reflection", TRUE);
		g_ruEngineSettings->SetSetting("gfx:water refraction", TRUE);
		g_ruEngineSettings->SetSetting("glowFX:enable", TRUE);

		// Set default paperdoll texture detail level
		g_ruEngineSettings->SetSetting("paperdoll:max mip level", 0);
	}

	break;
	}

	// Enable distort FX
	g_ruEngineSettings->SetSetting("distortFX:enable", TRUE);

	// Set allowable texture mip levels based on physical system memory size
	if (memStatus.ullTotalPhys < 1000000000)
	{
		g_ruEngineSettings->SetSetting("texture:max detail level", 3);
		g_ruEngineSettings->SetSetting("texture:min texture size", 256);
	}
	else
	{
		g_ruEngineSettings->SetSetting("texture:max detail level", 4);
		g_ruEngineSettings->SetSetting("texture:min texture size", 256);
	}

	// Set default audio settings
	g_ruEngineSettings->SetSetting("audioFX:ambience volume", 0.500f);
	g_ruEngineSettings->SetSetting("audioFX:music volume", 0.500f);
	g_ruEngineSettings->SetSetting("audioFX:sound fx volume", 1.000f);
	g_ruEngineSettings->SetSetting("audioFX:interface sfx volume", 1.000f);

	return true;
}

// ------------------------------------------------------------------------------------------------
void CRuneDev::RuneDevRelease()
{
	ClearCursorIcon();
	ClearDragCursorIcon();

	SAFE_DELETE(m_interface);
	SAFE_DELETE(m_pKeyboard);
	SAFE_DELETE(m_pMouse);
	SAFE_DELETE(m_pInputCOM);
	SAFE_DELETE(m_pRuTimer);

	CRuFontString::Shutdown();
	//CRuFontChar::Shutdown();	
	CTextureImage::Shutdown();

	ruSAFE_RELEASE(m_pRuHighlighter);
	ruSAFE_RELEASE(m_pRuSwapChain);
	ruSAFE_RELEASE(m_pRuCameraController);
	ruSAFE_RELEASE(m_pCursor_HW);
	ruSAFE_RELEASE(m_pRuSelectAreaEntity);
	RelaseTargetEntity();
	ClearSelectAreaEntity();
	ClearMoveTargetEntity();
	ReleaseWidgetGrid();

	m_spriteContainer.clear();
	m_DBIDMap.clear();
	IBaseSprite::Shutdown();

	ReleaseTerrain();
	ReleaseOldTerrain();
	ReleaseEnvironment();

	ruSAFE_RELEASE(m_pRuMinimap);
	ruSAFE_RELEASE(m_pCamera);
	ruSAFE_RELEASE(m_paperdollCache);


	// 關閉引擎
	if (m_isRuneRelease)
	{
		m_isRuneRelease = false;
		RuShutdown();
	}

	// 關閉音效系統
	RuSymphony_Shutdown();

	// 關閉音效庫
	m_pRuAudioLibrary = NULL;

	if (m_pRuVM)
	{
		lua_close(m_pRuVM);
		m_pRuVM = NULL;
	}

	Release();

	//
	CErrorList::Shutdown();

#ifdef RUINCLUDE_MEMORYMANAGER
	// Report any memory leaks
	CRuMemoryHeapFactory::ReportMemoryLeaks(m_memoryBookmark);
#endif
}

bool CRuneDev::SetScreenSize(DWORD width, DWORD height, bool isWindowed, bool forceWindowSizeConform)
{
	bool returnValue = false;

	if (m_isAdjustWindow == true)
		return false;

	if (m_xScreen != width || m_yScreen != height || m_isWindowed != isWindowed)
	{
		// Mark us as already adjusting the screen size
		m_isAdjustWindow = true;

		// Store new windowed setting
		m_isWindowed = isWindowed;

		// NOTE this #ifndef does not matter; in browser version forceWindowSizeConform is always false
		if (forceWindowSizeConform)
		{
			// Adjust window style
			if (m_isWindowed)
			{
				m_windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			}
			else
			{
				m_windowStyle = WS_POPUP | WS_VISIBLE;
			}

			SetWindowLong(m_hFrameWnd, GWL_STYLE, m_windowStyle);

			// Adjust window rect based on the current window style
			RECT rc;
			SetRect(&rc, 0, 0, width, height);
			AdjustWindowRect(&rc, m_windowStyle, FALSE);

			if (m_isWindowed)
			{
				SetWindowPos(m_hFrameWnd, HWND_NOTOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW | SWP_NOMOVE);
			}
			else
			{
				POINT origin = { 0, 0 };
				ScreenToClient(m_hFrameWnd, &origin);
				SetWindowPos(m_hFrameWnd, HWND_TOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
			}
		}

		// Update window and client rects
		GetWindowRect(m_hFrameWnd, &m_windowRect);
		GetClientRect(m_hFrameWnd, &m_clientRect);

		if (m_isWindowed)
		{
			// Store windowed resolution
			m_windowedResolution.cx = m_clientRect.right - m_clientRect.left;
			m_windowedResolution.cy = m_clientRect.bottom - m_clientRect.top;

			width = m_windowedResolution.cx;
			height = m_windowedResolution.cy;
		}
		else
		{
			// Store fullscreen resolution
			m_fullscreenResolution.cx = m_clientRect.right - m_clientRect.left;
			m_fullscreenResolution.cy = m_clientRect.bottom - m_clientRect.top;

			width = m_fullscreenResolution.cx;
			height = m_fullscreenResolution.cy;
		}

		// Calculate aspect ratio
		REAL scale = (REAL)width / height;

		// Store new window settings
		m_xScreen = width;
		m_yScreen = height;

		// Calculate UI scale
		m_UIHeight = 768.0f;
		m_UIWidth = 768.0f * scale;

		// Set display resolution
		returnValue = (RuSetResolution(m_pRuSwapChain, (UINT32)width, (UINT32)height, !m_isWindowed) != 0);

		// Set camera FOV and backdrop distance
		if (m_pCamera)
		{
			m_pCamera->SetPerspectiveProjectionFOV(m_cameraNearPlane, m_cameraFarPlane, m_cameraFOV, (float)width / height);
			m_pCamera->SetBackdropDistance(m_cameraBackdropNearPlane, m_cameraBackdropFarPlane);
		}

		// Set UI scale for interface and renderer
		if (g_ruUIRenderer)
			g_ruUIRenderer->ViewportScale_Push(m_UIWidth, m_UIHeight);

		if (m_interface)
			m_interface->SetScreenSize(m_UIWidth, m_UIHeight);
	}

	m_isAdjustWindow = false;

	return returnValue;
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetResourcePath(const char* pathStr)
{
	memset(m_resourceRootDirectoryStr, 0, sizeof(m_resourceRootDirectoryStr));
	if (pathStr && strlen(pathStr) > 0)
	{
		_tcscpy(m_resourceRootDirectoryStr, pathStr);
		int len = (int)strlen(m_resourceRootDirectoryStr);
		if (m_resourceRootDirectoryStr[len - 1] == '\\')
			m_resourceRootDirectoryStr[len - 1] = 0;
	}

	if (m_interface)
		m_interface->SetResourcePath(m_resourceRootDirectoryStr);

	if (g_ruResourceManager)
		g_ruResourceManager->SetRootDirectory(m_resourceRootDirectoryStr);
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateSubSystemResourcePaths()
{
	if (m_interface)
		m_interface->SetResourcePath(m_resourceRootDirectoryStr);

	if (g_ruResourceManager)
		g_ruResourceManager->SetRootDirectory(m_resourceRootDirectoryStr);
}

// --------------------------------------------------------------------------------------
const char* CRuneDev::GetFullFilePath(const char* fileStr)
{
	static int count = 0;
	static char tmpStr[5][MAX_PATH];

	count++;
	if (count >= 5)
		count = 0;

	if (m_resourceRootDirectoryStr[0] == 0)
		strcpy(tmpStr[count], fileStr);
	else
		sprintf(tmpStr[count], "%s\\%s", m_resourceRootDirectoryStr, fileStr);

	return tmpStr[count];
}

// --------------------------------------------------------------------------------------
bool CRuneDev::AdjustWindowForChange()
{
	if (m_isWindowed) {
		SetWindowLong(m_hFrameWnd, GWL_STYLE, m_windowStyle);
	}
	else {
		SetWindowLong(m_hFrameWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
	}
	return TRUE;
}

// ------------------------------------------------------------------------------------------------
int CRuneDev::Run()
{
	MSG Msg;

	PeekMessage(&Msg, NULL, 0U, 0U, PM_NOREMOVE);
	while (Msg.message != WM_QUIT)
	{
		BOOL boMessage = PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE | PM_NOYIELD);
		if (boMessage)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			if (m_isExit)
				SendMessage(m_hFrameWnd, WM_CLOSE, 0, 0);
			else if (true)
				FrameLoop();
			else
			{
				WaitMessage();
			}
		}
	}

	return (int)Msg.wParam;
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetFogDistance(float fmin, float fmax)
{
	m_fogMin = fmin;
	m_fogMax = fmax;

	g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_START, RuCastFloatToINT64(m_fogMin));
	g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_END, RuCastFloatToINT64(m_fogMax));

}

// --------------------------------------------------------------------------------------
void CRuneDev::DefaultCamera()
{
	m_pCamera->SetCameraPosition(CRuVector3(0.0f, 0.0f, 100.0f));		// 鏡頭所在位置
	m_pCamera->SetTargetPosition(CRuVector3(0.0f, 0.0f, 0.0f));		// 鏡頭目的位置
	m_pCamera->SetCameraUpVector(CRuVector3(0.0f, 1.0f, 0.0f));		// 鏡頭上方所朝方向
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCameraFarPlane(float farPlane)
{
	if (m_pCamera)
	{
		m_cameraFarPlane = farPlane;
		m_pCamera->SetPerspectiveProjectionFOV(m_cameraNearPlane, m_cameraFarPlane, m_cameraFOV, (float)m_xScreen / m_yScreen);
		m_pCamera->SetBackdropDistance(m_cameraBackdropNearPlane, m_cameraBackdropFarPlane);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCameraMoveTo(CRuVector3 targetPosition, float moveTime)
{
	if (m_pCamera)
	{
		if (moveTime < 0.000001f) {
			m_pCamera->SetCameraPosition(targetPosition);
		}
		else {
			m_cameraMoveVector = (targetPosition - m_pCamera->GetCameraPosition()) / moveTime;
			m_cameraMoveTime = moveTime;
		}
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCameraPos(CRuVector3& pos)
{
	if (m_pCamera)
		m_pCamera->SetCameraPosition(pos);
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCameraTargetPos(CRuVector3& pos)
{
	if (m_pCamera)
		m_pCamera->SetTargetPosition(pos);
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCameraUpVector(CRuVector3& upVector)
{
	if (m_pCamera)
		m_pCamera->SetCameraUpVector(upVector);
}

// --------------------------------------------------------------------------------------
void CRuneDev::MoveCameraPos(CRuVector3& pos)
{
	if (m_pCamera)
		m_pCamera->SetCameraPosition(pos + m_pCamera->GetCameraPosition());
}

// --------------------------------------------------------------------------------------
void CRuneDev::ZoomCamera(REAL zoomDelta)
{
	if (m_pCamera)
	{
		float distance;
		distance = (m_pCamera->GetTargetPosition() - m_pCamera->GetCameraPosition()).Magnitude();

		/*
		m_cameraDistance -= zoomDelta;
		if ( m_cameraDistance > MAX_CAMERA_DISTANCE )
			m_cameraDistance = MAX_CAMERA_DISTANCE;
		if ( m_cameraDistance < MIN_CAMERA_DISTANCE )
			m_cameraDistance = MIN_CAMERA_DISTANCE;
		*/

		if (m_cameraZoomLocked)
		{
			if (distance - zoomDelta > MAX_CAMERA_DISTANCE)
				zoomDelta -= MAX_CAMERA_DISTANCE - (distance - zoomDelta);
			//		if ( distance - zoomDelta < MIN_CAMERA_DISTANCE )
			//			zoomDelta -= MIN_CAMERA_DISTANCE - (distance - zoomDelta);

			if (distance - zoomDelta < 1.0f)
			{
				zoomDelta -= 1.0f - (distance - zoomDelta);
			}
		}

		// Apply camera movement
		m_pCamera->MoveCamera(CRuVector3(0.0f, 0.0f, zoomDelta), true);
		m_pRuCameraController->UpdatePresetCameraConfiguration();

		float distance2;
		distance2 = (m_pCamera->GetTargetPosition() - m_pCamera->GetCameraPosition()).Magnitude();

		if (abs(distance2 - distance) > 1.0f)
			m_cameraDistance = distance2;

		// Update first person camera mode
		if (m_cameraDistance - zoomDelta < MIN_CAMERA_DISTANCE)
		{
			m_isFirstPersonCamera = TRUE;
		}
		else
		{
			m_isFirstPersonCamera = FALSE;
		}
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCursorIcon(const char* fileName)
{
	if (m_pCursorIcon == NULL)
		m_pCursorIcon = ruNEW CTextureImage;
	m_pCursorIcon->SetFileName(fileName);

	IRuTexture* newIcon = g_ruResourceManager->LoadTexture(fileName);

	if (newIcon)
	{
		// Only set the cursor icon if it is not the same as the current one, or if its iteration has changed
		if (newIcon != m_pCursorIcon_HW || newIcon->GetIteration() != m_pCursorIcon_HW_Iteration)
		{
			// Store cursor icon texture & iteration
			ruSAFE_RELEASE(m_pCursorIcon_HW);
			m_pCursorIcon_HW = newIcon;
			m_pCursorIcon_HW_Iteration = m_pCursorIcon_HW->GetIteration();

			// Copy into hardware cursor
			IRuSurface* srcSurface = RuTextureUtility_GetLevelSurfaceBySize(m_pCursorIcon_HW, 32, 32);
			IRuSurface* dstSurface = RuTextureUtility_GetLevelSurfaceBySize(m_pCursor_HW, 32, 32);

			if (srcSurface && dstSurface)
			{
				RuTextureUtility_Blit(srcSurface, dstSurface, ruALPHAMODE_DISABLE);

				void* bits;
				INT32 pitch;
				m_pCursor_HW->Lock(0, &bits, &pitch);
				m_pCursor_HW->Unlock(0);
			}
		}
		else
		{
			ruSAFE_RELEASE(newIcon);
		}
	}
	else
	{
		// Reset cursor icon texture & iteration
		ruSAFE_RELEASE(m_pCursorIcon_HW);
		m_pCursorIcon_HW_Iteration = 0;
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetDragCursorIcon(const char* fileName)
{
	if (m_pDragCursorIcon == NULL)
		m_pDragCursorIcon = ruNEW CTextureImage;
	m_pDragCursorIcon->SetFileName(fileName);

	IRuTexture* newIcon = g_ruResourceManager->LoadTexture(fileName);

	if (newIcon)
	{
		// Only set the drag cursor icon if it is not the same as the current one, or if its iteration has changed
		if (newIcon != m_pDragCursorIcon_HW || newIcon->GetIteration() != m_pDragCursorIcon_HW_Iteration)
		{
			// Store drag cursor icon texture & iteration
			ruSAFE_RELEASE(m_pDragCursorIcon_HW);
			m_pDragCursorIcon_HW = newIcon;
			m_pDragCursorIcon_HW_Iteration = m_pDragCursorIcon_HW->GetIteration();

			// Copy into hardware cursor
			IRuSurface* srcSurface = RuTextureUtility_GetLevelSurfaceBySize(m_pDragCursorIcon_HW, 32, 32);
			IRuSurface* dstSurface = RuTextureUtility_GetLevelSurfaceBySize(m_pCursor_HW, 32, 32);

			if (srcSurface && dstSurface)
			{
				RuTextureUtility_Blit(srcSurface, dstSurface, ruALPHAMODE_NORMAL);

				void* bits;
				INT32 pitch;
				m_pCursor_HW->Lock(0, &bits, &pitch);
				m_pCursor_HW->Unlock(0);
			}
		}
		else
		{
			ruSAFE_RELEASE(newIcon);
		}
	}
	else
	{
		// Reset drag cursor icon texture & iteration
		ruSAFE_RELEASE(m_pDragCursorIcon_HW);
		m_pDragCursorIcon_HW_Iteration = 0;
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ClearCursorIcon()
{
	SAFE_DELETE(m_pCursorIcon);

	// Reset cursor icon texture & iteration
	ruSAFE_RELEASE(m_pCursorIcon_HW);
	m_pCursorIcon_HW_Iteration = 0;
}

// --------------------------------------------------------------------------------------
void CRuneDev::ClearDragCursorIcon()
{
	SAFE_DELETE(m_pDragCursorIcon);

	// Reset drag cursor icon texture & iteration
	ruSAFE_RELEASE(m_pDragCursorIcon_HW);
	m_pDragCursorIcon_HW_Iteration = 0;
}

// --------------------------------------------------------------------------------------
void CRuneDev::RenderCursor()
{
	g_ruEngine->SetCursor(0, 0, m_pCursor_HW);
	g_ruEngine->ShowCursor(m_isCursorVisible);
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetCursorPosition(int x, int y)
{
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	ClientToScreen(m_hFrameWnd, &pt);
	m_xCursor = x;
	m_yCursor = y;
	g_ruEngine->SetCursorPosition(m_xCursor + pt.x, m_yCursor + pt.y);
}

// --------------------------------------------------------------------------------------
void CRuneDev::ReleaseTerrain()
{
	m_terrainFileName.clear();
	ruSAFE_RELEASE(m_pRuTerrain);
}

// --------------------------------------------------------------------------------------
void CRuneDev::ReleaseOldTerrain()
{
	m_oldTerrainFileName.clear();
	ruSAFE_RELEASE(m_pOldRuTerrain);
}

// --------------------------------------------------------------------------------------
bool CRuneDev::LoadTerrain(const char* resourceName)
{
	bool loadTerrain = true;

	if (resourceName == NULL)
		return false;

	// 清除所有相關Sprite物件,必免更換地圖時有多餘物件未刪除
	{
		IBaseSprite::ReleaseAll();
		// ruSAFE_RELEASE( m_pRuHighlighter );
		m_spriteContainer.clear();
		m_DBIDMap.clear();
		m_worldGUIDMap.clear();

	}

	// 記錄最後載入時間
	m_lastLoadTerrainTime = m_frameTime;

	// 釋放環境資料
	ReleaseEnvironment();

	// 檢查地表是否已經存在暫存內
	if (strcmp(resourceName, m_oldTerrainFileName.c_str()) == 0)
	{
		// Swap terrain data
		CRuOptimalTerrain* tempTerrain = m_pRuTerrain;
		string tempFileName = m_terrainFileName;

		m_pRuTerrain = m_pOldRuTerrain;
		m_terrainFileName = m_oldTerrainFileName;
		m_pOldRuTerrain = tempTerrain;
		m_oldTerrainFileName = tempFileName;

		loadTerrain = false;
	}
	// 不相同將上次地圖資料移置貝份
	else if (m_pRuTerrain != NULL && m_strLoginWdbFileName != m_terrainFileName.c_str())
	{
		// 釋放舊有地圖資料
		ReleaseOldTerrain();

		m_oldTerrainFileName = m_terrainFileName;
		m_pOldRuTerrain = m_pRuTerrain;

		m_pRuTerrain = NULL;
		m_terrainFileName.clear();
	}
	else {
		// 釋放目前地圖資料
		ReleaseTerrain();

		// 釋放舊有地圖資料
		ReleaseOldTerrain();
	}

	// ReleaseTerrain();
	ruSAFE_RELEASE(m_pRuMinimap);

	if (loadTerrain)
	{
		// 加速地表讀取
		g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 200.0f);
		g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 200.0f);

		// Give global worker thread maximum allocation
		g_ruGlobalWorkerThread->SetSleepPeriod(0);
		g_ruResourceManager->SetObjectDequeueRate(200.0f);
		g_ruResourceManager->SetTextureDequeueRate(200.0f);

		// 讀取地表資料來源
#ifdef KALYDO
		// For kalydo, the big WDB's (e.g. world WDB's) are downloaded in parts.
		// we need to handle these files differently.
		CRuDataStore_Disk* mapFileDataStore;
		if (strstr(resourceName, "world") != NULL)
			// multi-file
			mapFileDataStore = ruNEW CRuDataStore_MultiFile();
		else
			mapFileDataStore = ruNEW CRuDataStore_Disk();
#else
		CRuDataStore_Disk* mapFileDataStore = ruNEW CRuDataStore_Disk();
#endif // KALYDO
		if (mapFileDataStore->Open(GetFullFilePath(resourceName), TRUE))
		{
			CRuOptimalTerrain_DataSource_DS* terrainDS = ruNEW CRuOptimalTerrain_DataSource_DS();

			if (terrainDS->Open(mapFileDataStore))
			{
				// 建立地表管理物件
				m_pRuTerrain = ruNEW CRuOptimalTerrain();
				m_pRuTerrain->Initialize();
				m_pRuTerrain->AttachDataSource(terrainDS);
				m_pRuTerrain->GetObjectContainer()->EnableDeferredLoading(TRUE);
				m_pRuTerrain->GetDoodadContainer()->EnableDeferredLoading(TRUE);

				m_terrainFileName = resourceName;
			}
			ruSAFE_RELEASE(terrainDS);
		}
		ruSAFE_RELEASE(mapFileDataStore);
	}

	// 地表資訊重新設定
	if (m_pRuTerrain != NULL)
	{
		// 設定鏡頭控制器所使用的碰撞資料
		m_pRuCameraController->SetCollisionObject(IBaseSprite::GetCollisionContainer());

		// Set default event filter
		UpdateSpecialEvent();

		/*
		// Set default event filter
		CRuWorld_ObjectManager *objectManager = m_pRuTerrain->GetObjectManager();
		if(objectManager)
		{
			objectManager->SetSpecialEventFilter(m_specialEventFilter);
			//objectManager->SetSpecialEventFilter("turkey");
			//objectManager->SetSpecialEventFilter("SpringRain Festival");
			//objectManager->SetSpecialEventFilter("Music Festival");
		}
		*/

		// Create new minimap manager
		m_pRuMinimap = ruNEW CRuFusion_Minimap();
		m_pRuMinimap->SetWorldProperties(resourceName, m_pRuTerrain->GetTerrainBounds());

		if (m_minimapMaskFile.size() != 0)
		{
			IRuTexture* maskTexture = g_ruResourceManager->LoadTexture(m_minimapMaskFile.c_str());
			if (maskTexture)
			{
				m_pRuMinimap->SetMinimapMask(maskTexture);
				ruSAFE_RELEASE(maskTexture);
			}
		}

		// 路徑資料
		// Get path hierarchy from terrain object
		m_pRuPathHierarchy = m_pRuTerrain->GetPathHierarchy();

		// Set path finder for the path hierarchy
		m_pRuPathHierarchy->SetPathFinder(m_pRuPathFinder);

		m_pathPoints.Clear();
	}

	/*
	// 加速地表讀取
	g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 200.0f);
	g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 200.0f);

	// Give global worker thread maximum allocation
	g_ruGlobalWorkerThread->SetSleepPeriod(0);
	g_ruResourceManager->SetObjectDequeueRate(200.0f);
	g_ruResourceManager->SetTextureDequeueRate(200.0f);

	// 讀取地表資料來源
	CRuDataStore_Disk *mapFileDataStore = ruNEW CRuDataStore_Disk();
	if(mapFileDataStore->Open(GetFullFilePath(resourceName), TRUE))
	{
		CRuOptimalTerrain_DataSource_DS *terrainDS = ruNEW CRuOptimalTerrain_DataSource_DS();

		if( terrainDS->Open(mapFileDataStore) )
		{
			// 建立地表管理物件
			m_pRuTerrain = ruNEW CRuOptimalTerrain();
			m_pRuTerrain->Initialize();
			m_pRuTerrain->AttachDataSource(terrainDS);
			m_pRuTerrain->GetObjectContainer()->EnableDeferredLoading(TRUE);
			m_pRuTerrain->GetDoodadContainer()->EnableDeferredLoading(TRUE);

			// 設定鏡頭控制器所使用的碰撞資料
			m_pRuCameraController->SetCollisionObject(IBaseSprite::GetCollisionContainer());

			// Create new minimap manager
			m_pRuMinimap = ruNEW CRuFusion_Minimap();
			m_pRuMinimap->SetWorldProperties(resourceName, m_pRuTerrain->GetTerrainBounds());

			if( m_minimapMaskFile.size() != 0 )
			{
				IRuTexture *maskTexture = g_ruResourceManager->LoadTexture( m_minimapMaskFile.c_str() );
				if(maskTexture)
				{
					m_pRuMinimap->SetMinimapMask(maskTexture);
					ruSAFE_RELEASE(maskTexture);
				}
			}
			m_oldTerrainFileName = resourceName;
		}
		ruSAFE_RELEASE(terrainDS);
	}
	ruSAFE_RELEASE(mapFileDataStore);
	*/

	// 設定天空
	InitialEnvironment();

	// 設定地表資訊
	IBaseSprite::SetRuTerrain(m_pRuTerrain);

	// 地表載入中
	m_loadingTerrain = true;

	return (m_pRuTerrain) ? true : false;
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateTerrain()
{
	if (m_pRuTerrain)
	{
		assert(m_pCamera);

		// 設定地表的鏡頭
		m_pRuTerrain->SetCamera(m_pCamera);

		// 更新地表
		m_pRuTerrain->Update(m_elapsedTime);

		if (m_loadingTerrain && m_pRuTerrain->GetLoadingStatus() >= 1.0f)
		{
			m_loadingTerrain = false;

			g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 8.0f);
			g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 16.0f);

			// Restore global worker thread default allocation
			g_ruGlobalWorkerThread->SetSleepPeriod(25);
			g_ruResourceManager->SetObjectDequeueRate(5.0f);
			g_ruResourceManager->SetTextureDequeueRate(25.0f);
		}
		/*
				if ( !m_loadingTerrain && m_pRuTerrain->GetChunkLoadingStatus() < 0.5f )
				{
					m_loadingTerrain = true;

					// 加速地表讀取
					g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 100.0f);
					g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 100.0f);

					// Give global worker thread maximum allocation
					g_ruGlobalWorkerThread->SetSleepPeriod(0);
					g_ruResourceManager->SetObjectDequeueRate(100.0f);
					g_ruResourceManager->SetTextureDequeueRate(100.0f);
				}
		*/
	}

	if (m_targetPolyLineEntity)
	{
		m_targetPolyLineEntity->Update(m_elapsedTime);
	}
	if (m_targetEntity)
	{
		m_targetEntity->Update(m_elapsedTime);
	}
	if (m_pRuSelectAreaEntity)
	{
		m_pRuSelectAreaEntity->Update(m_elapsedTime);
	}
	if (m_pRuSelectAreaStopEntity)
	{
		m_pRuSelectAreaStopEntity->Update(m_elapsedTime);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::RenderTerrain()
{
	if (m_pRuTerrain)
	{
		// 繪製地表
		g_ruDefaultRenderContext->Submit(m_pRuTerrain);
	}
}

// --------------------------------------------------------------------------------------
bool CRuneDev::ChangeScreenPoint(int x, int y, CRuVector3& pos)
{
	if (m_pCamera)
	{
		// Calculate view origin in world space
		CRuVector3 homPt, viewPt, worldPt, pt1, pt2;

		m_pCamera->Coord_ScreenToHomogeneous(x, y, m_xScreen, m_yScreen, homPt);
		m_pCamera->Coord_HomogeneousToView(homPt, viewPt);
		m_pCamera->Coord_ViewToWorld(viewPt, worldPt);

		pt1 = m_pCamera->GetCameraPosition();
		pt2 = pt1 + (worldPt - pt1) * 1000.0f;

		if (IBaseSprite::TerrainTest(pt1, pt2, &pos))
			return true;
	}
	return false;
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetTerrainDetailLevel(int level)
{
	if (m_pRuTerrain)
	{
		m_pRuTerrain->SetTerrainSplatDetailLevel(level);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ReleaseEnvironment()
{
	ruSAFE_RELEASE(m_pRuWorldEnvironment);
}

// --------------------------------------------------------------------------------------
void CRuneDev::InitialEnvironment()
{
	if (m_pRuWorldEnvironment == NULL)
	{
		m_pRuWorldEnvironment = ruNEW CRuEntity_Environment();
		m_pRuWorldEnvironment->Initialize(32, 10, 1.570796f, 300.0f, 1.0f, 2.0f);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateEnvironment(CRuVector3& pos, bool forceUpdate)
{
	if (m_pRuWorldEnvironment)
	{
		m_timeSinceLastWorldEnvironmentUpdate += m_elapsedTime;

		if (m_timeSinceLastWorldEnvironmentUpdate > 1.0f || forceUpdate)
		{
			m_timeSinceLastWorldEnvironmentUpdate = 0.0f;

			// Select proper zone descriptor to use
			if (m_pRuTerrain)
			{
				CRuWorld_ZoneDescriptor* zoneDescriptor = m_pRuTerrain->GetZoneDescriptor(pos);

				if (zoneDescriptor)
				{
					CRuWorld_ZoneSettings* zoneSettings = zoneDescriptor->GetZoneSettings(m_timeGroup, 0);

					if (zoneSettings == NULL)
					{
						CRuWorld_ZoneDescriptor* parentZoneDescriptor = m_pRuTerrain->GetZoneDescriptorByID(zoneDescriptor->GetParentZoneID());

						while (parentZoneDescriptor)
						{
							zoneSettings = parentZoneDescriptor->GetZoneSettings(m_timeGroup, 0);

							if (zoneSettings)
								break;

							parentZoneDescriptor = m_pRuTerrain->GetZoneDescriptorByID(parentZoneDescriptor->GetParentZoneID());
						}
					}

					// Select zone descriptor with time and weather settings
					static bool enable = false;
					static CRuWorld_ZoneSettings newZoneSettings;
					static DWORD color[5] = { 0, 0, 0, 0, 0 };

					if (m_forceMainLightColor > 0 && zoneSettings) {
						//if ( enable && zoneSettings ) {						
						newZoneSettings.CopySettings(*zoneSettings);
						newZoneSettings.m_color_Primary = m_forceMainLightColor;
						newZoneSettings.m_color_Secondary = m_forceMainLightColor;
						/*
						newZoneSettings.m_color_Primary		= color[0];
						newZoneSettings.m_color_Secondary	= color[1];
						newZoneSettings.m_color_Specular	= color[2];
						newZoneSettings.m_color_Ambient		= color[3];
						newZoneSettings.m_color_Fog			= color[4];
						*/
						m_pRuWorldEnvironment->SelectZoneDescriptor(zoneDescriptor, &newZoneSettings);
					}
					else {
						m_pRuWorldEnvironment->SelectZoneDescriptor(zoneDescriptor, zoneSettings);
					}

					// Set currently active zone descriptor into the event manager (will this become a logistical nightmare?)
					g_pRuGlobalEventManager->SetZoneSettings(zoneSettings);


				}

				// Set the active zone type (Outdoor, indoor, etc)
				m_activeZoneType = m_pRuTerrain->GetZoneType(pos);

				// Set current time group into the event manager
				g_pRuGlobalEventManager->SetTimeGroup(m_timeGroup);

				// Get overriding music trigger
				CRuWorld_Trigger* musicTrigger = m_pRuTerrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_MUSIC, pos, TRUE);
				g_pRuGlobalEventManager->SetMusicTrigger(musicTrigger);

				// Get overriding ambience trigger
				CRuWorld_Trigger* ambienceTrigger = m_pRuTerrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_AMBIENCE, pos, TRUE);
				g_pRuGlobalEventManager->SetAmbienceTrigger(ambienceTrigger);

				// Get overriding supplement ambience trigger
				CRuWorld_Trigger* supplementAmbienceTrigger = m_pRuTerrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_SUPPLEMENTAMBIENCE, pos, TRUE);
				g_pRuGlobalEventManager->SetSupplementAmbienceTrigger(supplementAmbienceTrigger);

				// Get overriding random ambience trigger
				CRuWorld_Trigger* randomAmbienceTrigger = m_pRuTerrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_RANDOMAMBIENCE, pos, TRUE);
				g_pRuGlobalEventManager->SetRandomAmbienceTrigger(randomAmbienceTrigger);

				// Enumerate 3D ambience triggers 
				g_pRuGlobalEventManager->Clear3DAmbienceTriggers();
				m_pRuTerrain->EnumerateTriggers(RuWorld::ruTRIGGERTYPE_3DAMBIENCE, g_pRuGlobalEventManager->Get3DAmbienceTriggers());
				g_pRuGlobalEventManager->AddRef3DAmbienceTriggers();
			}
		}

		// Update world environment and render sky dome
		m_pRuWorldEnvironment->SetCamera(m_pCamera);
		m_pRuWorldEnvironment->Update(m_elapsedTime);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::RenderEnvironment()
{
	// 繪製天空
	g_ruDefaultRenderContext->Submit(m_pRuWorldEnvironment);
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateInterface()
{
	// 重新取得輸入裝置資訊
	InputState inputState;
	memset(&inputState, 0, sizeof(inputState));

	// 視窗模式,按鍵超出畫面不接收
	if (m_isWindowed) {
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_hFrameWnd, &pt);

		if (pt.x < 0)
			m_xCursor = 0;
		else if (pt.x > (long)m_xScreen)
			m_xCursor = m_xScreen;

		if (pt.y < 0)
			m_yCursor = 0;
		else if (pt.y > (long)m_yScreen)
			m_yCursor = m_yScreen;
	}

	// 更新滑鼠座標
	inputState.xPos = (int)(m_xCursor * m_UIWidth / m_xScreen);
	inputState.yPos = (int)(m_yCursor * m_UIHeight / m_yScreen);

#ifdef KALYDO
	// only handle key and mouse if we have focus.
	if (m_isActived)
#endif // KALYDO
	{
		// 取得鍵盤資訊
		if (m_pKeyboard->Update())
		{
			for (int i = 0; i < 256; i++)
				inputState.key[i] = m_pKeyboard->GetKeyState(i);
		}

		// 取得滑鼠資訊
		if (m_pMouse->Update())
		{
			for (int i = 0; i < 4; i++)
				inputState.button[i] = m_pMouse->GetButtonState(i);

			inputState.xDelta = m_pMouse->GetXDelta() * m_UIWidth / m_xScreen;
			inputState.yDelta = m_pMouse->GetYDelta() * m_UIHeight / m_yScreen;
			inputState.zDelta = (float)m_pMouse->GetZDelta();
		}
	}

	// 鎖定游標座標值
	/*
	if ( m_isCursorLock )
		g_ruEngine->SetCursorPosition(m_xCursor, m_yCursor);
	else
	{
		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( m_hFrameWnd, &pt );
		m_xCursor = pt.x;
		m_yCursor = pt.y;
	}
	*/

	/*
	// 更新游標座標值
	if ( m_isCursorLock )
	{
		if ( m_isWindowed )
		{
			POINT pt;
			pt.x = m_xCursor;
			pt.y = m_yCursor;
			ClientToScreen( m_hFrameWnd, &pt );
			SetCursorPos( pt.x, pt.y );
		}
	}
	else
	{
		// 使用視窗座標值
		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( m_hFrameWnd, &pt );

		m_xCursor = pt.x;
		m_yCursor = pt.y;
	}
	*/

	m_interface->Update(m_elapsedTime, inputState);
}

// --------------------------------------------------------------------------------------
CRoleSprite* CRuneDev::CreateSprite(int ID, int DBID, int worldGUID, bool& outNew)
{
	// g_pError->AddMessage(0, 0, "CreateSprite ID[%d], DBID[%d] WORLD[%d]", ID, DBID, worldGUID);

	CRoleSprite* sprite = NULL;
	outNew = false;
	map<int, int>::iterator iter = m_worldGUIDMap.find(worldGUID);

	// 物件已存在
	if (iter != m_worldGUIDMap.end())
	{
		sprite = FindSpriteByID(iter->second);
		if (iter->second != ID)					// ID是否有改變
		{
			// 改變索引編號
			m_spriteContainer.erase(iter->second);
			m_spriteContainer.insert(make_pair(ID, sprite));
			iter->second = ID;

			// 通知某玩家編號改變
			if (sprite)
			{
				SendPlayerGItemIDChange(sprite, ID);
			}
		}

		if (DBID > 0)
		{
			// 改變 DBID Map
			map<int, int>::iterator DBIDiter = m_DBIDMap.find(DBID);
			if (DBIDiter != m_DBIDMap.end())
			{
				DBIDiter->second = ID;
			}
		}
	}
	else
	{
		outNew = true;
		sprite = new CRoleSprite;
		sprite->SetContainerState(true);
		m_spriteContainer.insert(make_pair(ID, sprite));
		m_worldGUIDMap.insert(make_pair(worldGUID, ID));
		if (DBID > 0) m_DBIDMap.insert(make_pair(DBID, ID));
	}

	if (sprite)
	{
		sprite->AddRefCount();
		sprite->SetIdentify(ID);
		sprite->SetDBID(DBID);
		sprite->SetWorldGUID(worldGUID);

		if (sprite->RefCount() > 0)
			sprite->ClearDestruct();

		if (outNew)
		{
			SendCreateRoleSprite(sprite);

			if (m_paperdollCache_ActivateWrites)
			{
				// Add sprite to list of unprocessed sprites
				m_paperdollCache_UnprocessedSprites.Add(sprite);
			}
		}
	}

	return sprite;
}

// --------------------------------------------------------------------------------------
CRoleSprite* CRuneDev::FindSpriteByID(int ID)
{
	map<int, CRoleSprite*>::iterator iter = m_spriteContainer.find(ID);
	if (iter != m_spriteContainer.end())
		return iter->second;
	return NULL;
}

// --------------------------------------------------------------------------------------
CRoleSprite* CRuneDev::FindSpriteByDBID(int DBID)
{
	map<int, int>::iterator iter = m_DBIDMap.find(DBID);
	if (iter != m_DBIDMap.end())
		return FindSpriteByID(iter->second);
	return NULL;
}

// --------------------------------------------------------------------------------------
CRoleSprite* CRuneDev::FindSpriteByWorldGUID(int worldGUID)
{
	map<int, int>::iterator iter = m_worldGUIDMap.find(worldGUID);
	if (iter != m_worldGUIDMap.end())
		return FindSpriteByID(iter->second);
	return NULL;
}

// --------------------------------------------------------------------------------------
bool CRuneDev::DeleteSprite(int worldGUID)
{
	map<int, int>::iterator iter = m_worldGUIDMap.find(worldGUID);
	if (iter != m_worldGUIDMap.end())
	{
		CRoleSprite* sprite = FindSpriteByID(iter->second);
		if (sprite)
		{
			sprite->DelRefCount();

			m_deleteRoleSpriteCount++;

			return true;
		}
	}
	return false;
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateSprite()
{
	if (m_paperdollCache_ActivateWrites)
	{
		for (INT32 i = 0; i < m_paperdollCache_UnprocessedSprites.Count(); ++i)
		{
			BOOL removeSpriteFromList = TRUE;
			CRoleSprite* sprite = m_paperdollCache_UnprocessedSprites[i];

			if (sprite->IsDestruct() == FALSE)
			{
				switch (sprite->GetObjectType())
				{
				case eSpriteObject_Unknown:
					continue;

				case eSpriteObject_NPC:
				case eSpriteObject_QuestNPC:
				{
					CRuEntity* entity = sprite->GetRuEntity();
					if (entity)
					{
						IRuPaperdoll* paperdoll = RuEntity_FindFirstPaperdoll(entity);
						if (paperdoll)
						{
							if (paperdoll->GetType().IsSubClassOf(CRuPaperdoll::Type()))
							{
								if (paperdoll->IsPaperdollBuilding() == FALSE && static_cast<CRuPaperdoll*>(paperdoll)->IsPaperdollInValidState())
								{
									// Store paperdol clone into cache
									m_paperdollCache->CachePaperdoll(entity->GetResourceName(), static_cast<CRuPaperdoll*>(paperdoll));
								}
								else
								{
									// Paperdoll is still building, wait for completion
									removeSpriteFromList = FALSE;
								}
							}
							else
							{
								// In-memory paperdoll is just a proxy, wait for load to complete
								removeSpriteFromList = FALSE;
							}
						}
					}
				}

				break;
				}
			}

			if (removeSpriteFromList)
			{
				m_paperdollCache_UnprocessedSprites.RemoveAt(i);
				--i;
			}
		}
	}

	IBaseSprite::UpdateAll(m_elapsedTime);
}

// --------------------------------------------------------------------------------------
void CRuneDev::RenderSprite()
{
	IBaseSprite::RenderAll();
}

// --------------------------------------------------------------------------------------
void CRuneDev::ForceDeleteSprite(CRoleSprite* sprite)
{
	if (sprite)
	{
		sprite->DelRefCount();
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ProtectSprite(CRoleSprite* sprite)
{
	if (sprite)
	{
		sprite->AddRefCount();
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SpriteMsgProc(IBaseSprite* sprite, SpriteMessageType msg, INT64 arg1, INT64 arg2)
{
	switch (msg)
	{
	case eSpriteMsg_Create:
		break;

	case eSpriteMsg_Delete:
		// 不為CLIENT物件
		if (((CRoleSprite*)sprite)->GetContainerState() == true)
		{
			map<int, CRoleSprite*>::iterator iter = m_spriteContainer.find(sprite->GetIdentify());
			if (iter != m_spriteContainer.end())
			{
				CRoleSprite* tempSprite = iter->second;
				SendDeleteRoleSprite(tempSprite);

				// 移除索引結構
				m_spriteContainer.erase(iter);
				m_worldGUIDMap.erase(tempSprite->GetWorldGUID());
				m_DBIDMap.erase(tempSprite->GetDBID());
			}
		}
		break;
	}
}

// --------------------------------------------------------------------------------------
CRoleSprite* CRuneDev::GetTopRoleSprite(int x, int y, CRuArrayList<CRoleSprite*>& ignoreList)
{
	multimap<float, CRoleSprite*> depth_iter;
	for (map<int, CRoleSprite*>::iterator iter = m_spriteContainer.begin(); iter != m_spriteContainer.end(); iter++)
	{
		CRoleSprite* sprite = iter->second;
		if (sprite)
		{


			CRuVector3 position;

			// 非玩家物件,乎略死掉物件
			if (sprite->GetDeadState() == true && sprite->GetCorpseState() == false && sprite->GetObjectType() != eSpriteObject_Player)
				continue;

			if (sprite->GetCursorState() == false)
				continue;

			if (GetKeyState(VK_SHIFT) < 0 && sprite->GetObjectType() == eSpriteObject_Player)
				continue;

			// 略判斷元件
			bool boContinue = false;
			for (INT32 i = 0; i < ignoreList.Count(); ++i)
			{
				if (ignoreList[i] == sprite)
				{
					boContinue = true;
					break;
				}
			}

			if (boContinue)
				continue;

			position = sprite->GetPos();
			m_pCamera->GetCameraTransform().TransformPoint(position);
			if (position.m_z > -10)
			{
				// 死亡加權值(愈遠)
				if (sprite->GetDeadState() == true && sprite->GetCorpseState() == false)
					position.m_z += 4;

				// 自己墓碑增加權值(愈近)
				if (sprite == m_pPlayer && sprite->GetCheckedStatus() == eChecked_Tomb)
					position.m_z -= 4;

				depth_iter.insert(make_pair(position.m_z, sprite));
			}
		}
	}

	// Do fine-grained picking
	for (multimap<float, CRoleSprite*>::iterator iter = depth_iter.begin(); iter != depth_iter.end(); iter++)
	{
		if (RuEntity_PickTest((iter->second)->GetCurrentEntity(), m_pCamera, m_pRuSwapChain, x, y, FALSE))
			return iter->second;
	}

	// Do rough picking
	for (multimap<float, CRoleSprite*>::iterator iter = depth_iter.begin(); iter != depth_iter.end(); iter++)
	{
		if (RuEntity_PickTest((iter->second)->GetCurrentEntity(), m_pCamera, m_pRuSwapChain, x, y, TRUE))
			return iter->second;
	}

	return NULL;
}

// --------------------------------------------------------------------------------------
void CRuneDev::SetHighlightSprite(CRoleSprite* sprite)
{
	if (m_pRuHighlighter)
	{
		CRuEntity* ruEntity = NULL;
		if (sprite)
		{
			ruEntity = sprite->GetRuTempEntity();
			if (ruEntity == NULL)
				ruEntity = sprite->GetRuEntity();
		}

		m_pRuHighlighter->SetTarget(ruEntity);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ResetRoleSpriteWidget()
{
	for (map<int, CRoleSprite*>::iterator iter = m_spriteContainer.begin(); iter != m_spriteContainer.end(); iter++)
		(iter->second)->SetWidgetTexture();
}

// --------------------------------------------------------------------------------------
void CRuneDev::SprEdit_Begin()
{
	if (m_editingModeOn == false)
	{
		m_editingModeOn = true;
		m_editingDragOn = false;
		m_editingSprite = NULL;
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SprEdit_End()
{
	if (m_editingModeOn == true)
	{
		m_editingModeOn = false;
		m_editingDragOn = false;
		m_editingSprite = NULL;
	}
}

// --------------------------------------------------------------------------------------
BOOL CRuneDev::SprEdit_PickAtCursor(INT32 x, INT32 y, IBaseSprite** spriteOut, REAL* distanceOut)
{
	REAL nearestPickDist = FLT_MAX;
	CRoleSprite* nearestSprite = NULL;

	// Perform precise sprite picking
	for (map<int, CRoleSprite*>::iterator iter = m_spriteContainer.begin(); iter != m_spriteContainer.end(); iter++)
	{
		if (iter->second->RefCount() > 0)
		{
			CRuEntity* entity = (iter->second)->GetRuEntity();

			if (entity)
			{
				REAL pickDist = FLT_MAX;
				if (RuEntity_PickTest(entity, this->GetCamera(), this->GetRuSwapChain(), x, y, FALSE, &pickDist))
				{
					if (pickDist < nearestPickDist)
					{
						nearestPickDist = pickDist;
						nearestSprite = iter->second;

						// Save current result
						*spriteOut = nearestSprite;
						*distanceOut = nearestPickDist;
					}
				}
			}
		}
	}

	// If no sprite was picked, perform non-precise sprite picking
	if (nearestPickDist == FLT_MAX)
	{
		for (map<int, CRoleSprite*>::iterator iter = m_spriteContainer.begin(); iter != m_spriteContainer.end(); iter++)
		{
			if (iter->second->RefCount() > 0)
			{
				CRuEntity* entity = (iter->second)->GetRuEntity();

				if (entity)
				{
					REAL pickDist = FLT_MAX;
					if (RuEntity_PickTest(entity, this->GetCamera(), this->GetRuSwapChain(), x, y, TRUE, &pickDist))
					{
						if (pickDist < nearestPickDist)
						{
							nearestPickDist = pickDist;
							nearestSprite = iter->second;

							// Save current result
							*spriteOut = nearestSprite;
							*distanceOut = nearestPickDist;
						}
					}
				}
			}
		}
	}

	return nearestPickDist < FLT_MAX;
}

// --------------------------------------------------------------------------------------
BOOL CRuneDev::SprEdit_BeginManipulation(IBaseSprite* sprite)
{
	if (sprite)
	{
		return TRUE;
	}

	return FALSE;
}

// --------------------------------------------------------------------------------------
BOOL CRuneDev::SprEdit_EndManipulation(IBaseSprite* sprite)
{
	if (sprite)
	{
		return TRUE;
	}

	return FALSE;
}

// --------------------------------------------------------------------------------------
void CRuneDev::SprEdit_RepositionToCursor(IBaseSprite* sprite, INT32 x, INT32 y)
{
	CRuCamera* camera = this->GetCamera();

	// Get the target position for the pick ray in world coordinates
	CRuVector3 homPt, viewPt, worldPt;
	camera->Coord_ScreenToHomogeneous(x, y, this->GetScreenWidth(), this->GetScreenHeight(), homPt);
	camera->Coord_HomogeneousToView(homPt, viewPt);
	camera->Coord_ViewToWorld(viewPt, worldPt);

	// Build collision segment
	CRuSegment3 colSegment(camera->GetCameraPosition(), camera->GetCameraPosition() + (worldPt - camera->GetCameraPosition()).Normalize() * 1200.0f);

	// Query against terrain for nearest intersection
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;
	segQuery.m_colSegment = colSegment;
	IBaseSprite::GetCollisionContainer()->QueryCollision(&segQuery);

	if (segQuery.m_positiveHit)
	{
		sprite->SetPos(segQuery.m_collisionPoint);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::SprEdit_SnapToNormalAtCursor(IBaseSprite* sprite, INT32 x, INT32 y)
{
	CRuCamera* camera = this->GetCamera();

	// Get the target position for the pick ray in world coordinates
	CRuVector3 homPt, viewPt, worldPt;
	camera->Coord_ScreenToHomogeneous(x, y, this->GetScreenWidth(), this->GetScreenHeight(), homPt);
	camera->Coord_HomogeneousToView(homPt, viewPt);
	camera->Coord_ViewToWorld(viewPt, worldPt);

	// Build collision segment
	CRuSegment3 colSegment(camera->GetCameraPosition(), camera->GetCameraPosition() + (worldPt - camera->GetCameraPosition()).Normalize() * 1200.0f);

	// Query against terrain for nearest intersection
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;
	segQuery.m_colSegment = colSegment;
	IBaseSprite::GetCollisionContainer()->QueryCollision(&segQuery);

	if (segQuery.m_positiveHit)
	{
		// Set orienting axis for the sprite
		sprite->SetOrientingAxis(segQuery.m_collisionNormal, true);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::FlagEditor_ClearAll()
{
	for (INT32 i = 0; i < m_flagEditor_Flags.Count(); ++i)
	{
		ruSAFE_RELEASE(m_flagEditor_Flags[i]);
	}

	m_flagEditor_Flags.Clear();
}

/*
// --------------------------------------------------------------------------------------
void Widget_AddRowText(CRuEntity_Widget_Label *labelWidget, CRuFontType *fontType, const WCHAR* testStr, float scale, int color)
{
	CRuFontTexture* pFontTex = NULL;

	vector<CRuRectangle> stock;
	for ( int i = 0; testStr[i]; i++ )
	{
		CRuFontChar* pChar = CRuFontChar::GetFontChar( fontType, testStr[i] );
		if ( pChar )
		{
			pFontTex = pChar->GetFontTexture();
			if ( pFontTex )
			{
				stock.push_back( pFontTex->GetRectangle() );
			}
		}
	}
	if ( !stock.empty() )
	{
		INT32 row = labelWidget->GetRowCount();
		labelWidget->InsertGlyph(row, stock.size(), &stock[0], scale, color, pFontTex->GetRuTextureD3D());
//		labelWidget->AddRowText( stock.size(), &stock[0], scale, color );
	}
}
*/

// --------------------------------------------------------------------------------------
void Widget_AddRowText(CRuEntity_Widget_Label* labelWidget, CRuFontType* fontType, const WCHAR* testStr, float scale, int color)
{
	vector<CRuRectangle> stock;

	vector<wstring> strList;
	CRuFontString::SplitString(testStr, (int)wcslen(testStr), strList);

	for (vector<wstring>::iterator iter = strList.begin(); iter != strList.end(); iter++)
	{
		CRuFontString* fontString = CRuFontString::GetFontString(fontType, iter->c_str(), (int)iter->length(), TRUE);
		if (fontString)
		{
			CRuFontTexture2* fontTexture = fontString->GetFontTexture();
			if (fontTexture)
			{
				stock.push_back(fontTexture->GetRectangle());
			}
		}
	}

	if (!stock.empty())
	{
		INT32 row = labelWidget->GetRowCount();
		labelWidget->InsertGlyph(row, (INT32)stock.size(), &stock[0], scale, color, CRuFontTexture2::GetRuTextureD3D());
	}
}

// --------------------------------------------------------------------------------------
INT32 CRuneDev::FlagEditor_CreateFlag(const CRuVector3& position, REAL yRotation, INT32 alternateIndex)
{
	// Create new flag entity
	CRuEntity* flagEntity = ruNEW CRuEntity();

	// Flag anchor sub-entity
	CRuEntity* flagAnchor = ruNEW CRuEntity_Widget_Tetrahedron();
	flagEntity->AddChild(flagAnchor);

	// Flag boby sub-entity
	CRuEntity* flagBody = ruNEW CRuEntity_Widget_Tetrahedron();
	flagBody->Scale(2.0f);
	flagBody->Translate(CRuVector3(0.0f, 5.0f, 0.0f));
	flagEntity->AddChild(flagBody);

	CRuEntity* flagArrow = ruNEW CRuEntity_Widget_Tetrahedron();
	flagArrow->Scale(0.5f);
	flagArrow->Translate(CRuVector3(5.0f, 5.0f, 0.0f));
	flagEntity->AddChild(flagArrow);

	// Flag label sub-entity
	CRuEntity_Widget_Label* flagLabel = ruNEW CRuEntity_Widget_Label();
	//	flagLabel->SetTexture(CRuFontTexture::GetRuTextureD3D());

	char buffer[1024];

	if (alternateIndex == 0xFFFFFFFF)
		sprintf(buffer, "%d", m_flagEditor_Flags.Count() + 1);
	else
		sprintf(buffer, "%d", alternateIndex);

	WCHAR* tmp = TransToWChar(buffer);
	Widget_AddRowText(flagLabel, m_pSysFontType, tmp, 20.0f, 0xFFFFFFFF);
	delete[] tmp;

	flagLabel->Translate(CRuVector3(0.0f, 10.0f, 0.0f));
	flagLabel->Update(0.0f);
	flagEntity->AddChild(flagLabel);

	// Release local entity references
	ruSAFE_RELEASE(flagAnchor);
	ruSAFE_RELEASE(flagBody);
	ruSAFE_RELEASE(flagArrow);

	// Orient new flag entity
	yRotation = yRotation * (ruPI / 180.0f);
	CRuQuaternion yQuat;
	yQuat.FromAngleAxis(CRuVector3(0.0f, -1.0f, 0.0f), yRotation);

	flagEntity->ResetAllTransforms();
	flagEntity->Rotate(yQuat);
	flagEntity->Translate(position);

	// Insert flag entity into flag array
	m_flagEditor_Flags.Add(flagEntity);

	// Return index for the newly created flag
	return m_flagEditor_Flags.Count() - 1;
}

// --------------------------------------------------------------------------------------
BOOL CRuneDev::FlagEditor_GetFlagAtCursor(INT32 x, INT32 y, INT32* flagIndexOut, CRuEntity** flagOut, REAL* distanceOut)
{
	REAL nearestPickDist = FLT_MAX;
	CRuEntity* nearestFlag = NULL;

	for (INT32 i = 0; i < m_flagEditor_Flags.Count(); ++i)
	{
		REAL pickDist = FLT_MAX;
		if (RuEntity_PickTest(m_flagEditor_Flags[i], this->GetCamera(), this->GetRuSwapChain(), x, y, FALSE, &pickDist))
		{
			if (pickDist < nearestPickDist)
			{
				nearestPickDist = pickDist;
				nearestFlag = m_flagEditor_Flags[i];

				*flagIndexOut = i;
				*flagOut = nearestFlag;
				*distanceOut = nearestPickDist;
			}
		}
	}

	return nearestPickDist < FLT_MAX;
}

// --------------------------------------------------------------------------------------
CRuEntity* CRuneDev::FlagEditor_GetFlagAtIndex(INT32 index)
{
	if (index >= 0 && index < m_flagEditor_Flags.Count())
	{
		return m_flagEditor_Flags[index];
	}

	return NULL;
}

// --------------------------------------------------------------------------------------
BOOL CRuneDev::FlagEditor_RepositionFlagToCursor(CRuEntity* flag, INT32 x, INT32 y, CRuVector3* outPosition)
{
	CRuCamera* camera = this->GetCamera();

	// Get the target position for the pick ray in world coordinates
	CRuVector3 homPt, viewPt, worldPt;
	camera->Coord_ScreenToHomogeneous(x, y, this->GetScreenWidth(), this->GetScreenHeight(), homPt);
	camera->Coord_HomogeneousToView(homPt, viewPt);
	camera->Coord_ViewToWorld(viewPt, worldPt);

	// Build collision segment
	CRuSegment3 colSegment(camera->GetCameraPosition(), camera->GetCameraPosition() + (worldPt - camera->GetCameraPosition()).Normalize() * 1200.0f);

	// Query against terrain for nearest intersection
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;
	segQuery.m_colSegment = colSegment;
	IBaseSprite::GetCollisionContainer()->QueryCollision(&segQuery);

	if (segQuery.m_positiveHit)
	{
		if (outPosition)
			*outPosition = segQuery.m_collisionPoint;
		flag->SetTranslation(segQuery.m_collisionPoint);
		return TRUE;
	}
	return FALSE;
}

// --------------------------------------------------------------------------------------
void CRuneDev::FlagEditor_RenderFlags()
{
	for (INT32 i = 0; i < m_flagEditor_Flags.Count(); ++i)
	{
		g_ruDefaultRenderContext->Submit(m_flagEditor_Flags[i]);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::FlagEditor_UpdateFlags()
{
	for (INT32 i = 0; i < m_flagEditor_Flags.Count(); ++i)
	{
		m_flagEditor_Flags[i]->Update(m_elapsedTime);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::PaperdollCache_ActivateWrites(BOOL activateWrites)
{
	// Store write activation status
	m_paperdollCache_ActivateWrites = activateWrites;

	// Disable/enable part compositing
	g_ruEngineSettings->SetSetting("composite model:enable part compositing", activateWrites);
}

// --------------------------------------------------------------------------------------
void CRuneDev::CreateWidgetGrid(CRuVector3 pos, bool isLock, float halfSize, float spacing)
{
	ReleaseWidgetGrid();

	m_gridPos = pos;
	m_isGridLock = isLock;
	m_gridHalfSize = halfSize;
	m_gridSpacing = spacing;

	// 產生格線
	m_pRuWidgetGrid = new CRuEntity_Widget_Grid;
	if (m_pRuWidgetGrid)
	{
		m_pRuWidgetGrid->SetGridProperties(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f), m_gridHalfSize, m_gridSpacing);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ReleaseWidgetGrid()
{
	ruSAFE_RELEASE(m_pRuWidgetGrid);
}

// --------------------------------------------------------------------------------------
void CRuneDev::DisplaySelectAreaEntity(float distance, float scale)
{
	ruSAFE_RELEASE(m_pRuSelectAreaEntity);
	ruSAFE_RELEASE(m_pRuSelectAreaStopEntity);

	m_pRuSelectAreaStopEntity = g_ruResourceManager->LoadEntity(SELECT_AREA_OVER_ENTITY);
	if (m_pRuSelectAreaStopEntity)
	{
		m_pRuSelectAreaStopEntity->ResetAllTransforms();
		m_pRuSelectAreaStopEntity->Scale(scale, ruTRANSFORMOP_PRECONCAT);
		RuACT_PlayMotion(m_pRuSelectAreaStopEntity, "idle");
	}

	if ((m_pRuSelectAreaEntity = g_ruResourceManager->LoadEntity(SELECT_AREA_ENTITY)) != NULL)
	{
		m_pRuSelectAreaEntity->ResetAllTransforms();
		m_pRuSelectAreaEntity->Scale(scale, ruTRANSFORMOP_PRECONCAT);
		m_selectAreaDistance = distance;
		RuACT_PlayMotion(m_pRuSelectAreaEntity, "idle");
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ClearSelectAreaEntity()
{
	ruSAFE_RELEASE(m_pRuSelectAreaEntity);
	ruSAFE_RELEASE(m_pRuSelectAreaStopEntity);
}

// --------------------------------------------------------------------------------------
void CRuneDev::DisplayMoveTargetEntity(CRuVector3& position)
{
	if (m_pRuMoveTargetEntity == NULL)
	{
		m_pRuMoveTargetEntity = g_ruResourceManager->LoadEntity(ACT_MOVE_TARGET_PATH);
		if (m_pRuMoveTargetEntity)
			RuACT_PlayMotion(m_pRuMoveTargetEntity, "idle");
	}

	if (m_pRuMoveTargetEntity)
		m_pRuMoveTargetEntity->SetTranslation(position);
}

// --------------------------------------------------------------------------------------
void CRuneDev::ClearMoveTargetEntity()
{
	ruSAFE_RELEASE(m_pRuMoveTargetEntity);
}

// --------------------------------------------------------------------------------------
int CRuneDev::FindPath(CRuVector3 origin, CRuVector3 destination)
{
	CRuVector3 minima, maxima;
	minima.m_x = min(origin.m_x, destination.m_x) - 500.0f;
	minima.m_y = min(origin.m_y, destination.m_y) - 500.0f;
	minima.m_z = min(origin.m_z, destination.m_z) - 500.0f;

	maxima.m_x = max(origin.m_x, destination.m_x) + 500.0f;
	maxima.m_y = max(origin.m_y, destination.m_y) + 500.0f;
	maxima.m_z = max(origin.m_z, destination.m_z) + 500.0f;

	float deltaX = maxima.m_x - minima.m_x;
	float deltaZ = maxima.m_z - minima.m_z;

	if (sqrt(deltaX * deltaX + deltaZ * deltaZ) > 9600)
	{
		return -1;
	}

	CRuAABB bounds(minima, maxima);
	BOOL preLoadOK = m_pRuPathHierarchy->PreLoadPathGraphs(bounds);

	// Load OK?
	if (preLoadOK)
	{
		// Find path from origin to destination
		BOOL pathFound = m_pRuPathHierarchy->FindPath(origin, destination, m_pathPoints);

		// Path found?
		if (pathFound)
		{
			return 0;
		}
	}

	return -2;
}

// --------------------------------------------------------------------------------------
bool CRuneDev::FindHeight(CRuVector3& position)
{

	CRuVector3 minima, maxima;
	minima.m_x = position.m_x - 200.0f;
	minima.m_y = position.m_y - 2000.0f;
	minima.m_z = position.m_z - 200.0f;

	maxima.m_x = position.m_x + 200.0f;
	maxima.m_y = position.m_y + 2000.0f;
	maxima.m_z = position.m_z + 200.0f;

	CRuAABB bounds(minima, maxima);
	BOOL preLoadOK = m_pRuPathHierarchy->PreLoadPathGraphs(bounds);

	// Load OK?
	if (false == preLoadOK)
	{
		return false;
	}

	minima = position;
	minima.m_y -= 2000.0f;

	maxima = position;
	maxima.m_y += 2000.0f;

	CRuSegment3 segment(maxima, minima);
	CRuArrayList<CRuPathNode*> pathNodes;
	m_pRuPathHierarchy->FindIntersectingPathNodes(segment, pathNodes);


	bool finded = false;
	float nearest = 5000.0f;
	CRuVector3 nearestPos;

	float temp;
	CRuVector3 tempPos;
	for (INT32 i = 0; i < pathNodes.Count(); ++i)
	{
		tempPos = pathNodes[i]->Centroid() - position;

		temp = tempPos.Magnitude();


		if (temp < nearest)
		{
			nearest = temp;
			nearestPos = pathNodes[i]->Centroid();
			finded = true;
		}

	}

	// Path found?
	if (false == finded)
	{
		return false;
	}

	position = nearestPos;

	return true;
}

// --------------------------------------------------------------------------------------
void CRuneDev::PushSpecialEvent(const char* festival)
{
	if (festival == NULL)
		return;

	for (vector<string>::iterator iter = m_specialEvent.begin(); iter != m_specialEvent.end(); iter++)
	{
		if (stricmp(iter->c_str(), festival) == 0)
			return;
	}

	m_specialEvent.push_back(festival);
	UpdateSpecialEvent();
}

// --------------------------------------------------------------------------------------
void CRuneDev::RemoveSpecialEvent(const char* festival)
{
	if (festival == NULL)
		return;

	for (vector<string>::iterator iter = m_specialEvent.begin(); iter != m_specialEvent.end(); iter++)
	{
		if (stricmp(iter->c_str(), festival) == 0)
		{
			m_specialEvent.erase(iter);
			UpdateSpecialEvent();
			return;
		}
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::ClearSpecialEvent()
{
	m_specialEvent.clear();
	UpdateSpecialEvent();
}

// --------------------------------------------------------------------------------------
void CRuneDev::UpdateSpecialEvent()
{
	if (m_pRuTerrain == NULL)
		return;

	CRuWorld_ObjectManager* objectManager = m_pRuTerrain->GetObjectManager();
	if (objectManager)
	{
		char eventFilter[1024];
		memset(eventFilter, 0, sizeof(eventFilter));

		if (m_specialEvent.empty())
		{
			strcpy(eventFilter, m_specialEventFilter);
		}
		else
		{
			for (vector<string>::iterator iter = m_specialEvent.begin(); iter != m_specialEvent.end(); iter++)
			{
				if (eventFilter[0])
				{
					strcat(eventFilter, ";");
				}
				strcat(eventFilter, iter->c_str());
			}

			if (stricmp(m_specialEventFilter, "none") != 0)
			{
				strcat(eventFilter, ";");
				strcat(eventFilter, m_specialEventFilter);
			}
		}

		objectManager->SetSpecialEventFilter(eventFilter);
	}
}

// --------------------------------------------------------------------------------------
void CRuneDev::Update()
{
	float last, time;

	{
		DWORD width = m_clientRect.right - m_clientRect.left;
		DWORD height = m_clientRect.bottom - m_clientRect.top;
		if (m_xScreen != width || m_yScreen != height)
		{
			SetScreenSize(width, height, m_isWindowed, FALSE);
		}
	}

	// 檢查單次刪除 RoleSprite 個數
	{
		int nowSize = (int)m_spriteContainer.size();
		if (m_lastFrameRoleSpriteCount > 0)
		{
			float deleteRate = (float)m_deleteRoleSpriteCount / (float)m_lastFrameRoleSpriteCount;

			if (deleteRate > 0.3)
			{
				// Clear instance manager cache
				g_ruInstanceManager->ClearCache();

				// Flush resource manager cache now
				g_ruResourceManager->GarbageCollect();
			}
		}

		m_lastFrameRoleSpriteCount = nowSize;
		m_deleteRoleSpriteCount = 0;
	}

	// Camera Update	
	if (m_cameraMoveTime > 0.0f) {
		MoveCameraPos(m_cameraMoveVector * min(m_elapsedTime, m_cameraMoveTime));
		m_cameraMoveTime -= m_elapsedTime;
	}

	if (m_pCamera)
	{
		// Update 3D sound positioning
		//const CRuVector3 &playerPosition = m_pCamera->GetTargetPosition();
		//const CRuVector3 &playerOrientation = m_pCamera->GetCameraLookAtVector();
		//g_ruSymphony->SetListenerOrientation(playerOrientation, m_pCamera->GetCameraUpVector());
		//g_ruSymphony->SetListenerPosition(playerPosition);
	}

	// Interface
	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[1] += time - last;

	RUPROFILE_SAMPLE_BEGIN(g_probe_Update_Interface, 0);
	UpdateInterface();
	RUPROFILE_SAMPLE_END(g_probe_Update_Interface, 0);

	// Terrain
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	last = time;

	RUPROFILE_SAMPLE_BEGIN(g_probe_Update_Terrain, 0);
	UpdateTerrain();
	RUPROFILE_SAMPLE_END(g_probe_Update_Terrain, 0);

	// Sprites
	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[0] += time - last;

	RUPROFILE_SAMPLE_BEGIN(g_probe_Update_Sprite, 0);
	// Update sprites
	if (m_isSprite_Update)
		UpdateSprite();
	RUPROFILE_SAMPLE_END(g_probe_Update_Sprite, 0);

	// Environment
	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[2] += time - last;

	RUPROFILE_SAMPLE_BEGIN(g_probe_Update_Environment, 0);
	if (m_isEnvironmentUpdate)
		UpdateEnvironment(m_playerPos);
	RUPROFILE_SAMPLE_END(g_probe_Update_Environment, 0);

	RUPROFILE_SAMPLE_ACCUMULATE(g_probe_Update_UpdateInvokeCount, 0, (float)CRuEntity::GetUpdateInvokeCount());
	CRuEntity::ResetUpdateInvokeCount();

	RUPROFILE_SAMPLE_ACCUMULATE(g_probe_Update_EventRaiseCount, 0, (float)CRuEntity::GetEventRaiseCount());
	CRuEntity::ResetEventRaiseCount();

	RUPROFILE_SAMPLE_ACCUMULATE(g_probe_Update_ListenerInvokeCount, 0, (float)CRuEntity::GetListenerInvokeCount());
	CRuEntity::ResetListenerInvokeCount();

	// Update flag editor
	FlagEditor_UpdateFlags();

	m_paperdollCache->Update(m_elapsedTime);

	// Highlighter
	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[3] += time - last;

	if (m_pRuHighlighter)
	{
		m_pRuHighlighter->Update(m_elapsedTime);
	}

	if (m_pRuWidgetGrid)
	{
		CRuVector3 pos;
		if (m_isGridLock)
		{
			pos = m_playerPos;
			pos.m_y += 16.0f;
		}
		else
			pos = m_gridPos;

		pos.m_y += 5.0f;
		m_pRuWidgetGrid->SnapGrid(CRuVector3(0.0f, 0.0f, 0.0f), pos);
		m_pRuWidgetGrid->Update(m_elapsedTime);
	}

	if (m_pRuMoveTargetEntity)
		m_pRuMoveTargetEntity->Update(m_elapsedTime);
}

// ------------------------------------------------------------------------------------------------
/*
extern CGameMain						*g_pGameMain;
CRuFX2D_MugShot *g_mugShot = NULL;
*/
bool CRuneDev::FrameLoop()
{
	static float s_time = 0.0f;
	static float s_stableTime = 0.0f;

	DWORD timeMS = m_pRuTimer->GetTime();
	float time = (float)timeMS / 1000.0f;
	float elapsedTime = min((timeMS - m_frameTimeMS) / 1000.0f, 2.0f);
	float last;

	/*
	// 控制每次的更新時間不超過正常值,多的會留待下一次更新
	static float remainingTime = 0.0f;
	elapsedTime += remainingTime;
	remainingTime = 0.0f;
	if ( elapsedTime > 0.2f )
	{
		remainingTime = min(0.8f, elapsedTime - 0.2f);
		elapsedTime = 0.2f;
	}
	*/

	/*
	// 控制穩速
	if ( m_isStableFrame )
	{
		while ( elapsedTime < s_stableTime )
		{
			time = (float)m_pRuTimer->GetTime() / 1000.0f;
			elapsedTime = time - m_frameTime;
		}
		//if ( elapsedTime < s_stableTime )
		//	return true;

		if ( elapsedTime > 0.2f )
			s_stableTime = 0.0f;
		else
			s_stableTime = elapsedTime * 0.95f;
	}
	*/

	if (g_debug_EnableThrottle)
	{
		static DWORD s_lastThrottleTime = 0;
		DWORD time = m_pRuTimer->GetTime();

		if ((time - s_lastThrottleTime) < 50)
		{
			return true;
		}

		s_lastThrottleTime = time;
	}

	// Check settings
	RuPropertyType propertyValue;
	REAL uiScale = 1.0f;

	if (g_ruEngineSettings->GetSetting("uiscale:enable", propertyValue) && boost::get<BOOL>(propertyValue) == TRUE)
	{
		if (g_ruEngineSettings->GetSetting("ui:scale", propertyValue))
		{
			uiScale = boost::get<REAL>(propertyValue);
		}
	}
	if (fabs(g_pInterface->GetScale() - uiScale) > 0.01f)
	{
		g_pInterface->SetScale(uiScale);
	}

	if (g_ruEngineSettings->GetSetting("world:view distance", propertyValue))
	{
		REAL viewDistance = boost::get<REAL>(propertyValue);

		viewDistance = max(min(viewDistance, 3000.0f), m_cameraTempFarPlane);

		if (fabs(viewDistance - m_cameraFarPlane) > 1.0f)
		{
			// Adjust far plane, and compute backdrop near/far plane distances based on new far plane distance
			m_cameraFarPlane = viewDistance;

			m_cameraBackdropNearPlane = max(100.0f, m_cameraFarPlane - 960.0f);
			m_cameraBackdropFarPlane = 4800.0f - max(0.0f, 2000.0f - m_cameraFarPlane);

			// Update camera
			m_pCamera->SetPerspectiveProjectionFOV(m_cameraNearPlane, m_cameraFarPlane, m_cameraFOV, (float)m_xScreen / m_yScreen);
			m_pCamera->SetBackdropDistance(m_cameraBackdropNearPlane, m_cameraBackdropFarPlane);

			// Force world environment to update
			m_pRuWorldEnvironment->IncrementIteration();

			// Update terrain view distance configuration
			if (m_pRuTerrain)
			{
				// 加速地表讀取
				g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 200.0f);
				g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 200.0f);

				// Give global worker thread maximum allocation
				g_ruGlobalWorkerThread->SetSleepPeriod(0);
				g_ruResourceManager->SetObjectDequeueRate(200.0f);
				g_ruResourceManager->SetTextureDequeueRate(200.0f);

				m_pRuTerrain->ModifyViewDistanceConfiguration(viewDistance);

				m_pRuTerrain->SetCamera(m_pCamera);

#ifndef KALYDO 
				// in Kalydo case, don't care about terrain not being ready yet, go on anyway and draw what's there
				while (m_pRuTerrain->GetLoadingStatus() < 1.0f)
				{
					// Update global function schedule
					g_ruFunctionSchedule->Update();

					m_pRuTerrain->SetCamera(m_pCamera);
					m_pRuTerrain->Update(0.0f);
					Sleep(5);
				}
#endif

				// Force collision data to be reset each cycle for 3 cycles
				m_forceColDataResetCycles = 3;

				g_ruEngineSettings->SetSetting("system:chunk dequeue rate", 8.0f);
				g_ruEngineSettings->SetSetting("system:backdrop dequeue rate", 16.0f);

				// Restore global worker thread default allocation
				g_ruGlobalWorkerThread->SetSleepPeriod(25);
				g_ruResourceManager->SetObjectDequeueRate(5.0f);
				g_ruResourceManager->SetTextureDequeueRate(25.0f);
			}
		}
	}

	m_frameTime = static_cast<double>(time);
	m_frameTimeMS = timeMS;
	m_elapsedTime = elapsedTime;

	// Set primary camera into global event manager
	IBaseSprite::GetGlobalEventManager()->SetCamera(m_pCamera);

	// Chain update to base class
	Update();

	// 更新鏡頭及控制器
	if (m_updateCameraController) {
		m_pRuCameraController->Update(m_elapsedTime);
	}
	m_pCamera->Update(m_elapsedTime);

	// If we have failed to present ten times in a row, forcibly reset the device
	if (m_failedPresentCount > 10)
	{
		m_failedPresentCount = 0;
		g_ruEngine->ResetDevice();
	}

	// If swap chain was invalidated, reset the device
	if (m_pRuSwapChain->IsInvalidated() && m_isActived)
	{
		if (g_ruEngine->ResetDevice() == FALSE)
		{
			++m_failedEngineResetCount;
		}
		else if (g_ruEngine->RestoreDevice() == FALSE)
		{
			++m_failedEngineResetCount;
		}

		// If we have failed to reset five times in a row, forcibly set screen size again using windowed mode
		if (m_failedEngineResetCount > 5)
		{
			m_failedEngineResetCount = 0;

			m_xScreen += 1;
			m_yScreen += 1;

			// TODO: Вызывает краш после разворачивания игры
			SetScreenSize(m_fullscreenResolution.cx, m_fullscreenResolution.cy, m_isWindowed, FALSE);
		}
	}

	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[4] += time - last;

	// Update global function schedule
	g_ruFunctionSchedule->Update();

	// 開始繪製
	if (!m_pRuSwapChain->IsInvalidated() && g_ruEngine->BeginScene())
	{
		// Perform present
		if (g_debug_FlipPresent)
		{
			if (m_pRuSwapChain->Present() == FALSE)
			{
				++m_failedPresentCount;
			}
		}

		// 開始3D繪圖
		g_ruRenderer->Begin();

		// 選取主要swap chain為繪製平面
		// m_pRuSwapChain->SelectRenderTarget();

		// 選用主鏡頭
		g_ruDefaultRenderContext->Camera_Push(m_pCamera);
		g_ruUIRenderer->Camera_Push(m_pCamera);

		// 設定主要swap chain為輸出目標
//		g_ruUIRenderer->SetRenderTarget( m_pRuSwapChain, 1000, RUCLEAR_TARGET | RUCLEAR_ZBUFFER | RUCLEAR_STENCIL, 0x00000000, 1.0f, 0);
//		g_ruEngine->Clear( RUCLEAR_TARGET, 0x00000000, 0.0f, 0 );

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[5] += time - last;

		RUPROFILE_SAMPLE_BEGIN(g_probe_Submit_Environment, 0);
		if (m_isEnvironmentRender)
			RenderEnvironment();
		RUPROFILE_SAMPLE_END(g_probe_Submit_Environment, 0);

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[6] += time - last;

		// 繪製地表跟天空
		RUPROFILE_SAMPLE_BEGIN(g_probe_Submit_Terrain, 0);
		RenderTerrain();
		RUPROFILE_SAMPLE_END(g_probe_Submit_Terrain, 0);

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[7] += time - last;

		if (m_pRuWidgetGrid)
			g_ruDefaultRenderContext->Submit(m_pRuWidgetGrid);

		// 繪出角色
		RUPROFILE_SAMPLE_BEGIN(g_probe_Submit_Sprite, 0);
		if (m_isSprite_Render)
			RenderSprite();
		RUPROFILE_SAMPLE_END(g_probe_Submit_Sprite, 0);

		if (m_targetPolyLineEntity && m_targetEntityVisible)
			g_ruDefaultRenderContext->Submit(m_targetPolyLineEntity);
		if (m_targetEntity && m_targetEntityVisible)
			g_ruDefaultRenderContext->Submit(m_targetEntity);

		// 繪製選取範圍
		if (m_pRuSelectAreaEntity)
		{
			CRuVector3 newPosition;
			bool boStop = false;

			if (m_pRuSelectAreaStopEntity)
			{
				FlagEditor_RepositionFlagToCursor(m_pRuSelectAreaStopEntity, m_xCursor, m_yCursor);
				if (FlagEditor_RepositionFlagToCursor(m_pRuSelectAreaEntity, m_xCursor, m_yCursor, &newPosition) == TRUE)
				{
					if ((m_playerPos - newPosition).Magnitude() > m_selectAreaDistance)
					{
						boStop = true;
						g_ruDefaultRenderContext->Submit(m_pRuSelectAreaStopEntity);
					}
				}
			}
			else
			{
				FlagEditor_RepositionFlagToCursor(m_pRuSelectAreaEntity, m_xCursor, m_yCursor);
			}

			if (boStop == false)
				g_ruDefaultRenderContext->Submit(m_pRuSelectAreaEntity);
		}

		// Mouse Target
		if (m_pRuMoveTargetEntity)
			g_ruDefaultRenderContext->Submit(m_pRuMoveTargetEntity);

		// Render flag editor flags
		FlagEditor_RenderFlags();

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[8] += time - last;

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[10] += time - last;

		if (m_isBlind)
		{
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_DARKEN);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_STRENGTH, 80);
		}

		RUPROFILE_SAMPLE_BEGIN(g_probe_C0, 0);

		// 繪出界面
		if (m_interface)
			m_interface->Render();

		RUPROFILE_SAMPLE_END(g_probe_C0, 0);

		if (g_enableProfilerOutput)
		{
			RUCOLOR textColor = RUCOLOR_ARGB(255, 160, 160, 240);

			WCHAR* tmp = NULL;
			char text[MAX_PATH];

			// Output profiler prefix data
			sprintf(text, "Profiling Level %d", g_ruProfilingLevel);

			tmp = TransToWChar(text);
			CRuFontString::DrawText(m_pSysFontType_Small, 0, 0, textColor, tmp);
			SAFE_DELETE(tmp);

			char* probeGroups[] = { "default", "Engine", "Scene", "Terrain", "World", "Misc", NULL };
			int probeGroupOrigins[][2] = { { 0, 32 }, { 160, 32 }, { 320, 32 }, { 480, 32 }, { 640, 32 }, { 800, 32 } };

			for (INT32 i = 0; probeGroups[i] != NULL; ++i)
			{
				// Output probe group prefix
				tmp = TransToWChar(probeGroups[i]);
				CRuFontString::DrawText(m_pSysFontType_Small, (float)probeGroupOrigins[i][0], (float)probeGroupOrigins[i][1], textColor, tmp);
				SAFE_DELETE(tmp);

				for (INT32 j = 0, oX = probeGroupOrigins[i][0], oY = probeGroupOrigins[i][1] + 32, numProbes = g_ruProfiler->GetNumProbes(); j < numProbes; ++j)
				{
					if (stricmp(g_ruProfiler->GetProbeGroup(j), probeGroups[i]) == 0)
					{
						sprintf(text, "[%s %.2f]", g_ruProfiler->GetProbeName(j), g_ruProfiler->GetProbeMeanValue(j));

						tmp = TransToWChar(text);
						CRuFontString::DrawText(m_pSysFontType_Small, (float)(oX - 1), (float)(oY - 1), 0xFF000000, tmp);
						CRuFontString::DrawText(m_pSysFontType_Small, (float)oX, (float)oY, textColor, tmp);
						SAFE_DELETE(tmp);

						oY += 12;
					}
				}
			}

			// Reset profiler samples
			RUPROFILE_SAMPLE_CLEAR()
		}


		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[11] += time - last;

		/*
		CRuFontString::DrawText(m_pSysFontType, 100, 60, 0xFFFFFFFF, L"نلتف[fg…(rg]63شش");
		CRuFontString::DrawText(m_pSysFontType, 100, 80, 0xFFFFFFFF, L"[ابت هبق]:fyrhr(rrrrr)");
		CRuFontString::DrawText(m_pSysFontType, 100, 100, 0xFFFFFFFF, L"[بتب]87(");
		CRuFontString::DrawText(m_pSysFontType, 100, 120, 0xFFFFFFFF, L"نه<CS>Quest Items");
		*/
		//CRuFontString::DrawText(m_pSysFontType, 100, 100, 0xFFFFFFFF, L"АаБбВвГгДдЕеЁёЖж");
		//CRuFontString::DrawText(m_pSysFontType, 100, 140, 0xFFFFFFFF, L"ชื่อตัวละคร 14-15 ตัวอักษเป็นภาษาอังกฤษเท่านั้นและขึ้นต้นด้วยตัวใหญ่ ตัวถัดมาเป็นตัวเล็ก");

		if (m_isShowFps)
		{
			WCHAR* tmp = NULL;
			char text[MAX_PATH];

			sprintf(text, "fps : %.2f %.4f", m_fps, s_stableTime);
			tmp = TransToWChar(text);
			CRuFontString::DrawText(m_pSysFontType, 800, 10, 0xFFFFFF00, tmp);
			SAFE_DELETE(tmp);
			sprintf(text, "sprite : %d", IBaseSprite::GetContainer().size());
			tmp = TransToWChar(text);
			CRuFontString::DrawText(m_pSysFontType, 800, 28, 0xFFFFFF00, tmp);
			SAFE_DELETE(tmp);

			sprintf(text, "Texture:%d/%d", CTextureImage::LoadedCapacity(), MAX_TEXTUREIMAGE);
			tmp = TransToWChar(text);
			CRuFontString::DrawText(m_pSysFontType, 800, 46, 0xFFFFFF00, tmp);
			SAFE_DELETE(tmp);

			/*
			sprintf( text, "Small font:%d/%d Large font:%d/%d",
				CRuFontTexture::GetSmallFontSize() - CRuFontTexture::FreeSmallFontCapacity(), CRuFontTexture::GetSmallFontSize(),
				CRuFontTexture::GetLargeFontSize() - CRuFontTexture::FreeLargeFontCapacity(), CRuFontTexture::GetLargeFontSize() );
			tmp = TransToWChar(text);
			CRuFontString::DrawText( m_pSysFontType, 800, 64, 0xFFFFFF00, tmp );
			SAFE_DELETE(tmp);
			*/

			int line = 0;
			for (int i = 0; i < 16; i++)
			{
				float time = abs(g_qualityRecord[i].time);
				sprintf(text, "%.4f(%.2f%%) %s", time, time / (s_time / m_fps), g_qualityRecord[i].name);
				tmp = TransToWChar(text);
				CRuFontString::DrawText(m_pSysFontType, (float)800, (float)(82 + i * 16), 0xBFDEDEDE, tmp);
				SAFE_DELETE(tmp);
			}
		}

		/*
		char tmp[MAX_PATH];
		sprintf(tmp, "Screen Size (%d x %d) UISize (%.1f x %.1f)", m_xScreen, m_yScreen, m_UIWidth, m_UIHeight);
		CRuFontString::DrawText( m_pSysFontType, 400.0f, 100.0f, 0xBFDEDE00, tmp );
		*/

		// Mouse cursor rendering
#ifndef KALYDO
		// For Kalydo: Only render mouse with WM_SETCURSOR; otherwise it will be seen inside the browser outside of the game.
		RenderCursor();
#endif // KALYDO

		/*
		// DEBUG: Test output font texture
		CRuRectangle dest, src;
		DWORD color = -1;
		CRuFontTexture2::PaintBegin();

		src.m_left		= 0.0f;
		src.m_right		= CRuFontTexture2::GetRuTextureWidth();
		src.m_top		= 0.0f;
		src.m_bottom	= CRuFontTexture2::GetRuTextureHeight();

		dest = src;
		//dest.m_left		-= 256.0f;
		//dest.m_top		-= 256.0f;
		//dest.m_right	-= 256.0f;
		//dest.m_bottom	-= 256.0f;

		g_ruUIRenderer->Submit(1, &src, &dest, &color);
		*/


		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[12] += time - last;

		// 結束3D繪圖
		g_ruRenderer->End();

		// 結束繪製
		if (!g_ruEngine->EndScene())
		{
		}

		last = time;
		time = (float)m_pRuTimer->GetTime() / 1000.0f;
		g_handleTime[13] += time - last;

		// Present主要swap chain
		if (!g_debug_FlipPresent)
		{
			if (m_pRuSwapChain->Present() == FALSE)
			{
				++m_failedPresentCount;
			}
		}
	}

	last = time;
	time = (float)m_pRuTimer->GetTime() / 1000.0f;
	g_handleTime[14] += time - last;

	// 輸出視窗資訊
	{
		static double oldTime;
		static double frame;

		/*
		if ( m_elapsedTime > maxTime )
			maxTime = m_elapsedTime;
		*/

		// 計算每秒張數
		if (m_frameTime - oldTime >= 1.0f)
		{
			m_fps = static_cast<float>(frame / (m_frameTime - oldTime));
			s_time = float(m_frameTime - oldTime);

			for (int i = 0; i < 16; i++)
			{
				if (i == 15)
				{
					g_qualityRecord[i].time = 0.0f;
					for (int j = 5; j < 14; j++)
						g_qualityRecord[i].time += g_qualityRecord[j].time;
				}
				else
				{
					g_qualityRecord[i].time = float(g_handleTime[i] / frame);
				}
				g_handleTime[i] = 0.0f;
			}
			oldTime = m_frameTime;
			frame = 0.0f;
		}
		frame++;
	}
	// TODO: Если делать задержку, то звуки кликают
	// If window is not active, sleep for 33 milliseconds
	if (!m_isActived)
		SleepEx(33, true);

	return true;
}

// ------------------------------------------------------------------------------------------------
LRESULT CRuneDev::WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	// DEBUG CODE
	//if(iMsg != WM_NCHITTEST && iMsg != WM_SETCURSOR && iMsg != WM_MOUSEMOVE && iMsg != WM_LBUTTONUP && iMsg != WM_LBUTTONDOWN && iMsg != WM_CHAR && iMsg != WM_KEYUP && iMsg != WM_KEYDOWN)
	//{
	//	char debugMsg[2048];
	//	sprintf(debugMsg, "WndProc Msg: %x %u %u\n", iMsg, wParam, lParam);
	//	OutputDebugString(debugMsg);
	//}

	switch (iMsg)
	{
	case WM_SETCURSOR:
		SetCursor(NULL);
		if (g_ruEngine)
		{
			g_ruEngine->SetCursor(0, 0, m_pCursor_HW);
			g_ruEngine->ShowCursor(m_isCursorVisible);
		}
		return TRUE;

	case WM_MOUSEMOVE:
		if (m_isCursorLock)
		{
			SetCursorPosition(m_xCursor, m_yCursor);
		}
		else
		{
			m_xCursor = int(lParam & 0xFFFF);
			m_yCursor = int(lParam >> 16);
		}
		return 0;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			m_isActived = false;
		else
			m_isActived = true;
		return 0;
	case WM_ACTIVATEAPP:
		m_isActived = (wParam) ? true : false;
		SetFocus(hWnd);
		break;

	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return 0;
	}

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
		if (wParam != VK_F10)
		{
			// Sys key event is fired because nobody has focus
			if ((lParam & (0x01 << 29)) == 0)
			{
				// Forcibly get focus for our window
				SetFocus(hWnd);

				// Redirect input as a regular key input message

				return 0;
			}
		}
		break;
	}

	if (m_editingModeOn == false || m_editingDragOn == false)
	{
		if (m_interface && m_interface->WndProc(iMsg, wParam, lParam))
			return 0;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

// ------------------------------------------------------------------------------------------------
string CRuneDev::GetDefaultFontPath()
{
	return DEFAULT_FONT_FILENAME;
}

// ------------------------------------------------------------------------------------------------
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pRuneDev->WndProc(hWnd, iMsg, wParam, lParam);
}

// --------------------------------------------------------------------------------------
void SpriteMessageProc(IBaseSprite* sprite, SpriteMessageType msg, INT64 arg1, INT64 arg2)
{
	g_pRuneDev->SpriteMsgProc(sprite, msg, arg1, arg2);
}

// ------------------------------------------------------------------------------------------------
void FontTextureRally()
{
	g_pRuneDev->ResetRoleSpriteWidget();
}
// ------------------------------------------------------------------------------------------------
bool CRuneDev::SetMiniMapMask()
{
	if (m_minimapMaskFile.size() != 0)
	{
		IRuTexture* maskTexture = g_ruResourceManager->LoadTexture(m_minimapMaskFile.c_str());

		if (maskTexture)
		{
			if (m_pRuMinimap)
			{
				m_pRuMinimap->SetMinimapMask(maskTexture);
			}
			ruSAFE_RELEASE(maskTexture);
			return true;
		}
	}

	return false;
}
// ------------------------------------------------------------------------------------------------
void CRuneDev::CreateTargetEntity()
{
	RelaseTargetEntity();

	m_targetEntity = g_ruResourceManager->LoadEntity("model\\item\\ui_object\\target_circle\\act_nor_target_circle.ros");
	if (m_targetEntity)
		RuACT_PlayMotion(m_targetEntity, "idle");

	m_targetPolyLineEntity = g_ruResourceManager->LoadEntity("model\\item\\ui_object\\act_dotted_line.ros");
	if (m_targetPolyLineEntity)
		RuACT_PlayMotion(m_targetPolyLineEntity, "idle");
}

// ------------------------------------------------------------------------------------------------
void CRuneDev::SetTargetEntityTarget(CRuEntity* parentEntity, const char* point)
{
	if (m_targetEntity == NULL || m_targetPolyLineEntity == NULL || parentEntity == NULL)
		return;

	// 自父元件解除連結
	m_targetEntity->DetachFromParent();
	m_targetPolyLineEntity->DetachFromParent();

	m_targetAttachPoint = point;
	m_targetParentEntity = parentEntity;

	RuEntity_Attach_BFS(parentEntity, m_targetPolyLineEntity, point);

	if (parentEntity->GetScale() != 1.0f)
	{
		m_targetPolyLineEntity->Scale(1.0f / parentEntity->GetScale(), ruTRANSFORMOP_PRECONCAT);
	}

	CRuEntity* childEntity = RuEntity_FindLinkFrame(m_targetEntity, ATTACH_POINT_DOWN);
	if (childEntity)
	{
		RuPolyLine_SetTarget(m_targetPolyLineEntity, childEntity);
	}
	else
	{
		RuPolyLine_SetTarget(m_targetPolyLineEntity, m_targetEntity);
	}
}

// ------------------------------------------------------------------------------------------------
void CRuneDev::RelaseTargetEntity()
{
	// 自父元件解除連結
	if (m_targetEntity)
		m_targetEntity->DetachFromParent();
	if (m_targetPolyLineEntity)
		m_targetPolyLineEntity->DetachFromParent();

	ruSAFE_RELEASE(m_targetEntity);
	ruSAFE_RELEASE(m_targetPolyLineEntity);

	m_targetParentEntity = NULL;
	m_targetAttachPoint.clear();
}

// ------------------------------------------------------------------------------------------------
void CRuneDev::SetTargetEntityVisible(bool visible)
{
	if ((bool)m_targetEntityVisible != visible)
	{
		m_targetEntityVisible = visible;

		if (m_targetEntityVisible)
		{
			RuEntity_Attach_BFS(m_targetParentEntity, m_targetPolyLineEntity, m_targetAttachPoint.c_str());
		}
		else
		{
			m_targetPolyLineEntity->DetachFromParent();
		}
	}
}
