@ECHO OFF
ECHO *********************PUSLISH RES*********************
SET TEMP_PATH=..\temp
SET LOCAL_PATH=%CD%

IF EXIST %TEMP_PATH%\client RD /S /Q %TEMP_PATH%\client
IF NOT EXIST %TEMP_PATH%\client mkdir %TEMP_PATH%\client

ECHO *****************COPY CLIENT TO TEMP DIR*****************
ECHO Doing....
xcopy /y /e /Q ..\client\*.* %TEMP_PATH%\client\
IF  ERRORLEVEL 1 GOTO CipherSrcError
IF  ERRORLEVEL 0 GOTO PulishRes
PAUSE
EXIT

:CipherSrcError
ECHO luacompile error
PAUSE
EXIT

:MoveRes
MD %TEMP_PATH%\client\res
xcopy /y /e ..\client\res %TEMP_PATH%\client\res
echo .lua > uncopy.txt
xcopy /y /e ..\client\src\privatemode %TEMP_PATH%\client\src\privatemode /EXCLUDE:uncopy.txt
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
xcopy /y /Q %TEMP_PATH%\client\src\BaseConfig.* ..\publish\packer\client\src\BaseConfig.*
IF EXIST %TEMP_PATH%\client\src\BaseConfig.* DEL /s /q %TEMP_PATH%\client\src\BaseConfig.*


ECHO *********************Publish Packer Base Src*********************
PUSHD %TEMP_PATH%
IF EXIST %LOCAL_PATH%\..\publish\packer\base.zip DEL /s /q %LOCAL_PATH%\..\publish\packer\base.zip
ECHO Packing base.zip To ..\publish\packer\base.zip
CALL rar a -k -r -m5 -inul -hp1516DD50D84048F3AB739BDCBB37D1B9 %LOCAL_PATH%\..\publish\packer\base.zip client\res\base client\src\app client\src\cocos client\src\packages client\src\controller client\src\framework client\src\model client\src\view client\src\config.* client\src\LuaDebug.* client\src\LuaDebugjit.* client\src\main.* client\src\AppFacade.* client\src\init.*
POPD

ECHO *********************Publish Packer MD5 Res*********************
ECHO Generate Client MD5 File List......
MakeMD5List -dst %TEMP_PATH%\client\res -src %TEMP_PATH%\client > nul

ECHO *********************Publish Packer Client Res*********************
PUSHD %TEMP_PATH%
IF EXIST %LOCAL_PATH%\..\publish\packer\client.zip DEL /s /q %LOCAL_PATH%\..\publish\packer\client.zip
ECHO Packing client.zip To \publish\packer\client.zip....
CALL rar a -k -r -m5 -inul -hp1516DD50D84048F3AB739BDCBB37D1B9 %LOCAL_PATH%\..\publish\packer\client.zip client
CALL rar d -inul -hp1516DD50D84048F3AB739BDCBB37D1B9 %LOCAL_PATH%\..\publish\packer\client.zip client\res\base client\src\app client\src\cocos client\src\packages client\src\controller client\src\framework client\src\model client\src\view client\src\config.* client\src\LuaDebug.* client\src\LuaDebugjit.* client\src\main.* client\src\AppFacade.* client\src\init.*

ECHO *********************Publish HotFix Res*********************
ECHO Copy Hotfix File To ..\publish\hotfix\client
IF EXIST %LOCAL_PATH%\..\publish\hotfix\client RD /s /q %LOCAL_PATH%\..\publish\hotfix\client
xcopy /y /s /e /Q %TEMP_PATH%\client %LOCAL_PATH%\..\publish\hotfix\client\
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