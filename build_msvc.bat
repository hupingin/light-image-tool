@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cd /d E:\xxx\tmp\003.WorkBuddyDemos\006.light-image-tool
if not exist bin mkdir bin
cl /nologo /O2 /D_CRT_SECURE_NO_WARNINGS /Isrc src\main.c src\core\image.c src\core\bmp.c src\core\ops.c /Febin\lit.exe
echo BUILD_EXIT=%ERRORLEVEL%
