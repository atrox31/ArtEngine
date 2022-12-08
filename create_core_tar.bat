@echo off
@echo prepare output dir
set outputDir="Core"
rmdir /Q /S %outputDir%
mkdir %outputDir%"

@echo searching for ACompiler.exe
if exist "ACompiler.exe" goto :compiler_found
@echo file ACompiler.exe not found, git clone ArtCompiler and compile a release binary and put in this folder
pause
exit
:compiler_found

@echo searching for AScript.lib
if exist "AScript.lib" goto :lib_found
@echo file AScript.lib not found, git clone ArtCore to download it
pause
exit
:lib_found


@echo prepare binares
copy ACompiler.exe  %outputDir%\ACompiler.exe"
copy AScript.lib  %outputDir%\AScript.lib"
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

rmdir /Q /S %outputDir%
pause