
@echo off

set OLDPATH=%PATH%
PATH=%PATH%;c:\Program Files\Microsoft Visual Studio 8\Common7\IDE

echo Building libshiva
devenv.exe "%CD%\shivalib\windows\libshiva.vcproj" /rebuild Debug
devenv.exe "%CD%\shivalib\windows\libshiva.vcproj" /rebuild Release

echo Building shivagui
devenv.exe "%CD%\shivagui\win32\windows\shivaguiwin32.vcproj" /rebuild Debug
devenv.exe "%CD%\shivagui\win32\windows\shivaguiwin32.vcproj" /rebuild Release

echo Building shivasqlite
devenv.exe "%CD%\shivasqlite\windows\shivasqlite.vcproj" /rebuild Debug
devenv.exe "%CD%\shivasqlite\windows\shivasqlite.vcproj" /rebuild Release

echo Building modules/dataengine
devenv.exe "%CD%\shivamodulelibs\dataengine\windows\dataengine.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\dataengine\windows\dataengine.vcproj" /rebuild Release

echo Building modules/socketserver
devenv.exe "%CD%\shivamodulelibs\socketserver\windows\socketserver.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\socketserver\windows\socketserver.vcproj" /rebuild Release

echo Building modules/xmlstream
devenv.exe "%CD%\shivamodulelibs\xmlstream\windows\xmlstream.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\xmlstream\windows\xmlstream.vcproj" /rebuild Release

echo Building modules/luaengine
devenv.exe "%CD%\shivamodulelibs\luaengine\windows\luaengine.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\luaengine\windows\luaengine.vcproj" /rebuild Release

PATH=%OLDPATH%
set OLDPATH=
pause
