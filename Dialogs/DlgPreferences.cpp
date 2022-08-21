/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// DlgPreferences.cpp : implementation file
//

#include "StdH.h"
#include "DlgPreferences.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences dialog
#define PREF_COMBO_HEIGHT 100


CDlgPreferences::CDlgPreferences( CWnd* pParent /*=NULL*/)
  : CDialog(CDlgPreferences::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDlgPreferences)
  m_PrefsCopy = FALSE;
  m_AutoMaximize = FALSE;
  m_SetDefaultColors = FALSE;
  m_AutomaticInfo = FALSE;
  m_UpdateAllways = FALSE;
  m_BinaryGrid = FALSE;
  m_bSaveUndoForDelete = FALSE;
  m_iWndStartupCfg = 0;
  m_bAutoColorize = FALSE;
  m_strSSProject = _T("");
  m_bShowAllOnOpen = FALSE;
  m_strCSGPrecission = _T("");
  m_bAutoUpdateDisplaceMap = FALSE;
  m_fFlyModeSpeed = 0.0f;
  m_bHideShadowsOnStart = FALSE;

#if SE1_TERRAINS
  m_bAutoUpdateTerrainDistribution = FALSE;
  m_iMemoryForTerrainUndo = 0;
#endif
  //}}AFX_DATA_INIT
}


void CDlgPreferences::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  // if dialog is recieving data
  if( pDX->m_bSaveAndValidate == FALSE)
  {
    GetDlgItem(IDC_STATIC_SOURCE_SAFE_BORDER)->EnableWindow( FALSE);
    GetDlgItem(IDC_STATIC_SOURCE_SAFE_PROJECT)->EnableWindow( FALSE);
    GetDlgItem(IDC_EDIT_SS_PROJECT)->EnableWindow( FALSE);

    // [Cecil] Disable terrain properties
    #if !SE1_TERRAINS
      GetDlgItem(IDC_STATIC_TERRAIN_BORDER)->EnableWindow(FALSE);
      GetDlgItem(IDC_TEXT_TERRAIN_HIDDEN)->EnableWindow(FALSE);
      GetDlgItem(IDC_TEXT_TERRAIN_VISIBLE)->EnableWindow(FALSE);
      GetDlgItem(IDC_TEXT_TERRAIN_MEMORY)->EnableWindow(FALSE);

      GetDlgItem(IDC_TERRAIN_SELECTION_HIDDEN)->EnableWindow(FALSE);
      GetDlgItem(IDC_TERRAIN_SELECTON_VISIBLE)->EnableWindow(FALSE);
      GetDlgItem(IDC_PREFS_AUTO_GENERATE_TD)->EnableWindow(FALSE);
      GetDlgItem(IDC_MEMORY_FOR_UNDO)->EnableWindow(FALSE);
    #endif

    // select current api
    if( IsWindow(m_ctrGfxApi.m_hWnd))
    {
      for(INDEX iItem=0; iItem<m_ctrGfxApi.GetCount(); iItem++)
      {
        if(theApp.m_iApi==m_ctrGfxApi.GetItemData(iItem))
        {
          m_ctrGfxApi.SetCurSel(theApp.m_iApi);
          break;
        }
      }
    }
    m_strSSProject = theApp.m_Preferences.ap_strSourceSafeProject;
    m_PrefsCopy = theApp.m_Preferences.ap_CopyExistingWindowPrefs;
    m_AutoMaximize = theApp.m_Preferences.ap_AutoMaximizeWindow;
    m_SetDefaultColors = theApp.m_Preferences.ap_SetDefaultColors;
    m_AutomaticInfo = theApp.m_Preferences.ap_AutomaticInfo;
    m_BinaryGrid = theApp.m_Preferences.ap_BinaryGrid;
    m_UpdateAllways = theApp.m_Preferences.ap_UpdateAllways;
    m_bSaveUndoForDelete = theApp.m_Preferences.ap_bSaveUndoForDelete;
    m_bAutoColorize = theApp.m_Preferences.ap_bAutoColorize;
    m_bShowAllOnOpen = theApp.m_Preferences.ap_bShowAllOnOpen;
    m_bHideShadowsOnStart = theApp.m_Preferences.ap_bHideShadowsOnOpen;
    m_bAutoUpdateDisplaceMap = theApp.m_Preferences.ap_bAutoUpdateDisplaceMap;
    m_fFlyModeSpeed=theApp.m_Preferences.ap_fDefaultFlyModeSpeed;
    
    if( IsWindow(m_ctrlCSGPrecission.m_hWnd))
    {
#if SE1_TERRAINS
      m_ctrlTerrainSelectionVisible.SetCurSel(theApp.m_Preferences.ap_iTerrainSelectionVisible);
      m_ctrlTerrainSelectionHidden.SetCurSel(theApp.m_Preferences.ap_iTerrainSelectionHidden);
      m_iMemoryForTerrainUndo=theApp.m_Preferences.ap_iMemoryForTerrainUndo;
      m_bAutoUpdateTerrainDistribution=theApp.m_Preferences.ap_bAutoUpdateTerrainDistribution;
#endif

      INDEX iCSGPrecission=0;
      for(INDEX iExp=-5; iExp<=5; iExp+=1)
      {
        FLOAT fPow=pow(2.0f ,iExp*2);
        FLOAT fEpsilon=_pShell->GetFLOAT("mth_fCSGEpsilon");
        if( fEpsilon==fPow)
        {
          iCSGPrecission=iExp;
          break;
        }
      }
      m_ctrlCSGPrecission.SetPos( iCSGPrecission+5);
      CTString strCSGPrecission;
      if( _pShell->GetFLOAT("mth_fCSGEpsilon")==1.0f)
      {
        strCSGPrecission.PrintF("CSG Precission: 1 (default)");
      }
      else
      {
        strCSGPrecission.PrintF("CSG Precission: %g", _pShell->GetFLOAT("mth_fCSGEpsilon"));
      }
      m_strCSGPrecission=strCSGPrecission;
    }
    if( IsWindow(m_UndoLevels.m_hWnd))
    {
      m_UndoLevels.SetPos( theApp.m_Preferences.ap_iUndoLevels);
    }
    m_iWndStartupCfg = theApp.m_Preferences.ap_iStartupWindowSetup;
  }
  //{{AFX_DATA_MAP(CDlgPreferences)
