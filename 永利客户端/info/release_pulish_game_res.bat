@ECHO OFF
ECHO *********************PUSLISH RES*********************
SET TEMP_PATH=..\temp
SET LOCAL_PATH=%CD%

IF EXIST %TEMP_PATH%\game RD /S /Q %TEMP_PATH%\game

for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (game_list.txt) do (
	echo %%b:Compiling.....
	CALL  cocos luacompile -s ..\%%f -d %TEMP_PATH%\%%f -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
	if  errorlevel 1 goto OnError
)

IF  ERRORLEVEL 1 GOTO CipherSrcError
IF  ERRORLEVEL 0 GOTO MoveRes
PAUSE
EXIT

:CipherSrcError
ECHO luacompile error
PAUSE
EXIT

:MoveRes
ECHO *********************Coping RES*********************
MD %TEMP_PATH%
echo .lua > uncopy.txt
rem game list
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (game_list.txt) do (
	echo %%b:[..\%%f\res] To %TEMP_PATH%\%%f\res
	xcopy /y /e /Q ..\%%f\res %TEMP_PATH%\%%f\res\ /EXCLUDE:uncopy.txt
	if  errorlevel 1 goto Error
)

IF  ERRORLEVEL 0 GOTO CompressPng
PAUSE
EXIT

:CompressPng
PUSHD %TEMP_PATH%\game
ECHO Compressing Png File.....
FOR /f "delims=" %%i in ('dir /b /a-d /s *.png') do  (
	%LOCAL_PATH%\pngquant -f --ext .png --quality 50-80  %%i > nul
)
POPD
IF  ERRORLEVEL 0 GOTO PulishRes
PAUSE
EXIT

:PulishRes
ECHO *********************Publish Game MD5 Res*********************
PUSHD %TEMP_PATH%
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (%LOCAL_PATH%\game_list.txt) do (
	echo %%b:[..\%%f] Doing......
	if EXIST %%f\res\filemd5List.json del %%f\res\filemd5List.json
	%LOCAL_PATH%\MakeMD5List -dst %%f\res -src %%f > nul
	if  errorlevel 1 goto Error
)
POPD

ECHO *********************Publish ALL Game Zip*********************
ECHO Doing........
PUSHD %TEMP_PATH%
IF EXIST game.zip del game.zip
CALL rar a -k -r -m5 -inul -hpA0EAFC07A8B9430788CED3BD606CA6E8 game.zip game
POPD

ECHO *********************Publish Packer Package Res*********************
PUSHD %TEMP_PATH%
FOR /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a IN (%LOCAL_PATH%\game_list.txt) DO (
	echo %%b:[..\%%f] Doing.....
	PUSHD %%f
	IF EXIST game.zip RD /s /q game.zip
	CALL rar a -k -r -m5 -inul -hpA0EAFC07A8B9430788CED3BD606CA6E8 game.zip res src
	POPD
)
POPD
ECHO *********************Publish HotFix Res*********************
ECHO Doing......
IF EXIST %LOCAL_PATH%\..\publish\hotfix\game RD /s /q %LOCAL_PATH%\..\publish\hotfix\game
xcopy /y /s /e /Q %TEMP_PATH%\game %LOCAL_PATH%\..\publish\hotfix\game\
xcopy /y /Q %TEMP_PATH%\game.zip %LOCAL_PATH%\..\publish\packer\
IF  ERRORLEVEL 0 GOTO NoError
IF ERRORLEVEL 1 GOTO ZipError
PAUSE
EXIT

:ZipError
ECHO file zip error
PAUSE
EXIT

:Error
ECHO Pulish Game Res Error!

:NoError
ECHO Finished processing!
