@echo off
ELPRO.CfgGen.Sensor.exe gen -sn grid-eye-sense
if %errorlevel% neq 0 ( goto gen_failure)
"C:\Program Files (x86)\SEGGER\JLink\JLink.exe" -speed 4000 -si SWD -NoGui 1 -commanderscript flash_rf_test.jlink
if %errorlevel% neq 0 ( goto flash_failure)

echo ================= Flash finish ==================
echo Flash success
pause
exit 0;

:gen_failure
echo ================= Flash finish ==================
echo Gen configue: failure
exit 1

:flash_failure
echo ================= Flash finish ==================
echo Flash failure
pause
exit 1