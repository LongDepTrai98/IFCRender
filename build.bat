@echo off
set current_drive=%~d0
echo %current_drive%
cd %current_drive%
set current_folder=%~dp0
cd %current_folder%
if not exist build (
    mkdir build
    echo Created build directory
) else (
    echo Build directory already exists
)
echo build app 
cd build 
cmake ../
echo build maplibre
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cd %current_drive% 
cd %current_folder% 
cd maplibre 
if not exist build (
    mkdir build
    echo Created build directory
) else (
    echo Build directory already exists
)
cd build 
cmake ../
pause