# Microsoft Developer Studio Project File - Name="nxcon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=nxcon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nxcon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nxcon.mak" CFG="nxcon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nxcon - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "nxcon - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nxcon - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 libnxcl.lib libnetxms.lib libsnmp.lib libnxsnmp.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\..\libnxcl\Release" /libpath:"..\..\libnetxms\Release" /libpath:"..\..\libnxsnmp\Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy files
PostBuild_Cmds=copy Release\nxcon.exe C:\NetXMS\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nxcon - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libnxcl.lib libnetxms.lib libsnmp.lib libnxsnmp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\libnxcl\Debug" /libpath:"..\..\libnetxms\Debug" /libpath:"..\..\libnxsnmp\Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\nxcon.exe ..\..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "nxcon - Win32 Release"
# Name "nxcon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActionEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ActionSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddrChangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentCfgEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentParamSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\comm.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsolePropsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateContainerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateNetSrvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateNodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateTemplateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateTGDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateVPNConnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataCollectionEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\DataExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataQueryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DCIDataView.cpp
# End Source File
# Begin Source File

SOURCE=.\DCIPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DCIThresholdsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DCITransformPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\DeploymentView.cpp
# End Source File
# Begin Source File

SOURCE=.\draw.cpp
# End Source File
# Begin Source File

SOURCE=.\EditActionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditSubnetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditVariableDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\EventEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\EventPolicyEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\EventSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Graph.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphDataPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphSettingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InputBox.cpp
# End Source File
# Begin Source File

SOURCE=.\InternalItemSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LastValuesPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LastValuesView.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MapFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MapView.cpp
# End Source File
# Begin Source File

SOURCE=.\MIBBrowserDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifiedAgentCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NetSrvPropsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\NetSummaryFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\NewActionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewUserDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NodePoller.cpp
# End Source File
# Begin Source File

SOURCE=.\NodePropsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\NodePropsPolling.cpp
# End Source File
# Begin Source File

SOURCE=.\NodeSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\nxcon.cpp
# End Source File
# Begin Source File

SOURCE=.\nxcon.rc
# End Source File
# Begin Source File

SOURCE=.\ObjectBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectInfoBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropCaps.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsPresentation.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsRelations.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsSecurity.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectSearchBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswordChangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoveTemplateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestProcessingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleAlarmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleCommentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleList.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleSeverityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveDesktopDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerCfgEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TableView.cpp
# End Source File
# Begin Source File

SOURCE=.\ThresholdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBox.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\TrapEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrapEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\TrapParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UserEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\UserPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UserSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ValueList.cpp
# End Source File
# Begin Source File

SOURCE=.\VPNCPropsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\WaitView.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActionEditor.h
# End Source File
# Begin Source File

SOURCE=.\ActionSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddrChangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdvSplitter.h
# End Source File
# Begin Source File

SOURCE=.\AgentCfgEditor.h
# End Source File
# Begin Source File

SOURCE=.\AgentParamSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\AlarmBrowser.h
# End Source File
# Begin Source File

SOURCE=.\ColorSelector.h
# End Source File
# Begin Source File

SOURCE=.\ConsolePropsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\ControlPanel.h
# End Source File
# Begin Source File

SOURCE=.\CreateContainerDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateNetSrvDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateNodeDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateTemplateDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateTGDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateVPNConnDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataCollectionEditor.h
# End Source File
# Begin Source File

SOURCE=.\DataExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataQueryDlg.h
# End Source File
# Begin Source File

SOURCE=.\DCIDataView.h
# End Source File
# Begin Source File

SOURCE=.\DCIPropPage.h
# End Source File
# Begin Source File

SOURCE=.\DCIThresholdsPage.h
# End Source File
# Begin Source File

SOURCE=.\DCITransformPage.h
# End Source File
# Begin Source File

SOURCE=.\DebugFrame.h
# End Source File
# Begin Source File

SOURCE=.\DeploymentView.h
# End Source File
# Begin Source File

SOURCE=.\EditActionDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditBox.h
# End Source File
# Begin Source File

SOURCE=.\EditEventDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditSubnetDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditVariableDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventBrowser.h
# End Source File
# Begin Source File

SOURCE=.\EventEditor.h
# End Source File
# Begin Source File

SOURCE=.\EventPolicyEditor.h
# End Source File
# Begin Source File

SOURCE=.\EventSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\Graph.h
# End Source File
# Begin Source File

SOURCE=.\GraphDataPage.h
# End Source File
# Begin Source File

SOURCE=.\GraphFrame.h
# End Source File
# Begin Source File

SOURCE=.\GraphSettingsPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\InputBox.h
# End Source File
# Begin Source File

SOURCE=.\InternalItemSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\LastValuesPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\LastValuesView.h
# End Source File
# Begin Source File

SOURCE=.\LoginDialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MapFrame.h
# End Source File
# Begin Source File

SOURCE=.\MapView.h
# End Source File
# Begin Source File

SOURCE=.\MIBBrowserDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifiedAgentCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\NetSrvPropsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\NetSummaryFrame.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\netxms-version.h"
# End Source File
# Begin Source File

SOURCE=.\NewActionDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewUserDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\nms_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\nms_util.h
# End Source File
# Begin Source File

SOURCE=.\NodePoller.h
# End Source File
# Begin Source File

SOURCE=.\NodePropsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\NodePropsPolling.h
# End Source File
# Begin Source File

SOURCE=.\NodeSummary.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\nxclapi.h
# End Source File
# Begin Source File

SOURCE=.\nxcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\nxsnmp.h
# End Source File
# Begin Source File

