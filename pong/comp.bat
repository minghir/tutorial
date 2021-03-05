del *.exe
del *.obj

c:\app\tc\bin\tcc -lm -Ic:\app\tc\include;c:\app\tc\src\lvga -Lc:\app\tc\lib pong_vga.c lvga\keyb.c lvga\lvga.c 
