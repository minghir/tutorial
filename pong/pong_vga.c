
#include <stdio.h>
//#include <graphics.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include <math.h>

#define GO_UP 1
#define GO_DOWN 0
#define GO_RIGHT 1
#define GO_LEFT 0

#define BALL_RADIUS 5
#define PL_BAR_SIZE 46
#define PL1_COLOR 49
#define PL2_COLOR 60

#define e_Key 0x12
#define d_Key 0x20
#define o_Key 0x18
#define l_Key 0x26
#define a_Key 0x1E
#define s_Key 0x1F
#define q_Key 0x10

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a
                                         number */
typedef unsigned char  byte;
typedef unsigned short word;
typedef long           fixed16_16;

fixed16_16 SIN_ACOS[1024];

byte *VGA=(byte *)0xA0000000L;        /* this points to video memory. */
word *my_clock=(word *)0x0000046C;    /* this points to the 18.2hz system
                                         clock. */
										 
/**************************************************************************
 *  set_mode                                                              *
 *     Sets the video mode.                                               *
 **************************************************************************/

void set_mode(byte mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  int86(VIDEO_INT, &regs, &regs);
}

/**************************************************************************
 *  plot_pixel                                                            *
 *    Plot a pixel by directly writing to video memory, with no           *
 *    multiplication.                                                     *
 **************************************************************************/

void plot_pixel(int x,int y,byte color)
{
  /*  y*320 = y*256 + y*64 = y*2^8 + y*2^6   */
  VGA[(y<<8)+(y<<6)+x]=color;
}		

/**************************************************************************
 *  line_fast                                                             *
 *    draws a line using Bresenham's line-drawing algorithm, which uses   *
 *    no multiplication or division.                                      *
 **************************************************************************/

void line_fast(int x1, int y1, int x2, int y2, byte color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  VGA[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      plot_pixel(px,py,color);
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      plot_pixel(px,py,color);
    }
  }
}

/**************************************************************************
 *  circle_fast                                                           *
 *    Draws a circle by using fixed point numbers and a trigonometry      *
 *    table.                                                              *
 **************************************************************************/

void circle_fast(int x,int y, int radius, byte color)
{
  fixed16_16 n=0,invradius=(1/(float)radius)*0x10000L;
  int dx=0,dy=radius-1;
  word dxoffset,dyoffset,offset = (y<<8)+(y<<6)+x;

  while (dx<=dy)
  {
    dxoffset = (dx<<8) + (dx<<6);
    dyoffset = (dy<<8) + (dy<<6);
    VGA[offset+dy-dxoffset] = color;  /* octant 0 */
    VGA[offset+dx-dyoffset] = color;  /* octant 1 */
    VGA[offset-dx-dyoffset] = color;  /* octant 2 */
    VGA[offset-dy-dxoffset] = color;  /* octant 3 */
    VGA[offset-dy+dxoffset] = color;  /* octant 4 */
    VGA[offset-dx+dyoffset] = color;  /* octant 5 */
    VGA[offset+dx+dyoffset] = color;  /* octant 6 */
    VGA[offset+dy+dxoffset] = color;  /* octant 7 */
    dx++;
    n+=invradius;
    dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
  }
}

/**************************************************************************
 *  circle_fill                                                           *
 *    Draws and fills a circle.                                           *
 **************************************************************************/

void circle_fill(int x,int y, int radius, byte color)
{
  fixed16_16 n=0,invradius=(1/(float)radius)*0x10000L;
  int dx=0,dy=radius-1,i;
  word dxoffset,dyoffset,offset = (y<<8)+(y<<6)+x;

  while (dx<=dy)
  {
    dxoffset = (dx<<8) + (dx<<6);
    dyoffset = (dy<<8) + (dy<<6);
    for(i=dy;i>=dx;i--,dyoffset-=SCREEN_WIDTH)
    {
      VGA[offset+i -dxoffset] = color;  /* octant 0 */
      VGA[offset+dx-dyoffset] = color;  /* octant 1 */
      VGA[offset-dx-dyoffset] = color;  /* octant 2 */
      VGA[offset-i -dxoffset] = color;  /* octant 3 */
      VGA[offset-i +dxoffset] = color;  /* octant 4 */
      VGA[offset-dx+dyoffset] = color;  /* octant 5 */
      VGA[offset+dx+dyoffset] = color;  /* octant 6 */
      VGA[offset+i +dxoffset] = color;  /* octant 7 */
    }
    dx++;
    n+=invradius;
    dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
  }
}

/**************************************************************************
 *  rect_fast                                                             *
 *    Draws a rectangle by drawing all lines by itself.                   *
 **************************************************************************/

void rect_fast(int left,int top, int right, int bottom, byte color)
{
  word top_offset,bottom_offset,i,temp;

  if (top>bottom)
  {
    temp=top;
    top=bottom;
    bottom=temp;
  }
  if (left>right)
  {
    temp=left;
    left=right;
    right=temp;
  }

  top_offset=(top<<8)+(top<<6);
  bottom_offset=(bottom<<8)+(bottom<<6);

  for(i=left;i<=right;i++)
  {
    VGA[top_offset+i]=color;
    VGA[bottom_offset+i]=color;
  }
  for(i=top_offset;i<=bottom_offset;i+=SCREEN_WIDTH)
  {
    VGA[left+i]=color;
    VGA[right+i]=color;
  }
}

/**************************************************************************
 *  rect_fill                                                             *
 *    Draws and fills a rectangle.                                        *
 **************************************************************************/

void rect_fill(int left,int top, int right, int bottom, byte color)
{
  word top_offset,bottom_offset,i,temp,width;

  if (top>bottom)
  {
    temp=top;
    top=bottom;
    bottom=temp;
  }
  if (left>right)
  {
    temp=left;
    left=right;
    right=temp;
  }

  top_offset=(top<<8)+(top<<6)+left;
  bottom_offset=(bottom<<8)+(bottom<<6)+left;
  width=right-left+1;

  for(i=top_offset;i<=bottom_offset;i+=SCREEN_WIDTH)
  {
    memset(&VGA[i],color,width);
  }
}


								 

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
