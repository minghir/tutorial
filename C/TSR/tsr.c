//http://www.dailyfreecode.com/Tutorial_Simple_C_Programming-15/TSR-Programming-225.aspx
#include <dos.h>
#include <conio.h>
#include<stdio.h>

#define e_Key 0x12
#define d_Key 0x20
#define o_Key 0x18
#define l_Key 0x26
#define a_Key 0x1E
#define s_Key 0x1F
#define q_Key 0x10

 void interrupt (*OldKeyboardInterruptFunction)( );
 void interrupt NewKeyboardInterruptFunction( );
FILE *fp;

 int main( )
 {
	 
	 
    OldKeyboardInterruptFunction=getvect(0x09);
    setvect(0x09,NewKeyboardInterruptFunction);

    keep(0,(_SS+(_SP/16)-_ psp));

    return 0;
 }


 /*************************************************************************
 ///-------------------  NewKeyboardInterruptFunction( ) 
 -----------------///*************************************************************************/
 void interrupt NewKeyboardInterruptFunction( )
 {
	 
	  unsigned char rawcode;
	  fp = fopen("dbg.txt","a"); 
	   rawcode = inp(0x60); /* read scancode from keyboard controller */
	   
	   if(rawcode == q_Key) {
			outp(0x20, 0x20);
		    if (OldKeyboardInterruptFunction != NULL) {
				setvect(0x09, OldKeyboardInterruptFunction);
				OldKeyboardInterruptFunction = NULL;
			}
		   _setcursortype(_NORMALCURSOR);
		   fclose(fp);
		   exit(0);
	   }
	   /*
	   if(rawcode == s_Key) {
			//outp(0x60, o_Key);
			fprintf(fp,"AICI2\n");   
	   }
	   */
	 //printf("0x%hhX\n",rawcode);
    //(*OldKeyboardInterruptFunction)( );
    //(*OldKeyboardInterruptFunction)( );
    //(*OldKeyboardInterruptFunction)( );
	outp(0x20, 0x20);
	(*OldKeyboardInterruptFunction)( );
 }
