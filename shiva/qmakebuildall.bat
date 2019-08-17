@ECHO OFF

SET DebugMode=0
SET DebugFlags=
SET StripMode=0
SET StaticMode=0
SET StaticFlags=
SET CleanMode=0
SET QMakeFlags=

:parseargs
IF "%1"=="" (
	GOTO :argsparsed
)
SET Found=0
IF "%1"=="debug" (
	SET DebugMode=1
	SET Found=1
	SET DebugFlags=CONFIG+=debug
)
IF "%1"=="strip" (
	SET StripMode=1
	SET Found=1
)
IF "%1"=="static" (
	SET StaticMode=1
	SET Found=1
	SET StaticFlags=CONFIG+=shivastaticlib
)
IF "%1"=="clean" (
	SET CleanMode=1
	SET Found=1
)
IF "%Found%"=="0" (
	ECHO Usage: qmakebuildall options
	ECHO Options
	ECHO   debug            compile in debug mode
	ECHO   strip            strip debugging symbols
	ECHO   static           compile static libs
	ECHO   clean            cleans before building
	GOTO :done
)
SHIFT
GOTO :parseargs
:argsparsed

SET BuildDir=build
IF "%DebugMode%"=="1"  (SET BuildDir=%BuildDir%-debug)
IF "%StaticMode%"=="1" (SET BuildDir=%BuildDir%-static)

IF "%CleanMode%"=="1"  (RMDIR /S /Q %BuildDir%)

REM Compile the projects
FOR %%G IN ( shivalib,
             shivasqlite,
             shivafreetds,
             shivamodulelibs\dataengine,
             shivamodulelibs\luaengine,
             shivamodulelibs\socketserver,
             shivamodulelibs\subprocess,
             shivamodulelibs\xmlstream
           ) DO (
	SET StartDir=%CD%
	CD %%G
	echo Building %%G
	IF "%CleanMode%"=="1"  (
		RMDIR /S /Q %BuildDir%
	)
	MKDIR %BuildDir%
	CD %BuildDir%
	REM For visual studio: qmake -r -tp vc 
	qmake -r ..\qmake\*.pro %DebugFlags% %StaticFlags%
	mingw32-make.exe -w
	CD %StartDir%
)

:done
