@echo off

SET MSVC_MAKE=nmake.exe
SET MSVC_PATH_1="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
SET MSVC_PATH_2=
for /f "delims=" %%a in ('where %MSVC_MAKE%') do @set MSVC_PATH_2="%%a"

SET MINGW_MAKE=mingw32-make.exe
SET MINGW_PATH_1="C:\Apps\Qt\Tools\mingw730_64\bin\"
SET MINGW_PATH_2="C:\Apps\Qt\Tools\mingw810_64\bin\"
for /f "delims=" %%a in ('where %MINGW_MAKE%') do @set MINGW_PATH_2="%%a"

SET fastcoverage = "C:\Apps\Python3\Scripts\fastcov.exe"


CLS

if [%1]==[] (
	echo Supported compilers:
	IF EXIST %MSVC_PATH_1% echo 1: MSVC in default location
	IF EXIST %MSVC_PATH_2% echo 2: MSVC in PATH
	
	IF EXIST %MINGW_PATH_1% echo 3: MinGW in C:\Apps\Qt\Tools
	IF EXIST %MINGW_PATH_2% echo 4: MinGW in PATH

	SET /p Test= Enter number for the compiler to use: 
	SET /a COMPILER_ID=Test
) else (
	SET /a COMPILER_ID=%1
)

IF NOT %COMPILER_ID% == 0 (
	IF %COMPILER_ID% == 1 (
		call %MSVC_PATH_1%
		SET USE_MAKE=%MSVC_MAKE%
		goto :Compile
	)
	IF %COMPILER_ID% == 2 (
		SET USE_MAKE=%MSVC_MAKE%
		goto :Compile
	)
    IF %COMPILER_ID% == 3 (
		SET USE_MAKE=%MINGW_PATH_1%
		goto :MinGWCompile
	)
    IF %COMPILER_ID% == 4 (
		SET USE_MAKE=%MINGW_PATH_2%
		goto :MinGWCompile
	)
)

:MinGWCompile
setlocal
SET PATH=%USE_MAKE:"=%;%PATH%
SET USE_MAKE=%USE_MAKE%%MINGW_MAKE%

:Compile
IF [%2]==[] (
	SET /p USE_ARG= "Enter Makefile argument (debug, release, clean...): "
) else (
	SET USE_ARG=%2
)

echo Calling:
echo %USE_MAKE% %USE_ARG%
echo.

%USE_MAKE% %USE_ARG%

:END
echo.
echo Ended with errorlevel %ERRORLEVEL%
pause