#if SE1_TERRAINS
  DDX_Control(pDX, IDC_TERRAIN_SELECTION_HIDDEN, m_ctrlTerrainSelectionHidden);
  DDX_Control(pDX, IDC_TERRAIN_SELECTON_VISIBLE, m_ctrlTerrainSelectionVisible);
#endif
  DDX_Control(pDX, IDC_API, m_ctrGfxApi);
  DDX_Control(pDX, IDC_CSG_PRECISSION, m_ctrlCSGPrecission);
  DDX_Control(pDX, IDC_UNDO_LEVELS, m_UndoLevels);
  DDX_Control(pDX, IDOK, m_OkButton);
  DDX_Check(pDX, IDC_PREFS_COPY, m_PrefsCopy);
  DDX_Check(pDX, IDC_PREFS_MAXIMIZE, m_AutoMaximize);
  DDX_Check(pDX, IDC_PREFS_SET_DEFAULT_COLORS, m_SetDefaultColors);
  DDX_Check(pDX, IDC_PREFS_AUTOMATIC_INFO, m_AutomaticInfo);
  DDX_Check(pDX, IDC_PREFS_UPDATE_ALLWAYS, m_UpdateAllways);
  DDX_Check(pDX, IDC_PREFS_BINARY_GRID_BY_DEFAULT, m_BinaryGrid);
  DDX_Check(pDX, IDC_PREFS_SAVE_UNDO_FOR_DELETE, m_bSaveUndoForDelete);
  DDX_Text(pDX, IDC_WND_STARTUP_CFG, m_iWndStartupCfg);
  DDV_MinMaxInt(pDX, m_iWndStartupCfg, 0, 9);
  DDX_Check(pDX, IDC_PREFS_AUTO_COLORIZE, m_bAutoColorize);
  DDX_Text(pDX, IDC_EDIT_SS_PROJECT, m_strSSProject);
  DDX_Check(pDX, IDC_PREFS_SHOW_ALL_ON_OPEN, m_bShowAllOnOpen);
  DDX_Text(pDX, IDC_CSG_PRECISSION_T, m_strCSGPrecission);
  DDX_Check(pDX, IDC_PREFS_AUTO_UPDATE_DISPLACEMAP, m_bAutoUpdateDisplaceMap);
  DDX_Text(pDX, IDC_FLY_MODE_SPEED, m_fFlyModeSpeed);
  DDV_MinMaxFloat(pDX, m_fFlyModeSpeed, 0.f, 1000.f);
  DDX_Check(pDX, IDC_PREFS_HIDE_SHADOWS_ON_START, m_bHideShadowsOnStart);
