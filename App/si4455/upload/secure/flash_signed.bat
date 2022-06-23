@echo off

set commander="C:\SiliconLabs\SimplicityStudio\v4\developer\adapter_packs\commander\commander.exe"
set device_serialno=--device EFR32BG22C224F512 --serialno 440174378

echo "Convert bootloader.s37 to bootloader-signed.s37"
%commander% convert bootloader.s37 --secureboot --keyfile sign_key.pem --outfile bootloader-signed.s37

if %errorlevel% neq 0 (
    echo "ERROR: Convert bootloader-signed"
    goto:eof    
)

:: Write encrypt key and gbl decryption pub key
echo "Write encrypt and public token key"
%commander% flash --tokengroup znet --tokenfile encrypt_key.txt --tokenfile sign_token.txt %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: write encrypt and public token key"
    goto:eof
)

:: Write bootloader
echo "Write bootloader-signed"
%commander% flash bootloader-signed.s37 %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: Write bootloader-signed"
    goto:eof
)

:: Write apploader
echo "Write apploader"
%commander% flash apploader-signed.gbl %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: write apploader-signed"
    goto:eof
)

echo "Write application"
%commander% flash application-signed.gbl %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: write application-signed"
    goto:eof
)

::echo Debug lock
::%commander% device lock --debug enable %deivce_seialno%

pause






