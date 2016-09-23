set name=WoG
compiler\pawncc.exe -;+ -(+ sources\%name%.pwn
if exist %name%.amx ^
move %name%.amx gamemodes\
pause