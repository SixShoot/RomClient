//--------------------------------------------------------------------------------------
// File: DXUTEnum.h
//
// Enumerates D3D adapters, devices, modes, etc.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_ENUM_H
#define DXUT_ENUM_H

#include <base\base_Array.h>
//--------------------------------------------------------------------------------------
// Error codes
//--------------------------------------------------------------------------------------
#define DXUTERR_NODIRECT3D              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0901)

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo;
class CD3DEnumDeviceInfo;
struct CD3DEnumDeviceSettingsCombo;
struct CD3DEnumDSMSConflict;
typedef bool    (CALLBACK *LPDXUTCALLBACKISDEVICEACCEPTABLE)( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
struct DXUTDeviceSettings
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	DWORD BehaviorFlags;
	D3DPRESENT_PARAMETERS pp;
};
//--------------------------------------------------------------------------------------
// Enumerates available Direct3D adapters, devices, modes, etc.
// Use DXUTGetEnumeration() to access global instance
//--------------------------------------------------------------------------------------
class CD3DEnumeration
{
public:
    // These should be called before Enumerate(). 
    //
    // Use these calls and the IsDeviceAcceptable to control the contents of 
    // the enumeration object, which affects the device selection and the device settings dialog.
    void SetRequirePostPixelShaderBlending( bool bRequire ) { m_bRequirePostPixelShaderBlending = bRequire; }
    void SetResolutionMinMax( UINT nMinWidth, UINT nMinHeight, UINT nMaxWidth, UINT nMaxHeight );  
    void SetRefreshMinMax( UINT nMin, UINT nMax );
    void SetMultisampleQualityMax( UINT nMax );    
    void GetPossibleVertexProcessingList( bool* pbSoftwareVP, bool* pbHardwareVP, bool* pbPureHarewareVP, bool* pbMixedVP );
    void SetPossibleVertexProcessingList( bool bSoftwareVP, bool bHardwareVP, bool bPureHarewareVP, bool bMixedVP );
	base::Array<D3DFORMAT>* GetPossibleDepthStencilFormatList();   
    base::Array<D3DMULTISAMPLE_TYPE>* GetPossibleMultisampleTypeList();   
    base::Array<UINT>* GetPossiblePresentIntervalList();
    void ResetPossibleDepthStencilFormats();
    void ResetPossibleMultisampleTypeList();
    void ResetPossiblePresentIntervalList();

    // Call Enumerate() to enumerate available D3D adapters, devices, modes, etc.
    HRESULT Enumerate( IDirect3D9* pD3D = NULL,
                       LPDXUTCALLBACKISDEVICEACCEPTABLE IsDeviceAcceptableFunc = NULL,
                       void* pIsDeviceAcceptableFuncUserContext = NULL );

    // These should be called after Enumerate() is called
    base::Array<CD3DEnumAdapterInfo*>*   GetAdapterInfoList();  
    CD3DEnumAdapterInfo*                    GetAdapterInfo( UINT AdapterOrdinal );  
    CD3DEnumDeviceInfo*                     GetDeviceInfo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType );    
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( DXUTDeviceSettings* pDeviceSettings ) { return GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed ); }
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL Windowed );  

    ~CD3DEnumeration();

private:
    friend CD3DEnumeration* DXUTGetEnumeration(); 

    // Use DXUTGetEnumeration() to access global instance
    CD3DEnumeration();

    IDirect3D9* m_pD3D;                                    
    LPDXUTCALLBACKISDEVICEACCEPTABLE m_IsDeviceAcceptableFunc;
    void* m_pIsDeviceAcceptableFuncUserContext;
    bool m_bRequirePostPixelShaderBlending;
    base::Array<D3DFORMAT> m_DepthStecilPossibleList;
    base::Array<D3DMULTISAMPLE_TYPE> m_MultiSampleTypeList;
    base::Array<UINT> m_PresentIntervalList;

    bool m_bSoftwareVP;
    bool m_bHardwareVP;
    bool m_bPureHarewareVP;
    bool m_bMixedVP;

    UINT m_nMinWidth;
    UINT m_nMaxWidth;
    UINT m_nMinHeight;
    UINT m_nMaxHeight;
    UINT m_nRefreshMin;
    UINT m_nRefreshMax;
    UINT m_nMultisampleQualityMax;

    // Array of CD3DEnumAdapterInfo* with unique AdapterOrdinals
    base::Array<CD3DEnumAdapterInfo*> m_AdapterInfoList;  

    HRESULT EnumerateDevices( CD3DEnumAdapterInfo* pAdapterInfo, base::Array<D3DFORMAT>* pAdapterFormatList );
    HRESULT EnumerateDeviceCombos( CD3DEnumAdapterInfo* pAdapterInfo, CD3DEnumDeviceInfo* pDeviceInfo, base::Array<D3DFORMAT>* pAdapterFormatList );
    void BuildDepthStencilFormatList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildMultiSampleTypeList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildDSMSConflictList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildPresentIntervalList( CD3DEnumDeviceInfo* pDeviceInfo, CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void ClearAdapterInfoList();
};

CD3DEnumeration* DXUTGetEnumeration(); 


//--------------------------------------------------------------------------------------
// A class describing an adapter which contains a unique adapter ordinal 
// that is installed on the system
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo
{
public:
    ~CD3DEnumAdapterInfo();

    UINT AdapterOrdinal;
    D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
    WCHAR szUniqueDescription[256];

    base::Array<D3DDISPLAYMODE> displayModeList; // Array of supported D3DDISPLAYMODEs
    base::Array<CD3DEnumDeviceInfo*> deviceInfoList; // Array of CD3DEnumDeviceInfo* with unique supported DeviceTypes
};


//--------------------------------------------------------------------------------------
// A class describing a Direct3D device that contains a 
//       unique supported device type 
//--------------------------------------------------------------------------------------
class CD3DEnumDeviceInfo
{
public:
    ~CD3DEnumDeviceInfo();

    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DCAPS9 Caps;

    // List of CD3DEnumDeviceSettingsCombo* with a unique set 
    // of AdapterFormat, BackBufferFormat, and Windowed
    base::Array<CD3DEnumDeviceSettingsCombo*> deviceSettingsComboList; 
};


//--------------------------------------------------------------------------------------
// A struct describing device settings that contains a unique combination of 
// adapter format, back buffer format, and windowed that is compatible with a 
// particular Direct3D device and the app.
//--------------------------------------------------------------------------------------
struct CD3DEnumDeviceSettingsCombo
{
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    D3DFORMAT BackBufferFormat;
    BOOL Windowed;

    base::Array<D3DFORMAT> depthStencilFormatList; // List of D3DFORMATs
    base::Array<D3DMULTISAMPLE_TYPE> multiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
    base::Array<DWORD> multiSampleQualityList; // List of number of quality levels for each multisample type
    base::Array<UINT> presentIntervalList; // List of D3DPRESENT flags
    base::Array<CD3DEnumDSMSConflict> DSMSConflictList; // List of CD3DEnumDSMSConflict

    CD3DEnumAdapterInfo* pAdapterInfo;
    CD3DEnumDeviceInfo* pDeviceInfo;
};


//--------------------------------------------------------------------------------------
// A depth/stencil buffer format that is incompatible with a
// multisample type.
//--------------------------------------------------------------------------------------
struct CD3DEnumDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};


#endif
