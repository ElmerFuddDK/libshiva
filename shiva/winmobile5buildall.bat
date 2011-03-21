
@echo off

set OLDPATH=%PATH%
PATH=%PATH%;c:\Program Files\Microsoft Visual Studio 8\Common7\IDE

echo Building libshiva
devenv.exe "%CD%\shivalib\winmobile5\libshiva.vcproj" /rebuild Debug
devenv.exe "%CD%\shivalib\winmobile5\libshiva.vcproj" /rebuild Release

echo Building shivagui
devenv.exe "%CD%\shivagui\win32\winmobile5\shivaguiwin32.vcproj" /rebuild Debug
devenv.exe "%CD%\shivagui\win32\winmobile5\shivaguiwin32.vcproj" /rebuild Release

echo Building shivasqlite
devenv.exe "%CD%\shivasqlite\winmobile5\shivasqlite.vcproj" /rebuild Debug
devenv.exe "%CD%\shivasqlite\winmobile5\shivasqlite.vcproj" /rebuild Release

echo Building modules/dataengine
devenv.exe "%CD%\shivamodulelibs\dataengine\winmobile5\dataengine.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\dataengine\winmobile5\dataengine.vcproj" /rebuild Release

echo Building modules/socketserver
devenv.exe "%CD%\shivamodulelibs\socketserver\winmobile5\socketserver.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\socketserver\winmobile5\socketserver.vcproj" /rebuild Release

echo Building modules/xmlstream
devenv.exe "%CD%\shivamodulelibs\xmlstream\winmobile5\xmlstream.vcproj" /rebuild Debug
devenv.exe "%CD%\shivamodulelibs\xmlstream\winmobile5\xmlstream.vcproj" /rebuild Release

PATH=%OLDPATH%
set OLDPATH=
pause