#if SE1_TERRAINS
  DDX_Check(pDX, IDC_PREFS_AUTO_GENERATE_TD, m_bAutoUpdateTerrainDistribution);
  DDX_Text(pDX, IDC_MEMORY_FOR_UNDO, m_iMemoryForTerrainUndo);
  DDV_MinMaxInt(pDX, m_iMemoryForTerrainUndo, -1, 512);
#endif
  //}}AFX_DATA_MAP

  // if dialog is giving data
  if( pDX->m_bSaveAndValidate != FALSE)
  {
#if SE1_TERRAINS
    theApp.m_Preferences.ap_iTerrainSelectionVisible=m_ctrlTerrainSelectionVisible.GetCurSel();
    theApp.m_Preferences.ap_iTerrainSelectionHidden=m_ctrlTerrainSelectionHidden.GetCurSel();
    theApp.m_Preferences.ap_bAutoUpdateTerrainDistribution=m_bAutoUpdateTerrainDistribution;
    theApp.m_Preferences.ap_iMemoryForTerrainUndo=m_iMemoryForTerrainUndo;
#endif

    theApp.m_Preferences.ap_CopyExistingWindowPrefs = m_PrefsCopy;
    theApp.m_Preferences.ap_AutoMaximizeWindow = m_AutoMaximize;
    theApp.m_Preferences.ap_SetDefaultColors = m_SetDefaultColors;
    theApp.m_Preferences.ap_AutomaticInfo = m_AutomaticInfo;
    theApp.m_Preferences.ap_BinaryGrid = m_BinaryGrid;
    theApp.m_Preferences.ap_UpdateAllways = m_UpdateAllways;
    theApp.m_Preferences.ap_bSaveUndoForDelete = m_bSaveUndoForDelete;
    theApp.m_Preferences.ap_bAutoColorize = m_bAutoColorize;
    theApp.m_Preferences.ap_bShowAllOnOpen = m_bShowAllOnOpen;
    theApp.m_Preferences.ap_bHideShadowsOnOpen = m_bHideShadowsOnStart;
    theApp.m_Preferences.ap_bAutoUpdateDisplaceMap = m_bAutoUpdateDisplaceMap;
    theApp.m_Preferences.ap_iUndoLevels = m_UndoLevels.GetPos();
    _pShell->SetFLOAT("mth_fCSGEpsilon", pow(2.0f, (m_ctrlCSGPrecission.GetPos()-5)*2)); 
    theApp.m_Preferences.ap_iStartupWindowSetup = m_iWndStartupCfg;
    theApp.m_Preferences.ap_strSourceSafeProject = MfcStringToCT(m_strSSProject);
    theApp.m_Preferences.ap_fDefaultFlyModeSpeed=m_fFlyModeSpeed;
    // and write all data that can be written to INI file multiple times
    theApp.WriteToIniFile();
    INDEX iCurSel=m_ctrGfxApi.GetCurSel();
    INDEX iOldGfxApi=theApp.m_iApi;
    if( iCurSel!=CB_ERR)
    {
      theApp.m_iApi=m_ctrGfxApi.GetItemData(iCurSel);
      if( iOldGfxApi!=theApp.m_iApi)
      {
        _pGfx->ResetDisplayMode((enum GfxAPIType) theApp.m_iApi);
      }
    }
  }
}


