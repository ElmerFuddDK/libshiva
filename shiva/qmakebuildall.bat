@ECHO OFF

SET DebugMode=0
SET DebugFlags=
SET StripMode=0
SET StaticMode=0
SET StaticFlags=
SET CleanMode=0
SET QMakeFlags=
SET VsMode=0
SET VsFlags=

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
IF "%1"=="nodebugdll" (
	SET DebugMode=1
	SET Found=1
	SET DebugFlags=CONFIG+=shivanodebugdll
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
IF "%1"=="vs" (
	SET VsMode=vs
	SET Found=1
	SET VsFlags=-tp vc
)
IF "%1"=="nmake" (
	SET VsMode=nmake
	SET Found=1
	IF "%QMAKESPEC%"=="" (
		SET VsFlags=-platform win32-msvc
	) ELSE (
		SET VsFlags=-platform win32-msvc -spec %QMAKESPEC%
	)
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
	ECHO   vs               Compiles with visual studio
	ECHO   nmake            Compiles with nmake and VC++
	GOTO :done
)
SHIFT
GOTO :parseargs
:argsparsed

SET BuildDir=build
IF "%DebugMode%"=="1"  (SET BuildDir=%BuildDir%-debug)
IF "%StaticMode%"=="1" (SET BuildDir=%BuildDir%-static)

IF "%CleanMode%"=="1"  (RMDIR /S /Q %BuildDir% > NUL 2>&1)

IF "%VsMode%"=="vs"  (SET BuildDir=%BuildDir%-vs)
IF "%DebugMode%"=="1" (SET VsConfig=Debug) ELSE (SET VsConfig=Release)

SET StartDir=%CD%

REM Compile the projects
FOR %%G IN ( shivalib,
             shivagui\win32
             shivasqlite,
             shivafreetds,
             shivamodulelibs\dataengine,
             shivamodulelibs\luaengine,
             shivamodulelibs\socketserver,
             shivamodulelibs\subprocess,
             shivamodulelibs\xmlstream
           ) DO (
	CD %%G
	echo Building %%G
	IF "%CleanMode%"=="1"  (
		RMDIR /S /Q %BuildDir% > NUL 2>&1
	)
	MKDIR %BuildDir% > NUL 2>&1
	CD %BuildDir%
	REM For visual studio: qmake -r -tp vc
	qmake -r  %VsFlags% ..\qmake\*.pro %DebugFlags% %StaticFlags% > %StartDir%\log-%BuildDir%-%%~nG.txt 2>&1
	IF "%VsMode%"=="vs" (
		FOR /F %%S IN ('dir *.sln /B /O:-D') DO (
			devenv.exe %%S /rebuild %VsConfig% /out %StartDir%\log-%BuildDir%-%%~nG.txt && DEL %StartDir%\log-%BuildDir%-%%~nG.txt || echo    Build failed
		)
	)
	IF "%VsMode%"=="nmake" (
		nmake.exe %VsConfig% >> %StartDir%\log-%BuildDir%-%%~nG.txt 2>&1 && DEL %StartDir%\log-%BuildDir%-%%~nG.txt || echo    Build failed
	)
	IF "%VsMode%"=="0" (
		mingw32-make.exe -w >> %StartDir%\log-%BuildDir%-%%~nG.txt 2>&1 && DEL %StartDir%\log-%BuildDir%-%%~nG.txt || echo    Build failed
	)
	CD %StartDir%
)

:done
