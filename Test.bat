@if not exist Build\test\ (
    @mkdir Build\test
)

@if "%~1" == "" (
	echo "Error: First parameter needs to be name of the test to run"
	exit /B 1
)

clang -g -O0 -std=c99 -D_CRT_SECURE_NO_WARNINGS -o Build/test/TestMain.exe -I Script/include Script/test/%1_test.c

@echo off
set err=%errorlevel%

if %err% NEQ 0 (
    exit /B 1
)

pushd Data
..\Build\test\TestMain.exe

popd
