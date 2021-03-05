
#include <stdio.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include <math.h>

#include "LVGA\\lvga.h"
#include "LVGA\\keyb.h"

#define GO_UP 1
#define GO_DOWN 0
#define GO_RIGHT 1
#define GO_LEFT 0

#define BALL_RADIUS 5
#define PL_BAR_SIZE 46
#define PL1_COLOR 49
#define PL2_COLOR 60

int x,y,direction_y,direction_x, page, pl1_score, pl2_score, ball_color;

int last_ball_pos_x[2];
int last_ball_pos_y[2];
int last_pl1_pos_y[2];
int last_pl2_pos_y[2];


void draw_table(){
	rect_fast(5,5, SCREEN_WIDTH - 6, SCREEN_HEIGHT - 6, 8 );
}

void print_score(){
	char p1s[10],p2s[10];
	//	sprintf(s,"%d",ball_delay);
	draw_table();
	sprintf(p1s,"%d",pl1_score);
	sprintf(p2s,"%d",pl2_score);

	//settextjustify(CENTER_TEXT,CENTER_TEXT);
	//setfillstyle(SOLID_FILL,BLACK);
	
	
	//bar( getmaxx()/5*2-10,90,getmaxx()/5*2+10,110);
	//bar( getmaxx()/5*3-10,90,getmaxx()/5*3+10,110);

	//setcolor(GREEN);
	//setcolor(DARKGRAY);
	//outtextxy(MaxX/5*2, 100,p1s);
	//outtextxy(MaxX/5*3, 100,p2s);
}

void draw_ball(int x,int y){
	//erase ball on old possition
	
	circle_fast(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS,0);
	circle_fast(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS - 2,0);
	
	//setcolor(BLACK);
	//circle(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS);
	//circle(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS-2);

	//draw ball
	//setcolor(ball_color);
	//circle(x,y,BALL_RADIUS);
	circle_fast(x,y,BALL_RADIUS,5);
	circle_fast(x,y,BALL_RADIUS - 2,5);
	//circle(x,y,BALL_RADIUS-2);

	last_ball_pos_x[page] = x;
	last_ball_pos_y[page] = y;
}


void draw_pl1_bar(int y){
	// erase bar
	rect_fill(10 ,last_pl1_pos_y[page],15,last_pl1_pos_y[page] + PL_BAR_SIZE, 0);
	// draw bar
	rect_fill(10 ,y,15,y + PL_BAR_SIZE, 49);
	last_pl1_pos_y[page] = y;
}

void draw_pl2_bar(int y){
	// erase bar
	rect_fill(SCREEN_WIDTH - 10 ,last_pl2_pos_y[page],SCREEN_WIDTH - 15,last_pl2_pos_y[page] + PL_BAR_SIZE, 0);
	// draw bar
	rect_fill( SCREEN_WIDTH - 10 ,y,SCREEN_WIDTH - 15,y + PL_BAR_SIZE, 60);
	last_pl2_pos_y[page] = y;
}


void calculate_ball_pos(){
	if(x == (SCREEN_WIDTH- (BALL_RADIUS + 6))){
			direction_x = GO_LEFT;
		}
		if(x == (BALL_RADIUS + 6)){
			direction_x = GO_RIGHT;
		}

		if(y == ( SCREEN_WIDTH - ( BALL_RADIUS + 7)))
            direction_y = GO_UP;
			
        if(y == (BALL_RADIUS + 5))
            direction_y = GO_DOWN;

        if(direction_y == GO_DOWN){
            y = y + 1;
		}else{
            y = y - 1;
		}

		if(direction_x == GO_RIGHT){
			x = x + 1;
		}else{
			x = x - 1;
		}
}


/*									*/
/*	INITIALIZE: Initializes the graphics system and reports 	*/
/*	any errors which occured.					*/
/*									*/

