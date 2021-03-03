#include <stdio.h>
#include <graphics.h>
#include <dos.h>
#include <time.h>
#include <conio.h>

#define GO_UP 1
#define GO_DOWN 0
#define GO_RIGHT 1
#define GO_LEFT 0

#define BALL_RADIUS 5
#define PL_BAR_SIZE 46
#define PL1_COLOR LIGHTGREEN
#define PL2_COLOR LIGHTBLUE

#define e_Key 0x12
#define d_Key 0x20
#define o_Key 0x18
#define l_Key 0x26
#define a_Key 0x1E
#define s_Key 0x1F
#define q_Key 0x10


int    GraphDriver;		/* The Graphics device driver		*/
int    GraphMode;		/* The Graphics mode value		*/
double AspectRatio;		/* Aspect ratio of a pixel on the screen*/
int    MaxX, MaxY;		/* The maximum resolution of the screen */
int    MaxColors;		/* The maximum # of colors available	*/
int    ErrorCode;		/* Reports any graphics errors		*/
struct palettetype palette;		/* Used to read palette info	*/
//struct viewporttype vp;



unsigned char normal_keys[0x60];
unsigned char extended_keys[0x60];

int x,y,direction_y,direction_x, page, pl1_score, pl2_score, ball_color;

int last_ball_pos_x[2];
int last_ball_pos_y[2];
int last_pl1_pos_y[2];
int last_pl2_pos_y[2];

static void interrupt keyb_int() {
    static unsigned char buffer;
    unsigned char rawcode;
    unsigned char make_break;
    int scancode;

    rawcode = inp(0x60); /* read scancode from keyboard controller */
    make_break = !(rawcode & 0x80); /* bit 7: 0 = make, 1 = break */
    scancode = rawcode & 0x7F;

    if (buffer == 0xE0) { /* second byte of an extended key */
        if (scancode < 0x60) {
            extended_keys[scancode] = make_break;
        }
        buffer = 0;
    } else if (buffer >= 0xE1 && buffer <= 0xE2) {
        buffer = 0; /* ingore these extended keys */
    } else if (rawcode >= 0xE0 && rawcode <= 0xE2) {
        buffer = rawcode; /* first byte of an extended key */
    } else if (scancode < 0x60) {
        normal_keys[scancode] = make_break;
    }

    outp(0x20, 0x20); /* must send EOI to finish interrupt */
}

static void interrupt (*old_keyb_int)();

void hook_keyb_int(void) {
    old_keyb_int = getvect(0x09);
    setvect(0x09, keyb_int);
}

void unhook_keyb_int(void) {
    if (old_keyb_int != NULL) {
        setvect(0x09, old_keyb_int);
        old_keyb_int = NULL;
    }
}

int ctrlbrk_handler(void) {
    unhook_keyb_int();
    _setcursortype(_NORMALCURSOR);
    return 0;
}

static get_key_status(unsigned char ch){ // 1 pressed  
	return normal_keys[ch];
}

void draw_table(){
	setcolor(DARKGRAY);
	rectangle(5,5,MaxX - 5, MaxY - 5);
}

void print_score(){
	char p1s[10],p2s[10];
	//	sprintf(s,"%d",ball_delay);
	draw_table();
	sprintf(p1s,"%d",pl1_score);
	sprintf(p2s,"%d",pl2_score);

	settextjustify(CENTER_TEXT,CENTER_TEXT);
	setfillstyle(SOLID_FILL,BLACK);
	//bar( getmaxx()/5*2-10,90,getmaxx()/5*2+10,110);
	//bar( getmaxx()/5*3-10,90,getmaxx()/5*3+10,110);
	
	bar( MaxX/5*2-10,90,MaxX/5*2+10,110);
	bar( MaxX/5*3-10,90,MaxX/5*3+10,110);

	//setcolor(GREEN);
	setcolor(DARKGRAY);
	outtextxy(MaxX/5*2, 100,p1s);
	outtextxy(MaxX/5*3, 100,p2s);
}

