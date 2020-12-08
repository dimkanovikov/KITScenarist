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
  Name "KIT Scenarist"
  Caption "Installing application for creating stories"
  OutFile "scenarist-setup.exe"
  BrandingText "DimkaNovikov labs."

  ;Default installation folder
  InstallDir "$PROGRAMFILES\KIT\Scenarist"
  InstallDirRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "UninstallString"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_FINISHPAGE_RUN "$INSTDIR\Scenarist.exe"

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

  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "Hungarian"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Turkish"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "Hebrew"

;--------------------------------
;Installer Sections

Section "App files section" SecFiles

  ; Добавление программы в список установленных программ системы
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "DisplayName" "KIT Scenarist"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "Publisher" "DimkaNovikov labs."
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "DisplayIcon" "$INSTDIR\Scenarist.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scenarist" "UrlInfoAbout" "https://kitscenarist.ru"
  
  SetOutPath "$INSTDIR"
  File /r "${pkgdir}\"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Регистрируем ассоциации 
  ${registerExtension} "$INSTDIR\Scenarist.exe" ".kitsp" "KIT Scenarist project"
  
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
  Delete "$INSTDIR\bearer\*.*"
  Delete "$INSTDIR\iconengines\*.*"
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\platforms\*.*"
  Delete "$INSTDIR\position\*.*"
  Delete "$INSTDIR\printsupport\*.*"
  Delete "$INSTDIR\qtwebengine\*.*"
  Delete "$INSTDIR\sqldrivers\*.*"
  Delete "$INSTDIR\translations\qtwebengine_locales\*.*"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.ico"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.pak"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\KIT\Scenarist\*.*"
  Delete "$DESKTOP\Scenarist.lnk"

  ; Remove directories used
  RMDir "$INSTDIR\bearer"
  RMDir "$INSTDIR\iconengines"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\position"
  RMDir "$INSTDIR\printsupport"
  RMDir "$INSTDIR\qtwebengine"
  RMDir "$INSTDIR\sqldrivers"
  RMDir /r "$INSTDIR\translations"
  RMDir "$INSTDIR"
  RMDir "$SMPROGRAMS\KIT\Scenarist"
  
  ; Отменяем зарегистрированные ассоциации файлов
  ${unregisterExtension} ".kitsp" "KIT Scenarist project"

SectionEnd

Function .onInit

	;Language selection dialog

	InitPluginsDir
	!insertmacro MUI_LANGDLL_DISPLAY
	
FunctionEnd

