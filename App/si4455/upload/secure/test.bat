@echo off
set commander="C:\SiliconLabs\SimplicityStudio\v4\developer\adapter_packs\commander\commander.exe"

::%commander% convert bootloader.s37 --secureboot --keyfile sign_key.pem --outfile bootloadr-signed.s37
set device_serialno=--device EFR32BG22C224F512 --serialno 440174378
::%commander% device lock debug --debug disable %device_serialno%
%commander% device info %device_serialno%
pause