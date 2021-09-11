@echo off
call Build.bat

set err=%errorlevel%

if %err% NEQ 0 (
    exit \B 1
)

pushd Data
..\build\cmd\ScriptMain.exe
