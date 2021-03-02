
//An Example of Getting a Key from Keyboard
#include <stdio.h>
#include <dos.h>


char get_key_number () {//return type of the function is char 
char a; 
_AH=0x00; //service number 0x00
geninterrupt(0x16);  //interrupt 0x16
a=_AL; //_AL is the key 
return a;//return the value
}


int key_ready() {//return 1 if a key is ready, 0 otherwise•
	long int x;
	_AH=1;//service number 0x01 
	geninterrupt(0x16);//interrupt 0x16•
	x=_FLAGS;//get flag register•
	if (x&0x40==0) {//if ZF==0 a key is ready•
		return 1;
	}
	else 
		return 0; //else no key
} 

char read_a_key() {//return char if a key is ready•
	if (key_ready()) {
		return get_key_number().x;
	}
	else return 0;
}


int main()
{

	while(1){
		//An Example of Getting a Key from KeyboardRemember to include dos.h in your file.
		char x; int y; 		//declare x to be “char” type
		x = read_a_key();
		printf("The key is: %c\n", x ); //print the key out
		if(x == 27)break; // if escape (ASCII:27) exit
	}
	
getch();
return 0;
}
