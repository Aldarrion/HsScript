@if not exist Build\cmd\ (
    @mkdir Build\cmd
)

clang -g -O0 -std=c99 -D_CRT_SECURE_NO_WARNINGS -o Build/cmd/ScriptMain.exe -I Script/include Script/src/main.c

@exit /B %errorlevel%
