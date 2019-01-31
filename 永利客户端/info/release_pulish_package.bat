echo *********************luacompile*********************
@echo %COCOS_CONSOLE_ROOT% 

:Compile
PUSHD ..\
COPY /y frameworks\.cocos-project.json .\
COPY /y frameworks\.project .\

CALL  cocos compile -p android -ap 20 --android-studio -m release
POPD
IF  ERRORLEVEL 1 GOTO CompileError
PAUSE
EXIT

:CompileError
echo *********************cocos compile fail*********************
rem del /p .cocos-project.json
rem del /p .project
pause
exit
