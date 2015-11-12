;NSIS Modern User Interface
;Welcome/Finish Page Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  
  ; Скрипт для регистрации ассоциаций файлов
  ; Взят отсюда http://nsis.sourceforge.net/File_Association , положить сюда "Program Files\NSIS\Include"
  !include "FileAssociation.nsh"

;--------------------------------
;General

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

  !define pkgdir "files"

  ;Name and file
  Name "КИТ Сценарист"
  Caption "Установка программы написания сценариев"
  OutFile "scenarist-setup.exe"
  BrandingText "DimkaNovikov labs."

  ;Default installation folder
  InstallDir "$PROGRAMFILES\KIT\Scenarist"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

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
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "Spanish"

;--------------------------------
;Installer Sections

Section "App files section" SecFiles

  ; Добавление программы в список установленных программ системы
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "DisplayName" "Сценарист"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "DisplayIcon" "$INSTDIR\Scenarist.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "UninstallString" "$INSTDIR\Uninstall.exe"
  
  ; Исполняемые файлы и плагины
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
  File "${pkgdir}\platforms\qoffscreen.dll"
  File "${pkgdir}\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\printsupport"
  File "${pkgdir}\printsupport\windowsprintersupport.dll"

  SetOutPath "$INSTDIR\sqldrivers"
  File "${pkgdir}\sqldrivers\qsqlite.dll"

  SetOutPath "$INSTDIR"
  File "${pkgdir}\icudt54.dll"
  File "${pkgdir}\icuin54.dll"
  File "${pkgdir}\icuuc54.dll"
  File "${pkgdir}\libeay32.dll"
  File "${pkgdir}\libgcc_s_dw2-1.dll"
  File "${pkgdir}\libstdc++-6.dll"
  File "${pkgdir}\libwinpthread-1.dll"
  File "${pkgdir}\Qt5Core.dll"
  File "${pkgdir}\Qt5Gui.dll"
  File "${pkgdir}\Qt5Network.dll"
  File "${pkgdir}\Qt5PrintSupport.dll"
  File "${pkgdir}\Qt5Sql.dll"
  File "${pkgdir}\Qt5Widgets.dll"
  File "${pkgdir}\Qt5Xml.dll"
  File "${pkgdir}\hunspell.dll"
  File "${pkgdir}\fileformats.dll"
  File "${pkgdir}\Scenarist.exe"
  File "${pkgdir}\ssleay32.dll"
  File "${pkgdir}\webloader.dll"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Регистрируем ассоциации 
  ${registerExtension} "$INSTDIR\Scenarist.exe" ".kitsp" "Проект сценария"
  
  ; Обновляем эксплорер
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'

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

  ; Удалим программу из списка установленных
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist"
  
  ; Удаляем все файлы кроме базы данных
  Delete "$INSTDIR\iconengines\*.*"
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\platforms\*.*"
  Delete "$INSTDIR\printsupport\*.*"
  Delete "$INSTDIR\sqldrivers\*.*"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.ico"

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
  
  ; Отменяем зарегистрированные ассоциации файлов
  ${unregisterExtension} ".kitsp" "Проект сценария"

SectionEnd

Function .onInit

	;Language selection dialog

	InitPluginsDir
	!insertmacro MUI_LANGDLL_DISPLAY
	
FunctionEnd

