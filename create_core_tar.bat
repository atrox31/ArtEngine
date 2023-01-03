@echo off
cls
@echo prepare output dir
set outputDir="Core"

if exist %outputDir% rmdir /Q /S %outputDir%
mkdir %outputDir%"

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
@echo asd
start ArtLibGenerator.exe
copy AScript.lib  %outputDir%\AScript.lib"

@echo prepare binares
mkdir %outputDir%\bin_Release"
mkdir %outputDir%\bin_Debug"
	copy "x64\Release\ArtCore.exe"  %outputDir%\bin_Release\ArtCore.exe"
	copy "x64\Release\*.dll"  %outputDir%\bin_Release"
	
	copy "x64\DebugEditor\ArtCore.exe"  %outputDir%\bin_Debug\ArtCore.exe"
	copy "x64\DebugEditor\*.dll"  %outputDir%\bin_Debug"
		
@echo prepare shaders
mkdir %outputDir%\shaders"
	copy "pack\shaders\*.*"  %outputDir%\shaders"

@echo prepare files
mkdir %outputDir%\pack"
	copy "pack\files\*.*"  %outputDir%\pack"
	
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