REM cocos2d-x\tools\cocos2d-console\plugins\plugin_compile 修改 compile_lua_scripts 函数 if not self._project._is_lua_project():   -->    if not 0:
ECHO *********************luacompile*********************
@echo %COCOS_CONSOLE_ROOT% 
:Compile
PUSHD ..\
COPY /y frameworks\.cocos-project.json .\
COPY /y frameworks\.project .\

CALL  cocos run -p android --android -m debug
POPD
IF  ERRORLEVEL 1 GOTO CompileError
PAUSE
EXIT

:CompileError
ECHO *********************cocos compile fail*********************
rem del /p .cocos-project.json
rem del /p .project
pause
exit