SOURCE=.\ObjectBrowser.h
# End Source File
# Begin Source File

SOURCE=.\ObjectInfoBox.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPreview.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropCaps.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsPresentation.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsRelations.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropsSecurity.h
# End Source File
# Begin Source File

SOURCE=.\ObjectSearchBox.h
# End Source File
# Begin Source File

SOURCE=.\ObjectSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\PackageMgr.h
# End Source File
# Begin Source File

SOURCE=.\PasswordChangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\RemoveTemplateDlg.h
# End Source File
# Begin Source File

SOURCE=.\RequestProcessingDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RuleAlarmDlg.h
# End Source File
# Begin Source File

SOURCE=.\RuleCommentDlg.h
# End Source File
# Begin Source File

SOURCE=.\RuleHeader.h
# End Source File
# Begin Source File

SOURCE=.\RuleList.h
# End Source File
# Begin Source File

SOURCE=.\RuleSeverityDlg.h
# End Source File
# Begin Source File

SOURCE=.\SaveDesktopDlg.h
# End Source File
# Begin Source File

SOURCE=.\ServerCfgEditor.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TableView.h
# End Source File
# Begin Source File

SOURCE=.\ThresholdDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolBox.h
# End Source File
# Begin Source File

SOURCE=.\TrapEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\TrapEditor.h
# End Source File
# Begin Source File

SOURCE=.\TrapParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\UserEditor.h
# End Source File
# Begin Source File

SOURCE=.\UserPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\UserSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\ValueList.h
# End Source File
# Begin Source File

SOURCE=.\VPNCPropsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\WaitView.h
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icons\ack.ico
# End Source File
# Begin Source File

SOURCE=.\icons\active.ico
# End Source File
# Begin Source File

SOURCE=.\icons\alarm.ico
# End Source File
# Begin Source File

SOURCE=.\res\any.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\connect.ico
# End Source File
# Begin Source File

SOURCE=.\icons\database.ico
# End Source File
# Begin Source File

SOURCE=.\icons\datacoll.ico
# End Source File
# Begin Source File

SOURCE=.\icons\dctemplate.ico
# End Source File
# Begin Source File

SOURCE=.\icons\deploy.ico
# End Source File
# Begin Source File

SOURCE=.\icons\desktop.ico
# End Source File
# Begin Source File

SOURCE=.\icons\disabled.ico
# End Source File
# Begin Source File

SOURCE=.\res\down_arrow.bmp
# End Source File
# Begin Source File

SOURCE=".\icons\e-mail.ico"
# End Source File
# Begin Source File

SOURCE=.\icons\EARTH.ICO
# End Source File
# Begin Source File

SOURCE=.\icons\editor.ico
# End Source File
# Begin Source File

SOURCE=.\icons\enabled.ico
# End Source File
# Begin Source File

SOURCE=.\icons\event.ico
# End Source File
# Begin Source File

SOURCE=.\icons\exec.ico
# End Source File
# Begin Source File

SOURCE=.\icons\graph.ico
# End Source File
# Begin Source File

SOURCE=.\icons\log.ico
# End Source File
# Begin Source File

SOURCE=.\res\login.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\nack.ico
# End Source File
# Begin Source File

SOURCE=.\icons\NetMap.ICO
# End Source File
# Begin Source File

SOURCE=.\res\network.BMP
# End Source File
# Begin Source File

SOURCE=.\icons\network.ico
# End Source File
# Begin Source File

SOURCE=.\icons\netxms.ico
# End Source File
# Begin Source File

SOURCE=.\res\none.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nxcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\nxcon.rc2
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_critical.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_disabled.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_major.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_minor.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_testing.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_unknown.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_unmanaged.ico
# End Source File
# Begin Source File

SOURCE=.\icons\ovl_status_warning.ico
# End Source File
# Begin Source File

SOURCE=.\icons\package.ico
# End Source File
# Begin Source File

SOURCE=.\icons\passwd.ico
# End Source File
# Begin Source File

SOURCE=.\icons\pending.ico
# End Source File
# Begin Source File

SOURCE=.\res\psym_any.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\question.ico
# End Source File
# Begin Source File

SOURCE=.\icons\rexec.ico
# End Source File
# Begin Source File

SOURCE=.\icons\RuleManager.ico
# End Source File
# Begin Source File

SOURCE=.\icons\running.ico
# End Source File
# Begin Source File

SOURCE=.\icons\setup.ico
# End Source File
# Begin Source File

SOURCE=.\icons\SeverityCritical.ico
# End Source File
# Begin Source File

SOURCE=.\icons\SeverityMajor.ico
# End Source File
# Begin Source File

SOURCE=.\icons\SeverityMinor.ico
# End Source File
# Begin Source File

SOURCE=.\icons\SeverityNormal.ico
# End Source File
# Begin Source File

SOURCE=.\icons\SeverityWarning.ico
# End Source File
# Begin Source File

SOURCE=.\icons\sms.ico
# End Source File
# Begin Source File

SOURCE=.\res\sort_up.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\sort_up.ico
# End Source File
# Begin Source File

SOURCE=.\icons\sortdown.ico
# End Source File
# Begin Source File

SOURCE=.\icons\Tips.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\trap.ico
# End Source File
# Begin Source File

SOURCE=.\icons\tree.ico
# End Source File
# Begin Source File

SOURCE=.\icons\unsuppor.ico
# End Source File
# Begin Source File

SOURCE=.\icons\unsupported.ico
# End Source File
# Begin Source File

SOURCE=.\res\up_arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\user.ico
# End Source File
# Begin Source File

SOURCE=.\icons\users.ico
# End Source File
# End Group
# End Target
# End Project
