
//An Example of Getting a Key from Keyboard
#include <stdio.h>
#include <dos.h>
int main()
{

	while(1){
		//An Example of Getting a Key from KeyboardRemember to include dos.h in your file.
		char x; int y; 		//declare x to be “char” type
		_AH=0x0; 			//choose service 0x0 with _AH
		geninterrupt(0x16); //invoke interrupt 0x16
		x=_AL;//get ASCII code of key from _AL
		printf("The key is: %c\n", x ); //print the key out
		if(x == 27)break; // if escape (ASCII:27) exit
	}
	
getch();
return 0;
}
