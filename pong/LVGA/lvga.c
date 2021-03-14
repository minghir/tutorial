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
	int ret,i;
	
	for(i=0;i<1024;i++)                 /* create the sin(arccos(x)) table. */
	{
		SIN_ACOS[i]=sin(acos((float)i/1024))*0x10000L;
	}
	
	double_buffer_enabled = dblbfr;
	DOUBLE_BUFF = (unsigned char *) malloc(320*200);
	if (DOUBLE_BUFF==NULL)
	{
		printf("Not enough memory for double buffer.\n");
		exit(1);
	}
	
	memset(DOUBLE_BUFF,0,SCREEN_SIZE);
	set_mode(VGA_256_COLOR_MODE);       /* set the video mode. */
	//printf("RET:%d\n",ret);
}


 void exit_graphic(){
	 free(DOUBLE_BUFF);  
	 set_mode(TEXT_MODE); 
 }

/**************************************************************************
 *  fskip                                                                 *
 *     Skips bytes in a file.                                             *
 **************************************************************************/

void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
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
 *  set_palette                                                           *
 *    Sets all 256 colors of the palette.                                 *
 **************************************************************************/

void set_palette(byte *palette)
{
  int i;

  outp(PALETTE_INDEX,0);              /* tell the VGA that palette data
                                         is coming. */
  for(i=0;i<256*3;i++)
    outp(PALETTE_DATA,palette[i]);    /* write the data */
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
 *  circle_slow                                                           *
 *    Draws a circle by using floating point numbers and math fuctions.   *
 **************************************************************************/

void circle_slow(int x,int y, int radius, byte color)
{
  float n=0,invradius=1/(float)radius;
  int dx=0,dy=radius-1;
  word dxoffset,dyoffset,offset=(y<<8)+(y<<6)+x;

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
    dy=radius * sin(acos(n));
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


/**************************************************************************
 *  load_bmp                                                              *
 *    Loads a bitmap file into memory.                                    *
 **************************************************************************/

void load_bmp(char *file,BITMAP *b)
{
  FILE *fp;
  long index;
  word num_colors;
  int x;

  /* open the file */
  if ((fp = fopen(file,"rb")) == NULL)
  {
    printf("Error opening file %s.\n",file);
    exit(1);
  }

  /* check to see if it is a valid bitmap file */
  if (fgetc(fp)!='B' || fgetc(fp)!='M')
  {
    fclose(fp);
    printf("%s is not a bitmap file.\n",file);
    exit(1);
  }

  /* read in the width and height of the image, and the
     number of colors used; ignore the rest */
  fskip(fp,16);
  fread(&b->width, sizeof(word), 1, fp);
  fskip(fp,2);
  fread(&b->height,sizeof(word), 1, fp);
  fskip(fp,22);
  fread(&num_colors,sizeof(word), 1, fp);
  fskip(fp,6);

  /* assume we are working with an 8-bit file */
  if (num_colors==0) num_colors=256;

  /* try to allocate memory */
  if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
  {
    fclose(fp);
    printf("Error allocating memory for file %s.\n",file);
    exit(1);
  }

  /* read the palette information */
  for(index=0;index<num_colors;index++)
  {
    b->palette[(int)(index*3+2)] = fgetc(fp) >> 2;
    b->palette[(int)(index*3+1)] = fgetc(fp) >> 2;
    b->palette[(int)(index*3+0)] = fgetc(fp) >> 2;
    x=fgetc(fp);
  }

  /* read the bitmap */
  for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
    for(x = 0; x < b->width; x++)
      b->data[(int)(index+x)]=(byte)fgetc(fp);

  fclose(fp);
}

/**************************************************************************
 *  draw_bitmap                                                           *
 *    Draws a bitmap.                                                     *
 **************************************************************************/

void draw_bitmap(BITMAP *bmp,int x,int y)
{
  int j;
  word screen_offset = (y<<8)+(y<<6)+x;
  word bitmap_offset = 0;

  for(j=0;j<bmp->height;j++)
  {
    //memcpy(&VGA[screen_offset],&bmp->data[bitmap_offset],bmp->width);
	 if(double_buffer_enabled){
		  //memset(&DOUBLE_BUFF[i],color,width);
		memcpy(&DOUBLE_BUFF[screen_offset],&bmp->data[bitmap_offset],bmp->width);
	  }else{
		memcpy(&VGA[screen_offset],&bmp->data[bitmap_offset],bmp->width);
	  }

    bitmap_offset+=bmp->width;
    screen_offset+=SCREEN_WIDTH;
  }
}

/**************************************************************************
 *  draw_transparent_bitmap                                               *
 *    Draws a transparent bitmap.                                         *
 **************************************************************************/

void draw_transparent_bitmap(BITMAP *bmp,int x,int y)
{
  int i,j;
  word screen_offset = (y<<8)+(y<<6);
  word bitmap_offset = 0;
  byte data;

  for(j=0;j<bmp->height;j++)
  {
    for(i=0;i<bmp->width;i++,bitmap_offset++)
    {
      data = bmp->data[bitmap_offset];
      if (data) VGA[screen_offset+x+i] = data;
    }
    screen_offset+=SCREEN_WIDTH;
  }
}

/**************************************************************************
 *  wait                                                                  *
 *    Wait for a specified number of clock ticks (18hz).                  *
 **************************************************************************/

void wait(int ticks)
{
  word start;

  start=*my_clock;

  while (*my_clock-start<ticks)
  {
    *my_clock=*my_clock;              /* this line is for some compilers
                                         that would otherwise ignore this
                                         loop */
  }
}



/////////////////////////////////////////////
/* VGA256.C
   The source file for the functions that render dots, circles, lines
   and text in VGA Mode 13H.

   The video mode capable of displaying 320x200 pixels in 256 colors.

   This was written by Gary Wilkerson Jr for use by the entire gaming
   community.  This code is not copyrighted nor do I expect any of it
   to be copyrighted.  This code is to be 'common knowledge' among the
   gaming community.

   You are free to use and modify it in any way as you see fit.

   Happy programming.
*/

// https://www.oocities.org/garyneal_71/OldPages/cGraphicsText.html
/* Points to all characters in ROM character set */
void put_ch(int x, int y, int c, int fc, int bc){
	unsigned char far *charSet=(unsigned char far *)0xF000FA6EL;
	int CharHeight = 8;
	int CharWidth = 8;
	
    char far *workChar;  /* Points to character rendered */
    unsigned char bit;   /* Bit mask */
    int xOff, yOff;      /* Offset from x, y coordinate */

    /* Get character to be rendered */
    workChar = &charSet[(c & 255) * CharHeight];
    for (yOff=0; yOff<CharHeight; yOff++) {
        if (y + yOff < SCREEN_HEIGHT) {
            bit = 0x80;
            for (xOff=0; xOff<CharWidth; xOff++) {
                if (x + xOff < SCREEN_WIDTH) {
                    if (*workChar & bit)
                        plot_pixel(x + xOff, y + yOff, fc);
                    else if (bc >= 0)
                        plot_pixel(x + xOff, y + yOff, bc);
                }
                bit >>= 1;
            }
        }
        workChar++;
    }
}

void put_str(int x, int y, char *string, int fc, int bc){
    int i;

    /* Call Gputch for each character of the string */
    for (i=0; string[i]; i++)
        put_ch(x + (i << 3), y, string[i], fc, bc);
}


void Circle(int x, int y, int radius, unsigned char color)
{
	
    int a, b, d;
    int xx1, yy1, xx2, yy2;
    int xy1, yx1, xy2, yx2;
    int XX1, YY1, XX2, YY2;
    int XY1, YX1, XY2, YX2;

	//y = y + aratio;

    a = 0;
    b = radius;
    d = (1 - radius) * 2;

    xx1 = x;
    yy1 = y + b;
    xx2 = x;
    yy2 = y - b;
    xy1 = x + b;
    yx1 = y;
    xy2 = x - b;
    yx2 = y;

    while (b >= a) {
        if (xx1 < 0) XX1 = 0;
        else if (xx1 >= SCREEN_WIDTH) XX1 = SCREEN_WIDTH - 1;
        else XX1 = xx1;

        if (yy1 < 0) YY1 = 0;
        else if (yy1 >= SCREEN_HEIGHT) YY1 = SCREEN_HEIGHT - 1;
        else YY1 = yy1;

        if (xx2 < 0) XX2 = 0;
        else if (xx2 >= SCREEN_WIDTH) XX2 = SCREEN_WIDTH - 1;
        else XX2 = xx2;

        if (yy2 < 0) YY2 = 0;
        else if (yy2 >= SCREEN_HEIGHT) YY2 = SCREEN_HEIGHT - 1;
        else YY2 = yy2;

        if (xy1 < 0) XY1 = 0;
        else if (xy1 >= SCREEN_WIDTH) XY1 = SCREEN_WIDTH - 1;
        else XY1 = xy1;

        if (yx1 < 0) YX1 = 0;
        else if (yx1 >= SCREEN_HEIGHT) YX1 = SCREEN_HEIGHT - 1;
        else YX1 = yx1;

        if (xy2 < 0) XY2 = 0;
        else if (xy2 >= SCREEN_WIDTH) XY2 = SCREEN_WIDTH - 1;
        else XY2 = xy2;

        if (yx2 < 0) YX2 = 0;
        else if (yx2 >= SCREEN_HEIGHT) YX2 = SCREEN_HEIGHT - 1;
        else YX2 = yx2;

        plot_pixel(XX1, YY1, color);
        plot_pixel(XX1, YY2, color);
        plot_pixel(XX2, YY1, color);
        plot_pixel(XX2, YY2, color);
        plot_pixel(XY1, YX1, color);
        plot_pixel(XY1, YX2, color);
        plot_pixel(XY2, YX1, color);
        plot_pixel(XY2, YX2, color);
        if (d + b > 0) {
            b--;
            d -= (b * 2) + 1;
            yy1--;
            yy2++;
            xy1--;
            xy2++;
        }
        if (a > d) {
            a++;
	        d += (a * 2) + 1;
           xx1++;
           xx2--;
           yx1++;
           yx2--;
        }
    }
}

/////////////////////////////////////////////
