del *.exe
del *.obj
del *.swp
del dbg.txt
cls
REM  c:\app\tc\bin\tcc -Ic:\app\tc\include;c:\app\tc\src\pong\lvga -Lc:\app\tc\lib pong_vga.c lvga\keyb.c lvga\lvga.c
c:\app\tc\bin\tcc -ml -Ic:\app\tc\include;c:\app\tc\src\pong\lvga -Lc:\app\tc\lib pongvga2.c lvga\keyb.c lvga\lvga.c
