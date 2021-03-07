/**************************************************************************
 * written by David Brackeen                                              *
 * http://www.brackeen.com/home/vga/                                      *
 *                                                                        *
 * This program will only work on DOS- or Windows-based systems with a    *
 * VGA, SuperVGA, or compatible video adapter.                            *
 *                                                                        *
 * Please feel free to copy this source code.                             *
 *                                                                        *
 **************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include <math.h>

#include "lvga.h"

fixed16_16 SIN_ACOS[1024];

byte *VGA=(byte *)0xA0000000L;        /* this points to video memory. */
word *my_clock=(word *)0x0000046C;    /* this points to the 18.2hz system
                                         clock. */
										 
bool double_buffer_enabled;										 
unsigned char *DOUBLE_BUFF;
										 
void initialize_graphics(bool dblbfr){
	int ret;
	
	double_buffer_enabled = dblbfr;
	DOUBLE_BUFF = (unsigned char *) malloc(320*200);
	if (DOUBLE_BUFF==NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}
	
	memset(DOUBLE_BUFF,0,SCREEN_SIZE);
	ret = set_mode(VGA_256_COLOR_MODE);       /* set the video mode. */
	//printf("RET:%d\n",ret);
	
}


/**************************************************************************
 *  show_buffer                                                           *
 *    displays a memory buffer on the screen                              *
 **************************************************************************/
void show_buffer()
{
  #ifdef VERTICAL_RETRACE
    while ((inp(INPUT_STATUS_1) & VRETRACE));
    while (!(inp(INPUT_STATUS_1) & VRETRACE));
  #endif
  memcpy(VGA,DOUBLE_BUFF,SCREEN_SIZE);
}
										 
										 
/**************************************************************************
 *  set_mode                                                              *
 *     Sets the video mode.                                               *
 **************************************************************************/

int set_mode(byte mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  return int86(VIDEO_INT, &regs, &regs);
}

/**************************************************************************
 *  plot_pixel                                                            *
 *    Plot a pixel by directly writing to video memory, with no           *
 *    multiplication.                                                     *
 **************************************************************************/

void plot_pixel(int x,int y,byte color)
{
	if(double_buffer_enabled){
		DOUBLE_BUFF[(y<<8)+(y<<6)+x]=color;
	}else{
		/*  y*320 = y*256 + y*64 = y*2^8 + y*2^6   */
		VGA[(y<<8)+(y<<6)+x]=color;
	}
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

	if(double_buffer_enabled){
		DOUBLE_BUFF[(py<<8)+(py<<6)+px]=color;
	}else{
		VGA[(py<<8)+(py<<6)+px]=color;
	}

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
	
	if(double_buffer_enabled){
		DOUBLE_BUFF[offset+dy-dxoffset] = color;  /* octant 0 */
		DOUBLE_BUFF[offset+dx-dyoffset] = color;  /* octant 1 */
		DOUBLE_BUFF[offset-dx-dyoffset] = color;  /* octant 2 */
		DOUBLE_BUFF[offset-dy-dxoffset] = color;  /* octant 3 */
		DOUBLE_BUFF[offset-dy+dxoffset] = color;  /* octant 4 */
		DOUBLE_BUFF[offset-dx+dyoffset] = color;  /* octant 5 */
		DOUBLE_BUFF[offset+dx+dyoffset] = color;  /* octant 6 */
		DOUBLE_BUFF[offset+dy+dxoffset] = color;  /* octant 7 */
	}else{
		VGA[offset+dy-dxoffset] = color;  /* octant 0 */
		VGA[offset+dx-dyoffset] = color;  /* octant 1 */
		VGA[offset-dx-dyoffset] = color;  /* octant 2 */
		VGA[offset-dy-dxoffset] = color;  /* octant 3 */
		VGA[offset-dy+dxoffset] = color;  /* octant 4 */
		VGA[offset-dx+dyoffset] = color;  /* octant 5 */
		VGA[offset+dx+dyoffset] = color;  /* octant 6 */
		VGA[offset+dy+dxoffset] = color;  /* octant 7 */
	}
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
		if(double_buffer_enabled){
			  DOUBLE_BUFF[offset+i -dxoffset] = color;  /* octant 0 */
			  DOUBLE_BUFF[offset+dx-dyoffset] = color;  /* octant 1 */
			  DOUBLE_BUFF[offset-dx-dyoffset] = color;  /* octant 2 */
			  DOUBLE_BUFF[offset-i -dxoffset] = color;  /* octant 3 */
			  DOUBLE_BUFF[offset-i +dxoffset] = color;  /* octant 4 */
			  DOUBLE_BUFF[offset-dx+dyoffset] = color;  /* octant 5 */
			  DOUBLE_BUFF[offset+dx+dyoffset] = color;  /* octant 6 */
			  DOUBLE_BUFF[offset+i +dxoffset] = color;  /* octant 7 */
		}else{
			  VGA[offset+i -dxoffset] = color;  /* octant 0 */
			  VGA[offset+dx-dyoffset] = color;  /* octant 1 */
			  VGA[offset-dx-dyoffset] = color;  /* octant 2 */
			  VGA[offset-i -dxoffset] = color;  /* octant 3 */
			  VGA[offset-i +dxoffset] = color;  /* octant 4 */
			  VGA[offset-dx+dyoffset] = color;  /* octant 5 */
			  VGA[offset+dx+dyoffset] = color;  /* octant 6 */
			  VGA[offset+i +dxoffset] = color;  /* octant 7 */
		}
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
	  if(double_buffer_enabled){
		DOUBLE_BUFF[top_offset+i]=color;
		DOUBLE_BUFF[bottom_offset+i]=color;
	  }else{
		VGA[top_offset+i]=color;
		VGA[bottom_offset+i]=color;
	  }
  }
  for(i=top_offset;i<=bottom_offset;i+=SCREEN_WIDTH)
  {
	  if(double_buffer_enabled){
			DOUBLE_BUFF[left+i]=color;
			DOUBLE_BUFF[right+i]=color;
	  }else{
			VGA[left+i]=color;
			VGA[right+i]=color;
	  }
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
	  if(double_buffer_enabled){
		  memset(&DOUBLE_BUFF[i],color,width);
	  }else{
		memset(&VGA[i],color,width);
	  }
  }
}

