@echo off
set _exitStatus=0
set _argcActual=0
set _argcExpected=2

for %%i in (%*) do set /A _argcActual+=1

if %_argcActual% NEQ %_argcExpected% (
  call :_ShowUsage %0%, "Wrong number of arguments"
  set _exitStatus=1
  goto:_EOF
)

set MUSICPATH=%1\_work\Data\Music\AddonWorld

dls2sf %MUSICPATH%\__DLS_Accordion.dls^
  %MUSICPATH%\__DLS_Bass.dls^
  %MUSICPATH%\__DLS_Celli.dls^
  %MUSICPATH%\__DLS_Fiddle.dls^
  %MUSICPATH%\__DLS_Guitar.dls^
  %MUSICPATH%\__DLS_Harp.dls^
  %MUSICPATH%\__DLS_Horn.dls^
  %MUSICPATH%\__DLS_MonoPerc.dls^
  %MUSICPATH%\__DLS_RareIngame.dls^
  %MUSICPATH%\__DLS_Violins.dls -o %2

goto:_EOF

:_ShowUsage
  echo Usage: %~1 [G2 directory] [Output file]
  echo.
  if NOT "%~2" == "" (
    echo %~2
    echo.
  )
  
  goto:eof
:_EOF
echo.
cmd /c exit %_exitStatus%
