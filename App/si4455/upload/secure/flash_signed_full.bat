@echo off

set commander="C:\SiliconLabs\SimplicityStudio\v4\developer\adapter_packs\commander\commander.exe"
set device_serialno=--device EFR32BG22C224F512 --serialno 440174378

echo "Convert bootloader.s37 to bootloader-signed.s37"
%commander% convert bootloader.s37 --secureboot --keyfile sign_key.pem --outfile bootloader-signed.s37

if %errorlevel% neq 0 (
    echo "ERROR: Convert bootloader-signed"
    goto:eof    
)

:: Write sign_pubkey.pem to MCU
echo "Write sign_pubkey"
%commander% writekey --sign sign_pubkey.pem %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: Write sign_pubkey.pem"
    goto:eof
)

:: Write secure configure
echo "Write secure configure"
%commander% security writeconfig --configfile user_configuration.json %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: Write user_configuration.json file"
    goto:eof
)

echo "Confirm writ user_configuration"
continue

:: Type continue to confirm OTP write to MCU
echo "Confirm write sign_pubkey"
continue

:: mass erase device
echo "Mass erase device"
%commander% security erasedevice %device_serialno%
if %errorlevel% neq 0 (
    echo "ERROR: Erase device"
    goto:eof
)

:: Write encrypt key and gbl decryption pub key
echo "Write encrypt and public token key"
%commander% flash --tokengroup znet --tokefile encrypt_key.txt --tokenfile sign_token.txt %device_serialno%
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

:: Enable secure debug and device erase: disable if production.

pause






