@echo off
cls
@echo prepare output dir
set outputDir="Core

if exist "%outputDir% rmdir /Q /S "%outputDir%
mkdir %outputDir%"
mkdir %outputDir%\StandardBehaviourTemplates"
mkdir %outputDir%\SceneTemplates"

@echo searching for ACompiler.exe
if exist "ACompiler.exe" goto :compiler_found
if exist "..\ACompiler\x64\Release\ACompiler.exe" goto :compiler_found_folder
	@echo file ACompiler.exe not found, git clone ArtCompiler and compile a release binary and put in this folder
	pause
	exit

:compiler_found
copy ACompiler.exe  %outputDir%\ACompiler.exe"
goto :search_for_ascript
:compiler_found_folder
copy ..\ACompiler\x64\Release\ACompiler.exe  %outputDir%\ACompiler.exe"


:search_for_ascript
@echo searching for AScript.lib
if exist "ArtLibGenerator.exe" goto :lib_found
	@echo file AScript.lib not found, git clone ArtCore to download it
	pause
	exit
	
:lib_found
start ArtLibGenerator.exe
copy AScript.lib  %outputDir%\AScript.lib"

if not exist "x64\Release_windows\ArtCore.exe" goto :win_skip
@echo prepare windows binares
mkdir %outputDir%\windows_bin_Release"
mkdir %outputDir%\windows_bin_Debug"
	copy "x64\Release_windows\ArtCore.exe"  %outputDir%\windows_bin_Release\ArtCore.exe"
	copy "x64\Release_windows\*.dll"  %outputDir%\windows_bin_Release"
	
	copy "x64\DebugEditor_windows\ArtCore.exe"  %outputDir%\windows_bin_Debug\ArtCore.exe"
	copy "x64\DebugEditor_windows\*.dll"  %outputDir%\windows_bin_Debug"
:win_skip	

if not exist "x64\Release_linux\ArtCore.exe" goto :lin_skip
@echo prepare linux binares
mkdir %outputDir%\linux_bin_Release"
mkdir %outputDir%\linux_bin_Debug"
	copy "x64\Release_linux\ArtCore.exe"  %outputDir%\linux_bin_Release\ArtCore.exe"
	copy "x64\Release_linux\*.dll"  %outputDir%\linux_bin_Release"
	
	copy "x64\DebugEditor_linux\ArtCore.exe"  %outputDir%\linux_bin_Debug\ArtCore.exe"
	copy "x64\DebugEditor_linux\*.dll"  %outputDir%\linux_bin_Debug"
:lin_skip	

@echo prepare macos binares
if not exist "x64\Release_macos\ArtCore.exe" goto :macos_skip
mkdir %outputDir%\macos_bin_Release"
mkdir %outputDir%\macos_bin_Debug"
	copy "x64\Release_macos\ArtCore.exe"  %outputDir%\macos_bin_Release\ArtCore.exe"
	copy "x64\Release_macos\*.dll"  %outputDir%\macos_bin_Release"
	
	copy "x64\DebugEditor_macos\ArtCore.exe"  %outputDir%\macos_bin_Debug\ArtCore.exe"
	copy "x64\DebugEditor_macos\*.dll"  %outputDir%\macos_bin_Debug"
:macos_skip	

@echo prepare android binares
if not exist "x64\Release_android\ArtCore.exe" goto :and_skip
mkdir %outputDir%\android_bin_Release"
mkdir %outputDir%\android_bin_Debug"
	copy "x64\Release_android\ArtCore.exe"  %outputDir%\android_bin_Release\ArtCore.exe"
	copy "x64\Release_android\*.dll"  %outputDir%\android_bin_Release"
	
	copy "x64\DebugEditor_android\ArtCore.exe"  %outputDir%\android_bin_Debug\ArtCore.exe"
	copy "x64\DebugEditor_android\*.dll"  %outputDir%\android_bin_Debug"
:and_skip	
	
@echo prepare shaders
mkdir %outputDir%\shaders"
	copy "pack\shaders\*.*"  %outputDir%\shaders"

@echo prepare files
mkdir %outputDir%\pack"
	copy "pack\files\*.*"  %outputDir%\pack"

@echo StandardBehaviourTemplates
copy "StandardBehaviourTemplates\*.*"  %outputDir%\StandardBehaviourTemplates"

@echo SceneTemplates
copy "SceneTemplates\*.*"  %outputDir%\SceneTemplates"
	
@echo prepare FileList	
cd %outputDir%"
ls -R > FileList.txt		

cd ..
if exist "Core.tar" goto :skip_del
del /Q "Core.tar"

:skip_del
tar -cf "Core.tar" "%outputDir%"

if exist "Core.tar" goto :okkk
@echo error while create Core.tar
pause
exit
:okkk
echo File Core.tar created succesfull

pause