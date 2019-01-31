ECHO *********************PUSLISH RES*********************
SET TEMP_PATH=%temp%\{CB994F32-CC18-4575-8EBB-D99A87768687}
SET LOCAL_PATH=%CD%

IF EXIST %TEMP_PATH%\client RD /S /Q %TEMP_PATH%\client

CALL  cocos luacompile -s ..\client\client -d %TEMP_PATH%\client -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
IF  ERRORLEVEL 1 GOTO CipherSrcError
IF  ERRORLEVEL 0 GOTO MoveRes
PAUSE
EXIT

:CipherSrcError
ECHO luacompile error
PAUSE
EXIT

:MoveRes
MD %TEMP_PATH%\client\res
xcopy /y /e ..\client\client\res %TEMP_PATH%\client\res
echo .lua > uncopy.txt
xcopy /y /e ..\client\client\src\privatemode %TEMP_PATH%\client\src\privatemode /EXCLUDE:uncopy.txt
IF  ERRORLEVEL 0 GOTO CompressPng

:CompressPng
PUSHD %TEMP_PATH%\client
FOR /f "delims=" %%i in ('dir /b /a-d /s *.png') do  (
ECHO compress %%i
	%LOCAL_PATH%\pngquant -f --ext .png --quality 50-80  %%i
)
POPD
IF  ERRORLEVEL 0 GOTO PulishRes
PAUSE
EXIT

:PulishRes

ECHO *********************Publish Packer Package Res*********************
IF EXIST ..\publish\packer\client\src\BaseConfig.* DEL ..\publish\packer\client\src\BaseConfig.*
xcopy /y %TEMP_PATH%\client\src\BaseConfig.* ..\publish\packer\client\src\BaseConfig.*
IF EXIST %TEMP_PATH%\client\src\BaseConfig.* DEL /s /q %TEMP_PATH%\client\src\BaseConfig.*
ECHO *********************Publish Packer MinClient Res*********************

ECHO *********************Publish Packer MD5 Res*********************
MakeMD5List -dst %TEMP_PATH%\client\res -src %TEMP_PATH%\client
ECHO *********************Publish Packer Client Res*********************
PUSHD %TEMP_PATH%
IF EXIST %LOCAL_PATH%\..\publish\packer\client.zip DEL /s /q %LOCAL_PATH%\..\publish\packer\client.zip
CALL WinRAR a -k -r -m5 -hpA0EAFC07A8B9430788CED3BD606CA6E8 %LOCAL_PATH%\..\publish\packer\client.zip client


ECHO *********************Publish HotFix Res*********************
IF EXIST %LOCAL_PATH%\..\publish\hotfix\client RD /s /q %LOCAL_PATH%\..\publish\hotfix\client
xcopy /y /s /e %TEMP_PATH%\client %LOCAL_PATH%\..\publish\hotfix\client\
POPD
IF  ERRORLEVEL 0 GOTO NoError
IF ERRORLEVEL 1 GOTO ZipError
PAUSE
EXIT

:ZipError
ECHO file zip error
PAUSE
EXIT

:NoError
ECHO Finished processing!