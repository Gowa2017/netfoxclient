@echo off
set PARAM=%1

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
echo *********************file copy*********************
md ..\client\ciphercode\game
md ..\client\ciphercode\client\res
md ..\client\ciphercode\base\res
xcopy /y /e /exclude:uncopy.txt ..\client\game ..\client\ciphercode\game
xcopy /y /e ..\client\client\res ..\client\ciphercode\client\res
xcopy /y /e ..\client\base\res ..\client\ciphercode\base\res
xcopy /y /e /exclude:uncopy.txt ..\client\client\src\privatemode ..\client\ciphercode\client\src\privatemode

PUSHD ..\info
echo *********************make md5*********************
call make_md5.bat
POPD

echo *********************file zip*********************
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
if errorlevel 0 goto FINISH
pause
exit

:ZipError
echo file zip error
pause
exit

:FINISH 
echo *********************zip finish*********************
if defined PARAM (echo finish) else pause
