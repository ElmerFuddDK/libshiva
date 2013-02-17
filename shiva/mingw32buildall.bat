
@echo off

SET CurDir=%CD%
IF "%QTDIR%." EQU "." (
  IF EXIST "C:\Qt\2010.03\bin\qtenv.bat" (
    CALL "C:\Qt\2010.03\bin\qtenv.bat"
  )
  ELIF EXIST "C:\Qt\2010.04\bin\qtenv.bat" (
    CALL "C:\Qt\2010.04\bin\qtenv.bat"
  )
)

echo Building libshiva
cd "%CurDir%\shivalib\windows"
mingw32-make.exe -w distclean
qmake libshiva.pro -r CONFIG+=release
mingw32-make.exe -w

echo Building shivagui
cd "%CurDir%\shivagui\win32\windows"
mingw32-make.exe -w distclean
qmake shivaguiwin32.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

echo Building shivasqlite
cd "%CurDir%\shivasqlite\windows"
mingw32-make.exe -w distclean
qmake shivasqlite.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

echo Building modules/dataengine
cd "%CurDir%\shivamodulelibs\dataengine\windows"
mingw32-make.exe -w distclean
qmake dataengine.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

echo Building modules/socketserver
cd "%CurDir%\shivamodulelibs\socketserver\windows"
mingw32-make.exe -w distclean
qmake socketserver.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

echo Building modules/xmlstream
cd "%CurDir%\shivamodulelibs\xmlstream\windows"
mingw32-make.exe -w distclean
qmake xmlstream.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

echo Building modules/luaengine
cd "%CurDir%\shivamodulelibs\luaengine\windows"
mingw32-make.exe -w distclean
qmake luaengine.pro -r CONFIG-=debug CONFIG+=release
mingw32-make.exe -w

cd "%CurDir%"
pause
