
REM /////////////////////////////////////////////////////////////
REM	Android_Build_BAT 2016-03-15 By.CP
REM /////////////////////////////////////////////////////////////

echo *********************luacompile*********************
RD /S /Q ..\client\ciphercode\client
RD /S /Q ..\client\ciphercode\game
RD /S /Q ..\client\ciphercode\base

call  cocos luacompile -s ..\client\client -d ..\client\ciphercode\client -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if  errorlevel 1 goto CipherSrcError
call  cocos luacompile -s ..\client\game -d ..\client\ciphercode\game -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if  errorlevel 1 goto CipherSrcError
call  cocos luacompile -s ..\client\base -d ..\client\ciphercode\base -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if  errorlevel 1 goto CipherSrcError
if  errorlevel 0 goto ZipFile
call  cocos luacompile -s ..\client\command -d ..\client\ciphercode\command -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if  errorlevel 1 goto CipherSrcError
if  errorlevel 0 goto ZipFile
pause
exit

:CipherSrcError
echo luacompile error
pause
exit

:ZipFile

echo *********************file zip*********************
md ..\client\ciphercode\game
md ..\client\ciphercode\client\res
md ..\client\ciphercode\base\res
xcopy /y /e /exclude:uncopy.txt ..\client\game ..\client\ciphercode\game
xcopy /y /e ..\client\client\res ..\client\ciphercode\client\res
xcopy /y /e ..\client\base\res ..\client\ciphercode\base\res
xcopy /y /e /exclude:uncopy.txt ..\client\client\src\privatemode ..\client\ciphercode\client\src\privatemode

echo *********************make md5*********************
call make_md5.bat

PUSHD ..\client\ciphercode
call WinRAR a -k -r -m1  base\res\client.zip client
if errorlevel 1 goto ZipError
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (..\..\info\game_list.txt) do (
	if %%j equ 1 (
		echo rar %%b
		call WinRAR a -k -r -m1  base\res\game.zip game\%%h
		if  errorlevel 1 goto ZipError
	)	
)
POPD
if errorlevel 0 goto Compile
pause
exit

:ZipError
echo file zip error
pause
exit

:Compile
PUSHD ..\
echo *********************cocos compile android*********************

copy /y frameworks\.cocos-project.json .\
copy /y frameworks\.project .\

call  cocos compile -p android -m release -j 4
POPD
if  errorlevel 1 goto CompileError
if  errorlevel 0 goto FinshiMove
pause
exit

:CompileError
echo *********************cocos compile fail*********************
rem del /p .cocos-project.json
rem del /p .project
pause
exit

:FinshiMove
echo *********************file moveto runpath*********************
xcopy /y /e ..\publish\android\*.apk ..\run\release\android\
RD /S /Q publish

if  errorlevel 1 goto FinshiMoveError
if  errorlevel 0 goto FINISH
echo *********************FINISH*********************
rem del /p .cocos-project.json
rem del /p .project

:FinshiMoveError
echo *********************file moveto fail*********************
rem del /p .cocos-project.json
rem del /p .project
pause
exit
