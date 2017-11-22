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

set MUSICPATH=%1\_work\Data\Music\newworld

dls2sf %MUSICPATH%\DLS_Alpenhorn.dls^
  %MUSICPATH%\DLS_Bass.dls^
  %MUSICPATH%\DLS_Brass.dls^
  %MUSICPATH%\DLS_Daduk.dls^
  %MUSICPATH%\DLS_DragonIsland.dls^
  %MUSICPATH%\DLS_Flute.dls^
  %MUSICPATH%\DLS_Guitar.dls^
  %MUSICPATH%\DLS_Harp.dls^
  %MUSICPATH%\DLS_Metronom.dls^
  %MUSICPATH%\DLS_Organ.dls^
  %MUSICPATH%\DLS_Percussions.dls^
  %MUSICPATH%\DLS_Piano.dls^
  %MUSICPATH%\DLS_Rare.dls^
  %MUSICPATH%\DLS_Strings.dls -o %2

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
