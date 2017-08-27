# Microsoft Developer Studio Project File - Name="SparkEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SparkEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SparkEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SparkEdit.mak" CFG="SparkEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SparkEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SparkEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SparkEditPC", ZEAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SparkEdit - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /stack:0x300000 /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "SparkEdit - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /nodefaultlib:"/NODEFAULTLIB:libc"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "SparkEdit - Win32 Release"
# Name "SparkEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AnalyzeDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Analyzer.cpp
# End Source File
# Begin Source File

SOURCE=.\BspManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\CollBsp.cpp
# End Source File
# Begin Source File

SOURCE=.\Encounters.cpp
# End Source File
# Begin Source File

SOURCE=.\Frustum1.cpp
# End Source File
# Begin Source File

SOURCE=.\HaloBmp2.cpp
# End Source File
# Begin Source File

SOURCE=.\HaloMapFile.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpControlsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportScenarioTagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemColl.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGLWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputPane.cpp
# End Source File
# Begin Source File

SOURCE=.\PcModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Perspective.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfileSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Scenario.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SparkEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SparkEdit.rc
# End Source File
# Begin Source File

SOURCE=.\SparkEditDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SparkEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TagManager.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolMultiplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTabEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTabModels.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTabNavigate.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolView.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\VisibleBsp.cpp
# End Source File
# Begin Source File

SOURCE=.\VisibleBspEx.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponHud.cpp
# End Source File
# Begin Source File

SOURCE=.\XboxModel.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AnalyzeDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\Analyzer.h
# End Source File
# Begin Source File

SOURCE=.\basic_types.h
# End Source File
# Begin Source File

SOURCE=.\bsp_defs.h
# End Source File
# Begin Source File

SOURCE=.\BspManager.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\collbsp.h
# End Source File
# Begin Source File

SOURCE=.\Encounters.h
# End Source File
# Begin Source File

SOURCE=.\Frustum1.h
# End Source File
# Begin Source File

SOURCE=.\HaloBmp2.h
# End Source File
# Begin Source File

SOURCE=.\HaloMapFile.h
# End Source File
# Begin Source File

SOURCE=.\HaloStructDefs.h
# End Source File
# Begin Source File

SOURCE=.\HelpControlsDlg.h
# End Source File
# Begin Source File

SOURCE=.\HtmlReport.h
# End Source File
# Begin Source File

SOURCE=.\ImportScenarioTagDlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemColl.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# Begin Source File

SOURCE=.\ModelManager.h
# End Source File
# Begin Source File

SOURCE=.\ObjectInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\OpenGLWnd.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutputPane.h
# End Source File
# Begin Source File

SOURCE=.\PcModel.h
# End Source File
# Begin Source File

SOURCE=.\Perspective.h
# End Source File
# Begin Source File

SOURCE=.\ProfileSettings.h
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scenario.h
# End Source File
# Begin Source File

SOURCE=.\SelectionManager.h
# End Source File
# Begin Source File

SOURCE=.\ShaderManager.h
# End Source File
# Begin Source File

SOURCE=.\SparkEdit.h
# End Source File
# Begin Source File

SOURCE=.\SparkEditDoc.h
# End Source File
# Begin Source File

SOURCE=.\SparkEditView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TagManager.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\ToolMultiplayer.h
# End Source File
# Begin Source File

SOURCE=.\ToolSheet.h
# End Source File
# Begin Source File

SOURCE=.\ToolTabEdit.h
# End Source File
# Begin Source File

SOURCE=.\ToolTabModels.h
# End Source File
# Begin Source File

SOURCE=.\ToolTabNavigate.h
# End Source File
# Begin Source File

SOURCE=.\ToolView.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\VisibleBsp.h
# End Source File
# Begin Source File

SOURCE=.\VisibleBspEx.h
# End Source File
# Begin Source File

SOURCE=.\WeaponHud.h
# End Source File
# Begin Source File

SOURCE=.\XboxModel.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\SparkEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\SparkEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SparkEditDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\c10_bsp0_tagrefs.txt
# End Source File
# Begin Source File

SOURCE=.\c10_bsp5_tagrefs.txt
# End Source File
# Begin Source File

SOURCE=.\new_bitmcode.txt
# End Source File
# Begin Source File

SOURCE=.\zlib\libz.lib
# End Source File
# End Target
# End Project
