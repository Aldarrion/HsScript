@if not exist Build\cmd\ (
    @mkdir Build\cmd
)

@pushd "Script/src"
@set objs=
@for /R %%f in (*.c) do @call set objs=%%objs%% %%f
@popd

clang -g -O0 -std=c99 -D_CRT_SECURE_NO_WARNINGS -o Build/cmd/ScriptMain.exe -I Script/include %objs%

@exit /B %errorlevel%
