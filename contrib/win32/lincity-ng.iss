; LinCity-NG setup script. Process this scrip with InnoSetup
;
; This script assumes that you have prepared a directory that contains all
; files that "jam install" installs in the data\ subdir. You should also place
; files like README and COPYING
; into the doc\ dir.
; The main directory should contain this file the lincity-ng.ico, all .dll
; files and the lincity-ng.exe file.
[Setup]
AppName=LinCity-NG
AppVerName=LinCity-NG 1.1.2
AppPublisher=LinCity-NG Developers
AppPublisherURL=http://lincity-ng.berlios.de/wiki
AppSupportURL=http://lincity-ng.berlios.de/wiki
AppUpdatesURL=http://lincity-ng.berlios.de/wiki
DefaultDirName={pf}\LinCity-NG
DefaultGroupName=LinCity-NG
UninstallDisplayIcon={app}\lincity-ng.ico

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "lincity-ng.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "lincity-ng.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "doc\*.*"; DestDir: "{app}\doc\"; Flags: ignoreversion recursesubdirs
Source: "data\*.*"; DestDir: "{app}\data\"; Flags: ignoreversion recursesubdirs

[INI]
Filename: "{app}\lincity-ng.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://lincity-ng.berlios.de"

[Icons]
Name: "{group}\LinCity-NG"; Filename: "{app}\lincity-ng.exe"; WorkingDir: "{app}"; IconFilename: "{app}\lincity-ng.ico"
Name: "{group}\LinCity-NG on the Web"; Filename: "{app}\lincity-ng.url"
Name: "{group}\Uninstall LinCity-NG"; Filename: "{uninstallexe}"
Name: "{userdesktop}\LinCity-NG"; Filename: "{app}\lincity-ng.exe"; WorkingDir: "{app}"; IconFilename: "{app}\lincity-ng.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\lincity-ng.exe"; Description: "Launch LinCityNG"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\lincity-ng.url"
Type: files; Name: "{app}\stdout.txt"
Type: files; Name: "{app}\stderr.txt"