BEGIN_MESSAGE_MAP(CDlgPreferences, CDialog)
  //{{AFX_MSG_MAP(CDlgPreferences)
  ON_BN_CLICKED(ID_SAVE_PREFERENCES, OnSavePreferences)
  ON_BN_CLICKED(ID_LOAD_PREFERENCES, OnLoadPreferences)
  ON_BN_CLICKED(IDC_CHANGE_FULL_SCREEN_DISPLAY_MODE, OnChangeFullScreenDisplayMode)
  ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_CSG_PRECISSION, OnReleasedcaptureCsgPrecission)
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDC_RENDERING_PREFS01, IDC_RENDERING_PREFS10, OnChangePreferences)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences message handlers

BOOL CDlgPreferences::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_ctrlCSGPrecission.SetRange(0, 10);
  m_UndoLevels.SetRange( 0, 10);

  m_ctrGfxApi.ResetContent();
  
  if( _pGfx->HasAPI( GAT_OGL))
  {
    INDEX iAddedAs=m_ctrGfxApi.AddString(_T("OpenGL"));
    m_ctrGfxApi.SetItemData(iAddedAs, GAT_OGL);
  }
#ifdef SE1_D3D
  if( _pGfx->HasAPI( GAT_D3D))
  {
    INDEX iAddedAs=m_ctrGfxApi.AddString(_T("Direct3D"));
    m_ctrGfxApi.SetItemData(iAddedAs, GAT_D3D);
  }
#endif // SE1_D3D

#if SE1_TERRAINS
  m_ctrlTerrainSelectionVisible.ResetContent();
  m_ctrlTerrainSelectionVisible.AddString(_T("Texture"));
  m_ctrlTerrainSelectionVisible.AddString(_T("Wireframe"));
  m_ctrlTerrainSelectionVisible.AddString(_T("Vertices"));
  m_ctrlTerrainSelectionVisible.AddString(_T("None"));

  m_ctrlTerrainSelectionHidden.ResetContent();
  m_ctrlTerrainSelectionHidden.AddString(_T("Texture"));
  m_ctrlTerrainSelectionHidden.AddString(_T("Wireframe"));
  m_ctrlTerrainSelectionHidden.AddString(_T("Vertices"));
  m_ctrlTerrainSelectionHidden.AddString(_T("None"));
#endif

  UpdateData( FALSE);
  return TRUE;
}

void CDlgPreferences::OnChangePreferences(UINT nID)
{
  INDEX iPreferencesBuffer = nID - IDC_RENDERING_PREFS01;
  CDlgRenderingPreferences dlg( iPreferencesBuffer);
  dlg.DoModal();
}

void CDlgPreferences::OnSavePreferences()
{
  UpdateData( TRUE);
  // and write all data that can be written to ini file multiple times
  theApp.WriteToIniFile();
}

void CDlgPreferences::OnLoadPreferences()
{
  // this will load all data that can be readen from ini file multiple times
  theApp.ReadFromIniFile();
  UpdateData( FALSE);
}

void CDlgPreferences::OnChangeFullScreenDisplayMode()
{
  // pass old and get new full screen mode and driver
  _EngineGUI.SelectMode( m_dmFullScreen, m_gatFullScreen);
}

void CDlgPreferences::OnReleasedcaptureCsgPrecission(NMHDR* pNMHDR, LRESULT* pResult) 
{
  *pResult = 0;
  /*
  CTString strCSGPrecission;
  strCSGPrecission.PrintF("CSG Precission: %g", _pShell->GetFLOAT("mth_fCSGEpsilon"));
  m_strCSGPrecission=strCSGPrecission;
  */
  UpdateData(TRUE);
  UpdateData(FALSE);
}
