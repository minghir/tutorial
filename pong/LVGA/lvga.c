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

