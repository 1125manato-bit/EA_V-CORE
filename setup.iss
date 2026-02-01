[Setup]
AppName=EA V-CORE
AppVersion=1.0.0
AppPublisher=EMU AUDIO
AppPublisherURL=https://emu-audio.com
DefaultDirName={commonpf}\VST3\
DefaultGroupName=EMU AUDIO
OutputBaseFilename=EA V-CORE Installer Win
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Files]
; VST3
Source: "build_windows\EA_V-CORE_artefacts\Release\VST3\EA V-CORE.vst3\*"; DestDir: "{commonpf}\Common Files\VST3\EA V-CORE.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Uninstall EA V-CORE"; Filename: "{uninstallexe}"

[Messages]
WelcomeLabel1=Welcome to [name] Setup
WelcomeLabel2=This will install [name/ver] on your computer.%n%nIt is recommended that you close all other applications before continuing.
