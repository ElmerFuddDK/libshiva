@echo off

if exist "..\..\..\shivalib\windows\debug\libshiva.a"; xcopy /D /Y "..\..\..\shivalib\windows\debug\libshiva.a" ".\"
if exist "..\..\..\shivalib\windows\debug\libshiva.dll"; xcopy /D /Y "..\..\..\shivalib\windows\debug\libshiva.dll" ".\"


if exist "..\..\..\shivalib\windows\release\libshiva.a"; xcopy /D /Y "..\..\..\shivalib\windows\release\libshiva.a" ".\"
if exist "..\..\..\shivalib\windows\release\libshiva.dll"; xcopy /D /Y "..\..\..\shivalib\windows\release\libshiva.dll" ".\"
