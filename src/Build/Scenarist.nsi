;NSIS Modern User Interface
;Welcome/Finish Page Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  !define pkgdir "files"

  ;Name and file
  Name "Сценарист"
  Caption "Установка программы написания сценариев"
  OutFile "scenarist-setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\KIT\Scenarist"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "Russian"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  ;ADD YOUR OWN FILES HERE...

  SetOutPath "$INSTDIR\iconengines"
  File "${pkgdir}\iconengines\qsvgicon.dll"

  SetOutPath "$INSTDIR\imageformats"
  File "${pkgdir}\imageformats\qgif.dll"
  File "${pkgdir}\imageformats\qico.dll"
  File "${pkgdir}\imageformats\qjpeg.dll"
  File "${pkgdir}\imageformats\qmng.dll"
  File "${pkgdir}\imageformats\qsvg.dll"
  File "${pkgdir}\imageformats\qtga.dll"
  File "${pkgdir}\imageformats\qtiff.dll"
  File "${pkgdir}\imageformats\qwbmp.dll"
  
  SetOutPath "$INSTDIR\platforms"
  File "${pkgdir}\platforms\qminimal.dll"
  File "${pkgdir}\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\printsupport"
  File "${pkgdir}\printsupport\windowsprintersupport.dll"

  SetOutPath "$INSTDIR\sqldrivers"
  File "${pkgdir}\sqldrivers\qsqlite.dll"

  SetOutPath "$INSTDIR"
  File "${pkgdir}\icudt51.dll"
  File "${pkgdir}\icuin51.dll"
  File "${pkgdir}\icuuc51.dll"
  File "${pkgdir}\libgcc_s_dw2-1.dll"
  File "${pkgdir}\libstdc++-6.dll"
  File "${pkgdir}\libwinpthread-1.dll"
  File "${pkgdir}\mingwm10.dll"
  File "${pkgdir}\Qt5Core.dll"
  File "${pkgdir}\Qt5Gui.dll"
  File "${pkgdir}\Qt5PrintSupport.dll"
  File "${pkgdir}\Qt5Sql.dll"
  File "${pkgdir}\Qt5Widgets.dll"
  File "${pkgdir}\Qt5Xml.dll"
  File "${pkgdir}\Scenarist.exe"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\KIT\Scenarist"
  CreateShortCut "$SMPROGRAMS\KIT\Scenarist\Scenarist.lnk" "$INSTDIR\Scenarist.exe"
  CreateShortCut "$SMPROGRAMS\KIT\Scenarist\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  
SectionEnd

Section "Desctop Shortcut"

  CreateShortcut "$DESKTOP\Scenarist.lnk" "$INSTDIR\Scenarist.exe" "" "$INSTDIR\Scenarist.exe" 0
SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ; Удаляем все файлы кроме базы данных
  Delete "$INSTDIR\iconengines\*.*"
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\platforms\*.*"
  Delete "$INSTDIR\printsupport\*.*"
  Delete "$INSTDIR\sqldrivers\*.*"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\KIT\Scenarist\*.*"
  Delete "$DESKTOP\Scenarist.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\KIT\Scenarist"
  RMDir "$INSTDIR\iconengines"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\printsupport"
  RMDir "$INSTDIR\sqldrivers"
  RMDir "$INSTDIR"

SectionEnd
