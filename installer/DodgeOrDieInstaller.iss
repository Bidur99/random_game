; ----------------------------------------------
;   DODGE OR DIE – Installer Script
; ----------------------------------------------

[Setup]
AppName=Dodge Or Die
AppVersion=1.0
AppPublisher=Group AY
DefaultDirName={pf}\Dodge Or Die
DefaultGroupName=Dodge Or Die
UninstallDisplayIcon={app}\DodgeOrDie.exe
OutputDir=.
OutputBaseFilename=DodgeOrDie_Installer
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Files]
; Main EXE  (CHANGE PATH + EXE NAME IF NEEDED)
Source:"D:\40736653_Project\GameBuild\DodgeOrDie.exe"; DestDir:"{app}"; Flags: ignoreversion

; ALL DLL FILES
Source:"D:\40736653_Project\GameBuild\*.dll"; DestDir:"{app}"; Flags: ignoreversion

; RESOURCES FOLDER (recursive)
Source:"D:\40736653_Project\GameBuild\resources\*"; DestDir:"{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
; Desktop shortcut
Name:"{desktop}\Dodge Or Die"; Filename:"{app}\DodgeOrDie.exe"

; Start menu shortcut
Name:"{group}\Dodge Or Die"; Filename:"{app}\DodgeOrDie.exe"

; Uninstaller shortcut
Name:"{group}\Uninstall Dodge Or Die"; Filename:"{uninstallexe}"

[Run]
Filename:"{app}\DodgeOrDie.exe"; Description:"Launch Dodge Or Die"; Flags: nowait postinstall skipifsilent
