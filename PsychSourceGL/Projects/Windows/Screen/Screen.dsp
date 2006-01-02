# Microsoft Developer Studio Project File - Name="Screen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Screen - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Screen.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Screen.mak" CFG="Screen - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Screen - Win32 Release" (basierend auf  "Win32 (x86) External Target")
!MESSAGE "Screen - Win32 Debug" (basierend auf  "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Screen - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Screen.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Screen.exe"
# PROP BASE Bsc_Name "Screen.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "Screen.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "Screen.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Screen - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Screen.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Screen.exe"
# PROP BASE Bsc_Name "Screen.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "Screen.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "Screen.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Screen - Win32 Release"
# Name "Screen - Win32 Debug"

!IF  "$(CFG)" == "Screen - Win32 Release"

!ELSEIF  "$(CFG)" == "Screen - Win32 Debug"

!ENDIF 

# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\MiniBox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\MODULEVersion.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\ProjectTable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychAlphaBlending.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychAuthors.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychCellGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychError.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\PsychGLGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychHelp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychInit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychInstrument.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychMemory.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychMovieSupport.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\PsychRects.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychRegisterProject.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychScreenGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychScriptingGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychStructGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychTextureSupport.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Base\PsychTimeGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychVersioning.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychWindowGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\PsychWindowSupport.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychWindowTextGlue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\RegisterProject.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenArguments.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENBlendFunction.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENClearTimeList.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENClose.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENCloseAll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENCloseMovie.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENComputer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENCopyWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDontCopyWindowOLD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawDots.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawingFinished.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawLine.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawLines.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawText.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawText_OLD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENDrawTexture.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenExit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFillArc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFillOval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFillPoly.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFillRect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFlip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFrameOval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFramePoly.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENFrameRect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetFlipInterval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetImage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetMouseHelper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetMovieImage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetMovieTimeIndex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGetTimeList.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENglMatrixFunctionWrappers.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENGlobalRect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENglPoint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENgluDisk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENHideCursorHelper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENInitIntrinsicPreferences.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENIsOffscreen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENLineStipple.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENLoadNormalizedGammaTable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENMakeTexture.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENNominalFrameRate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENNull.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENOpenMovie.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENOpenOffscreenWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENOpenWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPixelSize.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPixelSizes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPlayMovie.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPreference.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenPreferenceState.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPreloadTextures.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENPutImage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENReadNormalizedGammaTable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENRect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENScreens.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENSelectStereoDrawBuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENSetGLSynchronous.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENSetMouseHelper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENSetMovieTimeIndex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENShowCursorHelper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\ScreenSurfaces.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenSynopsis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTestStructures.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextBackgroundColor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextBounds.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextColor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextFont.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextMode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextModes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextSize.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTextStyle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTexturizeOffscreenWindowsOld.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENTexturizeWindowOLD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenTypes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENUpdateShadow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENWindowKind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENWindows.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENWindowScreenNumber.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\SCREENWindowSize.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\TimeLists.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\TimeLists.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\WindowBank.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\WindowHelpers.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\..\..\MATLABR11\extern\include\mex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\MiniBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\ProjectTable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\Psych.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychAlphaBlending.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychAuthors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychCellGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychConstants.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychHelp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychIncludes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychInit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychInstrument.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychMovieSupport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Base\PsychPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychPlatformConstants.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\PsychRects.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychRegisterProject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychScreenGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychScriptingGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychStructGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychTextureSupport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Base\PsychTimeGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\PsychVersioning.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychVideoGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychWindowGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\PsychWindowSupport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\windows\Screen\PsychWindowTextGlue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\RegisterProject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\Screen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenArguments.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenPreferenceState.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\ScreenTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Base\TimeLists.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\TimeLists.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\WindowBank.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Common\Screen\WindowHelpers.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