void initialize(void)
{
	set_mode(VGA_256_COLOR_MODE);       /* set the video mode. */
	
	
  //int xasp, yasp;			/* Used to read the aspect ratio*/

  //GraphDriver = DETECT; 		/* Request auto-detection	*/
  //GraphDriver = EGA; GraphMode = 1;		/* Request auto-detection	*/
  //initgraph( &GraphDriver, &GraphMode, "C:\\APP\\TC\\BGI" );
  //initgraph( &GraphDriver, &GraphMode, "BGI" );
  //ErrorCode = graphresult();		/* Read result of initialization*/
  //if( ErrorCode != grOk ){		/* Error occured during init	*/
  //  printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
  //  exit( 1 );
  //}

  //getpalette( &palette );		/* Read the palette from board	*/
  //MaxColors = getmaxcolor() + 1;	/* Read maximum number of colors*/

  //getaspectratio( &xasp, &yasp );	/* read the hardware aspect	*/
  //AspectRatio = (double)xasp / (double)yasp; /* Get correction factor	*/
  
  //MaxX = getmaxx();
  //MaxY = getmaxy();
 }
 
 void exit_graphic(){
	 set_mode(TEXT_MODE); 
 }


int main(){

	int tmp, ball_counter=0,player_counter=0;
	int pl1_y = 20, pl2_y = 20;
	int ball_delay = 160, player_delay = 60;
	
	initialize();

	x=SCREEN_WIDTH/2,y=SCREEN_HEIGHT/2;
    page = 0;
	
	pl1_score = pl2_score = 0;
    direction_y = GO_DOWN;
	direction_x = GO_LEFT;
	ball_color = DARKGRAY;

	last_pl1_pos_y[0] = SCREEN_HEIGHT/2;
	last_pl1_pos_y[1] = SCREEN_HEIGHT/2;;
	last_pl2_pos_y[0] = SCREEN_HEIGHT/2;;
	last_pl2_pos_y[1] = SCREEN_HEIGHT/2;;
		
    ctrlbrk(ctrlbrk_handler);
    hook_keyb_int();
	//setactivepage(page);
    draw_table();
	//setactivepage(1-page);
	draw_table();

	while(1){

		//pl2_y = pl1_y = y - PL_BAR_SIZE / 2;
			
				if(get_key_status(q_Key)){
					 unhook_keyb_int();
					 exit_graphic();
					 return 0;
				}
				
/*				
				if(get_key_status(s_Key)){//s
					ball_delay <= 2 ? ball_delay : ball_delay--;
				}	
				
				if(get_key_status(a_Key)){//a
					ball_delay >= 25 ? ball_delay : ball_delay++;
				}
*/
				
				if(player_counter == player_delay){
					if(get_key_status(l_Key)){ //l
							if(pl2_y < SCREEN_HEIGHT - PL_BAR_SIZE - 9 )
								pl2_y += 1;
					}
					
					if(get_key_status(o_Key)){ //o
							if(pl2_y  >   9  )
								pl2_y -= 1;
						}
						
						
					if(get_key_status(d_Key)){ //d
							if(pl1_y < SCREEN_HEIGHT - PL_BAR_SIZE - 9 )
								pl1_y += 1;
					}
					
					if(get_key_status(e_Key)){ //e
							if(pl1_y  >   9  )
								pl1_y -= 1;
					}
					
					player_counter = 0;
				}
				
		

		if(x + BALL_RADIUS/2 - 1 == 20 ){
			if( y > pl1_y && y < pl1_y + PL_BAR_SIZE){
				pl1_score = pl1_score + 1;
				ball_color = PL1_COLOR;
				//direction_x = 1 - direction_x;
				direction_x = GO_RIGHT;
			}
		}
		
		if(x + BALL_RADIUS/2 - 1 == SCREEN_WIDTH - 20){
			if(y > pl2_y && y < pl2_y + PL_BAR_SIZE){
				pl2_score++;
				ball_color = PL2_COLOR;
				direction_x = GO_LEFT;
			}
		}
			
		if(ball_counter == ball_delay){
			calculate_ball_pos();			
			ball_counter = 0;
		}	
			
		//setactivepage(page);
		//cleardevice();
		//setvisualpage(1-page);
		print_score();
		draw_ball(x,y);
		draw_pl1_bar(pl1_y);
		draw_pl2_bar(pl2_y);
		//page = 1 - page;
		
		ball_counter++;
		player_counter++;
    }
}