void draw_ball(int x,int y){
	//erase ball on old possition
	setcolor(BLACK);
	circle(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS);
	circle(last_ball_pos_x[page],last_ball_pos_y[page],BALL_RADIUS-2);

	//draw ball
	setcolor(ball_color);
	circle(x,y,BALL_RADIUS);
	circle(x,y,BALL_RADIUS-2);

	last_ball_pos_x[page] = x;
	last_ball_pos_y[page] = y;
}


void draw_pl1_bar(int y){
	// erase bar
	setfillstyle(SOLID_FILL,BLACK);
	bar( 10 ,last_pl1_pos_y[page],15,last_pl1_pos_y[page] + PL_BAR_SIZE);
	// draw bar
	setfillstyle(SOLID_FILL,PL1_COLOR);
	bar( 10 ,y,15,y + PL_BAR_SIZE);
	last_pl1_pos_y[page] = y;
}

void draw_pl2_bar(int y){
	// erase bar
	setfillstyle(SOLID_FILL,BLACK);
	bar( getmaxx() - 10 ,last_pl2_pos_y[page],getmaxx() - 15,last_pl2_pos_y[page] + PL_BAR_SIZE);
	// draw bar
	setfillstyle(SOLID_FILL,PL2_COLOR);
	bar( getmaxx()-10 ,y,getmaxx()-15,y + PL_BAR_SIZE);
	last_pl2_pos_y[page] = y;
}


void calculate_ball_pos(){
	if(x == (getmaxx()- (BALL_RADIUS + 6))){
			direction_x = GO_LEFT;
		}
		if(x == (BALL_RADIUS + 6)){
			direction_x = GO_RIGHT;
		}

		if(y == ( getmaxy() - ( BALL_RADIUS + 5)))
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
  int xasp, yasp;			/* Used to read the aspect ratio*/

  GraphDriver = DETECT; 		/* Request auto-detection	*/
  //GraphDriver = EGA; GraphMode = 1;		/* Request auto-detection	*/
  //initgraph( &GraphDriver, &GraphMode, "C:\\APP\\TC\\BGI" );
  initgraph( &GraphDriver, &GraphMode, "BGI" );
  ErrorCode = graphresult();		/* Read result of initialization*/
  if( ErrorCode != grOk ){		/* Error occured during init	*/
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
  }

  getpalette( &palette );		/* Read the palette from board	*/
  MaxColors = getmaxcolor() + 1;	/* Read maximum number of colors*/

  getaspectratio( &xasp, &yasp );	/* read the hardware aspect	*/
  AspectRatio = (double)xasp / (double)yasp; /* Get correction factor	*/
  
  MaxX = getmaxx();
  MaxY = getmaxy();
  

}


int main(){

	int tmp, ball_counter=0,player_counter=0;
	int pl1_y = 20, pl2_y = 20;
	int ball_delay = 6, player_delay = 3;
	
	initialize();

	x=getmaxx()/2,y=getmaxy()/2;
    page = 0;
	
	pl1_score = pl2_score = 0;
    direction_y = GO_DOWN;
	direction_x = GO_LEFT;
	ball_color = DARKGRAY;

	last_pl1_pos_y[0] = getmaxy()/2;
	last_pl1_pos_y[1] = getmaxy()/2;;
	last_pl2_pos_y[0] = getmaxy()/2;;
	last_pl2_pos_y[1] = getmaxy()/2;;
		
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
					 closegraph();
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
							if(pl2_y < getmaxy() - PL_BAR_SIZE - 9 )
								pl2_y += 1;
					}
					
					if(get_key_status(o_Key)){ //o
							if(pl2_y  >   9  )
								pl2_y -= 1;
						}
						
						
					if(get_key_status(d_Key)){ //d
							if(pl1_y < getmaxy() - PL_BAR_SIZE - 9 )
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
		
		if(x + BALL_RADIUS/2 - 1 == getmaxx() - 20){
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


