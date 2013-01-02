::@ECHO OFF

::REM 10 tracs, 16 heads, 63 sectors-per-track = 10080 sectors
SET maxbytes=5160960
SET mbr=..\Release\mbr.img
SET bbp=..\Release\bbp.img
SET output=..\Release\disk.img

..\..\diskutils\Release\buildimg.exe -m %mbr% -b %bbp% -s %maxbytes% %output%

ECHO Cleanup
::DEL %mbr%
::DEL %bbp%

::PAUSE