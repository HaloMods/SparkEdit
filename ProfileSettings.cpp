// ProfileSettings.cpp: implementation of the CProfileSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "ProfileSettings.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*-------------------------------------------------------------------
 * Name: CProfileSettings()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CProfileSettings::CProfileSettings()
{

}

/*-------------------------------------------------------------------
 * Name: ~CProfileSettings()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CProfileSettings::~CProfileSettings()
{

}

/*-------------------------------------------------------------------
 * Name: GetBitmapPath()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CString CProfileSettings::GetBitmapPath()
{
  return(m_BinProfile.BitmapPath);
}
CString CProfileSettings::GetCEBitmapPath()
{
  return(m_BinProfile.CEBitmapPath);
}


/*-------------------------------------------------------------------
 * Name: LoadBinaryProfile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::LoadBinaryProfile()
{
  CString init_file;
  BOOL bCreateNewProfile = TRUE;
  init_file = m_WorkingDir + "\\sparkedit.dat";

  if(m_NewPers.Open(init_file, CFile::modeRead))
  {
    m_NewPers.Read(&m_BinProfile, sizeof(m_BinProfile));

    //check for correct profile version
    if(m_BinProfile.ProfileVersion == 1)
      bCreateNewProfile = FALSE;

    m_NewPers.Close();
  }

  if(bCreateNewProfile)
  {
    //use default values
    //Get install path from the registry
    CRegistry registry;

    if(registry.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Microsoft Games\\Halo"))
    {
      CString HpcPath;
      registry.Read("EXE Path", HpcPath);
      registry.Close();
      HpcPath += "\\maps\\bitmaps.map";

      strncpy(m_BinProfile.BitmapPath, HpcPath.GetBuffer(), sizeof(m_BinProfile.BitmapPath)-1);
      HpcPath.ReleaseBuffer();
    }

    if(registry.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Microsoft Games\\Halo CE"))
    {
      CString HcePath;
      registry.Read("EXE Path", HcePath);
      registry.Close();
      HcePath += "\\maps\\bitmaps.map";

      strncpy(m_BinProfile.CEBitmapPath, HcePath.GetBuffer(), sizeof(m_BinProfile.CEBitmapPath)-1);
      HcePath.ReleaseBuffer();
    }

    m_BinProfile.ProfileVersion = 1;



    //strcpy(m_BinProfile.BitmapPath, "C:\\Program Files\\Microsoft Games\\Halo\\MAPS\\bitmaps.map");
    //strcpy(m_BinProfile.CEBitmapPath, "C:\\Program Files\\Microsoft Games\\HaloCE\\MAPS\\bitmaps.map");
    
    m_BinProfile.VehicleSel.HighDetailDrawDist = 50;
    m_BinProfile.VehicleSel.bShowSelection = TRUE;
    
    m_BinProfile.ScenerySel.HighDetailDrawDist = 50;
    m_BinProfile.ScenerySel.bShowSelection = TRUE;

    m_BinProfile.WeaponSel.HighDetailDrawDist = 50;
    m_BinProfile.WeaponSel.bShowSelection = TRUE;

    m_BinProfile.EquipSel.HighDetailDrawDist = 50;
    m_BinProfile.EquipSel.bShowSelection = TRUE;
    
    m_BinProfile.MachinesSel.HighDetailDrawDist = 50;
    m_BinProfile.MachinesSel.bShowSelection = TRUE;
    
    m_BinProfile.BipedsSel.HighDetailDrawDist = 10;
    m_BinProfile.BipedsSel.bShowSelection = TRUE;
    
    m_BinProfile.CullDistance = 100.0f;
    m_BinProfile.bInvertMouseLook = FALSE;

    m_BinProfile.InfoDlgRect.top = 150;
    m_BinProfile.InfoDlgRect.bottom = 600;
    m_BinProfile.InfoDlgRect.left = 150;
    m_BinProfile.InfoDlgRect.right = 600;

    SaveBinaryProfile();
  }
}

/*-------------------------------------------------------------------
 * Name: SaveBinaryProfile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::SaveBinaryProfile()
{
  CString init_file;
  init_file = m_WorkingDir + "\\sparkedit.dat";

  if(m_NewPers.Open(init_file, CFile::modeWrite|CFile::modeCreate))
  {
    m_NewPers.Write(&m_BinProfile, sizeof(m_BinProfile));
    m_NewPers.Close();
  }
  else
  {
    AfxMessageBox("Error saving 'sparkedit.dat' (initialization settings)");
  }
}

/*-------------------------------------------------------------------
 * Name: SetInfoDlgPosition()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::SetInfoDlgPosition(RECT rc)
{
  memcpy(&m_BinProfile.InfoDlgRect, &rc, sizeof(RECT));
}

/*-------------------------------------------------------------------
 * Name: GetInfoDlgPosition()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::GetInfoDlgPosition(RECT *pRect)
{
  memcpy(pRect, &m_BinProfile.InfoDlgRect, sizeof(RECT));
}

/*-------------------------------------------------------------------
 * Name: GetProfile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::GetProfile(PROFILE_SETTINGS *pProfile)
{
  memcpy(pProfile, &m_BinProfile, sizeof(m_BinProfile));
}

/*-------------------------------------------------------------------
 * Name: SetProfile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CProfileSettings::SetProfile(PROFILE_SETTINGS *pProfile)
{
  memcpy(&m_BinProfile.BitmapPath, pProfile->BitmapPath, 255);
  memcpy(&m_BinProfile.CEBitmapPath, pProfile->CEBitmapPath, 255);
  m_BinProfile.VehicleSel.HighDetailDrawDist = pProfile->VehicleSel.HighDetailDrawDist;
  m_BinProfile.ScenerySel.HighDetailDrawDist = pProfile->ScenerySel.HighDetailDrawDist;
  m_BinProfile.WeaponSel.HighDetailDrawDist = pProfile->WeaponSel.HighDetailDrawDist;
  m_BinProfile.EquipSel.HighDetailDrawDist = pProfile->EquipSel.HighDetailDrawDist;
  m_BinProfile.MachinesSel.HighDetailDrawDist = pProfile->MachinesSel.HighDetailDrawDist;
  m_BinProfile.BipedsSel.HighDetailDrawDist = pProfile->BipedsSel.HighDetailDrawDist;
  m_BinProfile.CullDistance = pProfile->CullDistance;
  m_BinProfile.bInvertMouseLook = pProfile->bInvertMouseLook;

  SaveBinaryProfile();
}

float CProfileSettings::GetCullDistance()
{
  return(m_BinProfile.CullDistance);
}
