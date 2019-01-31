@ECHO OFF
ECHO *********************PUSLISH RES*********************
SET TEMP_PATH=..\temp
SET LOCAL_PATH=%CD%

IF EXIST %TEMP_PATH%\game RD /S /Q %TEMP_PATH%\game
IF NOT EXIST %TEMP_PATH%\game MD %TEMP_PATH%\game
ECHO *********************Make File Copy*********************
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (game_list.txt) do (
	echo %%b:[..\%%f] To %TEMP_PATH%\%%f\
	xcopy /y /e /Q ..\%%f %TEMP_PATH%\%%f\
	if  errorlevel 1 goto OnError
)

IF  ERRORLEVEL 1 GOTO CipherSrcError
 IF  ERRORLEVEL 0 GOTO PulishRes
PAUSE
EXIT

:CipherSrcError
ECHO luacompile error
PAUSE
EXIT

:MoveRes
MD %TEMP_PATH%
echo .lua > uncopy.txt
rem game list
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (game_list.txt) do (
	xcopy /y /e /Q ..\%%f\res %TEMP_PATH%\%%f\res\ /EXCLUDE:uncopy.txt
	if  errorlevel 1 goto Error
)

IF  ERRORLEVEL 0 GOTO CompressPng
PAUSE
EXIT

:CompressPng
PUSHD %TEMP_PATH%\game
FOR /f "delims=" %%i in ('dir /b /a-d /s *.png') do  (
ECHO compress %%i
	%LOCAL_PATH%\pngquant -f --ext .png --quality 50-80  %%i
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
	%LOCAL_PATH%\MakeMD5List -dst %%f\res -src %%f  > nul
	if  errorlevel 1 goto Error
)
POPD

ECHO *********************Publish ALL Game Zip*********************
ECHO Doing........
PUSHD %TEMP_PATH%
IF EXIST game.zip del game.zip
CALL rar a -k -r -m5 -hp1516DD50D84048F3AB739BDCBB37D1B9 -inul game.zip game
POPD

ECHO *********************Publish Packer Package Res*********************
PUSHD %TEMP_PATH%
FOR /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a IN (%LOCAL_PATH%\game_list.txt) DO (
	echo %%b:[..\%%f] Doing.....
	PUSHD %%f
	IF EXIST game.zip RD /s /q game.zip
	CALL rar a -k -r -m5 -hp1516DD50D84048F3AB739BDCBB37D1B9 -inul game.zip res src
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
